/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Ilkka Lehtoranta
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
 * $Id: dialogwindow.c,v 1.7 2007/10/25 15:23:48 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct WidgetNode
{
  struct MinNode node;
  APTR obj;
  LONG resp;
};

struct Data
{
  struct MinList widgetlist;
  LONG response;
};

/*******************************************
 * mNew
 *******************************************/
STATIC ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  obj = (APTR)DoSuperNew(cl, obj,
    MUIA_Window_CloseGadget, FALSE,
    TAG_MORE, ((struct opSet *)msg)->ops_AttrList);

  if (obj)
  {
    GETDATA;
    MGTK_NEWLIST(data->widgetlist);
  }

  return (ULONG)obj;
}

/*******************************************
 * mDispose
 *******************************************/
STATIC VOID mDispose(struct Data *data)
{
  struct WidgetNode *node;

  ForeachNode(&data->widgetlist, node)
  {
    REMOVE(node);
    g_free(node);
  }
}

/*******************************************
 * mSet
 *******************************************/
STATIC VOID mSet(struct Data *data, APTR obj, struct opSet *msg)
{
  struct TagItem *tstate, *tag;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_GtkDialog_DefaultResponse:
      {
        struct WidgetNode *node;
        LONG def = tag->ti_Data;

        ForeachNode(&data->widgetlist, node)
        {
          if (node->resp == def)
          {
            set(obj, MUIA_Window_ActiveObject, node->obj);
            break;
          }
        }
      }
      break;

      case MA_GtkDialog_Response:
        data->response = tag->ti_Data;
        DoMethod(_app(obj), MUIM_Application_ReturnID, MV_Dialog_ReturnID_Done);
        break;
    }
  }
}

/*******************************************
 * mGet
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID)
  {
    case MA_GtkDialog_Response:
      rc = data->response;
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * mAddWidget
 *******************************************/
STATIC ULONG mAddWidget(struct Data *data, struct MUIP_GtkDialog_AddWidget *msg)
{
  struct WidgetNode *node;

  node = g_new(struct WidgetNode, 1);

  if (node)
  {
    node->obj = msg->WidgetObj;
    node->resp = msg->Response;
    ADDTAIL(&data->widgetlist, node);
  }

  /* Monitor this widget. It must support MA_GtkWidget_Activate in order to work.
   * Currently only buttons do.
  */

  DoMethod(msg->WidgetObj, MUIM_Notify, MA_GtkWidget_Activate, TRUE, MUIV_Notify_Window, 3, MUIM_Set, MA_GtkDialog_Response, msg->Response);

  return 0;
}

/*******************************************
 * mEnableResponse
 *******************************************/
STATIC ULONG mEnableResponse(struct Data *data, struct MUIP_GtkDialog_EnableResponse *msg)
{
  struct WidgetNode *node;
  LONG resp = msg->Response;

  ForeachNode(&data->widgetlist, node)
  {
    if (node->resp == resp)
    {
      set(node->obj, MUIA_Disabled, !msg->Enable);
      break;
    }
  }

  return 0;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID)
  {
    case OM_DISPOSE     :        mDispose   (data); break;
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET                     : return mGet           (data, obj, (APTR)msg, cl);
    case MM_GtkDialog_AddWidget     : return mAddWidget     (data, (APTR)msg);
    case MM_GtkDialog_EnableResponse: return mEnableResponse(data, (APTR)msg);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_dialog_class(void)
{
  CL_Dialog = MUI_CreateCustomClass(NULL, NULL, CL_Window, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Dialog ? 1 : 0;
}

void mgtk_delete_dialog_class(void)
{
  if (CL_Dialog)
  {
    MUI_DeleteCustomClass(CL_Dialog);
    CL_Dialog=NULL;
  }
}
