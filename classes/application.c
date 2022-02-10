/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Oliver Brunner, Ilkka Lehtoranta
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: gtk-mui "at" oliver-brunner.de
 *
 * $Id: application.c,v 1.15 2012/07/09 09:30:09 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <devices/timer.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

enum
{
  G_SOURCE_TIMEOUT_ID = 0,
};

struct gm_source
{
  ULONG id;
  APTR  ptr;
};

struct TimerReq
{
  struct timerequest timer;
  struct MinNode link;
  ULONG    interval;
  guint    (*function)(gpointer);
  gpointer parameter;

  struct gm_source source;
};

struct LonerNode
{
  struct MinNode link;
  APTR loner;
};

struct Data
{
  APTR  pool;

  struct MinList      lonerlist;

  struct MinList      timerlist;
  struct MsgPort      *port;
  struct timerequest  timereq;
  struct MUI_InputHandlerNode ihnode;
};

STATIC ULONG mNew(struct IClass *cl, APTR obj, struct opSet *msg)
{
  struct TagItem *tag;
  CONST_STRPTR title;
  TEXT basetext[30];  // this is approx. max recommended by MUI
  APTR pool;

  pool  = CreatePool(MEMF_PUBLIC, 4096, 4000);

  if (!pool)
    return 0;

  // Build application

	strcpy(basetext, "GTKPROGRAM");

	tag   = FindTagItem(MA_GtkApplication_Title, msg->ops_AttrList);
	title = "GTK Application";

	// Build MUIA_Application_Title and MUIA_Application_Base

	if (tag && tag->ti_Data)
	{
		CONST_STRPTR str = (CONST_STRPTR)tag->ti_Data;

		if (*str)
		{
			char c;
			int i = 3;

			title = str;	// or maybe sprintf("GTK %s", str); ?

			do
			{
				c = *str++;

				if (c == ' ')
					c = '_';

				if ((c >= '0' && c <= 'z') || c == '_')
				{
					if (c >= 'a')
						c -= 'a' - 'A';

					basetext[i] = c;
					i++;
				}
			}
			while (c && i < 29);

			basetext[i] = 0;	// NULL terminate
		}
	}

	obj = DoSuperNew(cl, obj,
		MUIA_Application_Title, title,
		#ifndef __AROS__
		MUIA_Application_Base, basetext,
		#endif
		TAG_MORE, msg->ops_AttrList);

  if (obj)
  {
    GETDATA;
    data->pool = pool;

    MGTK_NEWLIST(data->lonerlist);
  }

	return (ULONG)obj;
}

/*******************************************
 * mDispose
 *******************************************/
STATIC VOID mDispose(struct Data *data, APTR obj)
{
  struct LonerNode *loner;
  struct List *windowlist;
  APTR objstate;

  DebOut("application:mDispose\n");

  if (data->port) {
    struct timerequest *timer;

    ForeachNode(&data->timerlist, timer) {
      timer--;

      AbortIO((struct IORequest *)timer);
      WaitIO((struct IORequest *)timer);
    }

    DoMethod(obj, MUIM_Application_RemInputHandler, (ULONG) &data->ihnode);
    CloseDevice((struct IORequest *)&data->timereq);
    DeleteMsgPort(data->port);
  }

  DebOut("application: MsgPorts freed\n");

  ForeachNode(&data->lonerlist, loner) {
    DebOut("  MUI_DisposeObject Loner %lx\n",loner->loner);
    MUI_DisposeObject(loner->loner);
  }

  DebOut("application: Loners freed\n");

  // we should use GetSuperAttr() instead
  windowlist = (struct List *)xget(obj, MUIA_Application_WindowList);
  objstate = windowlist->lh_Head;

#warning set(winobj, MA_GtkWindow_DestroyWithParent, FALSE); is TODO!
#ifdef __MoORPHOS__
  while ((winobj = NextObject(&winobj))) {
    // avoid double dispose
    set(winobj, MA_GtkWindow_DestroyWithParent, FALSE);
  }

  DebOut("application: windows unlinked\n");
#endif

  DeletePool(data->pool);

  DebOut("application: the pool is clean now\n");

  DebOut("application: disposed\n");
}

/*******************************************
 * mAddLoner
 *******************************************/
STATIC ULONG mAddLoner(struct Data *data, struct MUIP_GtkApplication_AddLoner *msg)
{
  struct LonerNode *loner = AllocPooled(data->pool, sizeof(*loner));

  DebOut("application:mAddLoner(%lx)\n",loner);

  if (loner) {
    loner->loner = msg->lonely_object;
    ADDTAIL(&data->lonerlist, loner);
  }

  return (ULONG)loner;
}

