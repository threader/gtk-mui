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
 * $Id: menu.c,v 1.8 2007/06/20 13:49:31 o1i Exp $
 *
 *****************************************************************************/

/* THIS DOES NOT WORK!
 * NO SIGNAL IS TRIGGERED!! TODO!!
 */

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

struct Data
{
};


/*******************************************
 * MUIHook_menu
 *******************************************/
#if 0
HOOKPROTO(MUIHook_menu, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_menu(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  DebOut("emit activate signal for %lx\n",widget);
  g_signal_emit_by_name(widget,"activate",0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_menu, MUIHook_menu);
#endif

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew (menu)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("menu: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/menu.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%lx\n",widget);

  if (obj) {
    /* setup internal hooks 
     * If the user selects an item, the object will receive
     * a MUIM_ContextMenuChoice method containing the selected
     * menuitem object. 
     *
     * We don't have to care for that here!
     */ 
#if 0
    DoMethod(obj, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (ULONG) global_mgtk->MuiApp,2,MUIM_CallHook,(ULONG) &MyMuiHook_menu);
#endif
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 *******************************************/
#if 0
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);
  struct TagItem *tstate, *tag;
  GtkWidget *widget;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Menuitem_Trigger:
        break;
    }
  }
}
#endif

static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) { 

  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Menu_Is_Item : rc = TRUE; break;

    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

#if 0
static ULONG mContextMenu(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {

  ErrOut("Menu mContextMenu!\n");

  return DoSuperMethodA(cl, obj, msg);
}
#endif
/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  DebOut("menu->msg->MethodID: %lx\n",(ULONG) msg->MethodID);

  switch (msg->MethodID) {
    case OM_NEW                 : return mNew          (cl, obj, msg);
    case OM_GET                 : return mGet          (data, obj, (APTR)msg, cl);
#if 0
    case MUIM_ContextMenuChoice : return mContextMenu  (data, obj, (APTR)msg, cl);
#endif
#if 0
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
#endif

  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_menu_class(void)
{
  DebOut("mgtk_create_menu_class()\n");

  CL_Menu = MUI_CreateCustomClass(NULL, MUIC_Menuitem, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Menu=%lx\n",CL_Menu);

  return CL_Menu ? 1 : 0;
}

void mgtk_delete_menu_class(void)
{
  if (CL_Menu)
  {
    MUI_DeleteCustomClass(CL_Menu);
    CL_Menu=NULL;
  }
}
