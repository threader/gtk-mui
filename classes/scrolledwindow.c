/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
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
 * $Id: scrolledwindow.c,v 1.2 2007/10/25 15:23:48 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data {
  Object *virt;
  Object *scroll;
};

/*******************************************
 * mNew
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  Object *scroll;
  struct Data *data;

  DebOut("mNew (scrolledwindow)\n");

  obj = (APTR) DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("scrolledwindow: unable to create object!");
    return (ULONG) NULL;
  }

  data = (struct Data *)INST_DATA(cl, obj);
  data->scroll=NULL; /* as long as this is NULL, we are in mNew */

  DebOut("  new obj=%lx\n",obj);

  scroll=ScrollgroupObject,
                MUIA_Scrollgroup_FreeHoriz, TRUE,
                MUIA_Scrollgroup_FreeVert, TRUE,
                MUIA_Scrollgroup_Contents, data->virt=VirtgroupObject,
                                                      End,
              End;
              
  if(!scroll) {
    ErrOut("scrolledwindow: unable to create scroll obj!");
    DisposeObject(obj);
    return (ULONG) NULL;
  }

  DoMethod(obj,OM_ADDMEMBER,(LONG) scroll);
  data->scroll=scroll; /* init done */

  DebOut("==> mNew data->scroll=%lx\n",data->scroll);

  return (ULONG)obj;
}

static ULONG mAddMember(struct IClass *cl, APTR obj, struct opMember *msg, struct Data *data) {

  DebOut("scrolledwindow.c: mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->scroll) {
    /* now someone added a new object to us, so add it to the vrt group */
    DebOut("  data->virt,OM_ADDMEMBER,%lx\n",(ULONG) msg->opam_Object);
    DoMethod(data->virt,OM_ADDMEMBER,(ULONG) msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->virt, we just pass it to the original method */
    DebOut("  init => DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID) {
    case OM_NEW          : return mNew           (cl, obj, msg);
    case OM_ADDMEMBER    : return mAddMember     (cl, obj, (APTR) msg, data);
#if 0
    case MUIM_AskMinMax  : return mgtk_askminmax (cl,   obj, (APTR)msg, AskMin_Unlimited, AskMin_Unlimited);
#endif
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_scrolledwindow_class(void) {

  DebOut("mgtk_create_scrolledwindow_class()\n");

  CL_Scrolledwindow = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Scrolledwindow=%lx\n",CL_Scrolledwindow);

  return CL_Scrolledwindow ? 1 : 0;
}

void mgtk_delete_scrolledwindow_class(void) {

  if (CL_Button)
  {
    MUI_DeleteCustomClass(CL_Scrolledwindow);
    CL_Scrolledwindow=NULL;
  }
}