/*******************************************
 * mAddTimer
 *******************************************/
STATIC ULONG mAddTimer(struct Data *data, APTR obj, struct MUIP_GtkApplication_AddTimer *msg)
{
  struct TimerReq *req;

  if (!data->port) {
    data->port = CreateMsgPort();

    if (!data->port) {
      ErrOut("classes/application.c: unable to CreateMsgPort\n");
      return 0;
    }

    MGTK_NEWLIST(data->timerlist);

    data->ihnode.ihn_Object  = obj;
    data->ihnode.ihn_Signals = 1 << data->port->mp_SigBit;
    data->ihnode.ihn_Method  = MM_GtkApplication_TimerEvent;

    data->timereq.tr_node.io_Message.mn_ReplyPort = data->port;
    data->timereq.tr_node.io_Message.mn_Length = sizeof(struct timerequest);
#ifdef __AROS__
    /* AROS seems not to like UNIT_MICROHZ .. */
    if (OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)&data->timereq, 0)) 
#else
    if (OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)&data->timereq, 0)) 
#endif
      {
      ErrOut("classes/application.c: unable to open timer.device\n");
      DeleteMsgPort(data->port);
      data->port = NULL;
      return 0;
    }

    DebOut("timer device is open!\n");
    DoMethod(obj, MUIM_Application_AddInputHandler, (ULONG) &data->ihnode);
  }

  // we could use CreateIORequest() but since we copy original over anyway it doesnt matter...

  req = AllocPooled(data->pool, sizeof(*req));

  DebOut("new req: %lx\n", req);

  if (req) {
    ULONG interval = msg->interval;

    CopyMemQuick(&data->timereq, &req->timer, sizeof(struct timerequest));

    ADDTAIL(&data->timerlist, &req->link);

    req->interval = interval;
    req->function = (guint (*)(gpointer))msg->function;
    req->parameter = msg->parameter;

    req->timer.tr_node.io_Command = TR_ADDREQUEST;
    req->timer.tr_time.tv_secs = interval / 1000;
    req->timer.tr_time.tv_micro = (interval % 1000) * 1000;
    SendIO((struct IORequest *)req);

    req->source.id  = G_SOURCE_TIMEOUT_ID;
    req->source.ptr = req;
    return (ULONG)&req->source;
  }

  return 0;
}

/*******************************************
 * mRemTimer
 *******************************************/
STATIC ULONG mRemTimer(struct Data *data, APTR obj, struct MUIP_GtkApplication_RemTimer *msg) {
  struct TimerReq *req = ((struct gm_source *)msg->source)->ptr;

  DebOut("entered(req %lx)\n", req);

  if (!CheckIO(req)) {
    AbortIO(req);
  }

  DebOut("WaitIO ..\n");
  WaitIO(req);

  DebOut("req %lx removed\n", req);

  REMOVE(&req->link);
  FreePooled(data->pool, req, sizeof(*req));

  return 1;
}

/*******************************************
 * mTimerEvent
 *
 * If req->parameter is TRUE, set the
 *  timer again. 
 * If FALSE, we are done
 *  with this timer, remove it from our
 *  list and free the req memory.
 *******************************************/
STATIC ULONG mTimerEvent(struct Data *data) {
  struct TimerReq *req;

  while ((req = (struct TimerReq *)GetMsg(data->port))) {

    if (req->function(req->parameter)) {
      req->timer.tr_node.io_Command = TR_ADDREQUEST;
      req->timer.tr_time.tv_secs = req->interval / 1000;
      req->timer.tr_time.tv_micro = (req->interval % 1000) * 1000;
      SendIO((struct IORequest *)req);
    }
    else {

      REMOVE(&req->link);
      FreePooled(data->pool, req, sizeof(*req));
    }
  }

  return 1;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

 // DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID)
  {
    case OM_DISPOSE : mDispose(data, obj); break;
    case OM_NEW     : return mNew(cl, obj, (APTR)msg);
    case MM_GtkApplication_AddLoner   : return mAddLoner(data, (APTR)msg);
    case MM_GtkApplication_AddTimer   : return mAddTimer(data, obj, (APTR)msg);
    case MM_GtkApplication_RemTimer   : return mRemTimer(data, obj, (APTR)msg);
    case MM_GtkApplication_TimerEvent : return mTimerEvent(data);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_application_class(void)
{
  CL_Application = MUI_CreateCustomClass(NULL, MUIC_Application, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Application ? 1 : 0;
}

void mgtk_delete_application_class(void)
{
  if (CL_Application)
  {
    MUI_DeleteCustomClass(CL_Application);
    CL_Application=NULL;
  }
}
