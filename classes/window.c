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
 * $Id: window.c,v 1.9 2011/07/04 12:16:21 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <proto/utility.h>
#include <mui.h>

//#define MGTK_DEBUG 1
#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct ChildWindow {
  struct MinNode  link;
  APTR            window;
};

struct Data {
  struct MinList    childlist;    // child windows
  APTR              parentwindow; // our parent window or NULL if no parent
  ULONG             destroywithparent;
  ULONG             window_id;
  CONST_STRPTR      role;

  gboolean          is_modal;
  gboolean          is_decorated;
  GdkWindowTypeHint typehint;
  GdkPixbuf					*icon;			// not really supported
};

/*******************************************
 * MUIHook_win_close
 *
 * emit delete_event on close
 *******************************************/
HOOKPROTO(MUIHook_win_close, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer) {

  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_win_close called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%lx\n",(int) widget);

  DebOut("  emit: g_signal_emit_by_name(%lx,delete_event)\n",widget);
  g_signal_emit_by_name (GTK_OBJECT (widget), "delete_event");

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_win_close, MUIHook_win_close);

/*******************
 * mNew
 *******************/
STATIC ULONG mNew(struct IClass *cl, APTR obj, APTR msg) {

  DebOut("classes/window.c: mNew\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (obj) {
    GETDATA;
#ifdef __AROS__
    MGTK_NEWLIST(data->childlist);
#else
    MGTK_NEWLIST(data->childlist); /*  was MGTK_NEWLIST(&data->childlist); ??*/
#endif
    data->is_decorated = 1;
  }

  DebOut(" win obj: %lx\n",obj);

  DoMethod(obj,MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (ULONG) obj, 2, MUIM_CallHook, (ULONG) &MyMuiHook_win_close);

  return (ULONG)obj;
}

STATIC VOID mDispose(struct Data *data, APTR obj, struct IClass *cl)
{
  struct ChildWindow *child;
  APTR app = (APTR)xget(obj, MUIA_ApplicationObject);

  DebOut("classes/window.c: mDispose mui object %lx\n", obj);

  ForeachNode(&data->childlist, child)
  {
    struct Data *cdata = (struct Data *)INST_DATA(cl, child->window);

    if (cdata->destroywithparent)
    {
		DebOut("classes/window.c: mDispose child window %lx\n", obj);
      DoMethod(app, OM_REMMEMBER, (ULONG) child->window);
      MUI_DisposeObject(child->window);
    }

    mgtk_freemem(child, sizeof(*child));
  }
}

STATIC ULONG calc_id(CONST_STRPTR str) {

  ULONG id = 0;

  while (*str) {
    id += *str++;
  }

	return (('W' << 24) | (id & 0xffffff));
}

/*******************************************
 * mSet
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg)
{
  struct TagItem *tstate, *tag;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {

    switch (tag->ti_Tag) {

      case MUIA_Window_Open:
        if (data->is_modal) {
          set((APTR)xget(obj, MUIA_ApplicationObject), MUIA_Application_Sleep, tag->ti_Data);
        }
        break;

      case MUIA_Window_Title:
        if (data->window_id == 0 && tag->ti_Data) {
          set(obj, MUIA_ObjectID, calc_id((CONST_STRPTR)tag->ti_Data));
        }
        break;

      case MA_GtkWindow_Decorated:
        data->is_decorated = tag->ti_Data;
        SetAttrs(obj,
            MUIA_Window_CloseGadget, tag->ti_Data,
            MUIA_Window_DepthGadget, tag->ti_Data,
            MUIA_Window_DragBar, tag->ti_Data,
            MUIA_Window_SizeGadget, tag->ti_Data,
            TAG_DONE);
        break;

      case MA_GtkWindow_DestroyWithParent:
        data->destroywithparent = tag->ti_Data;
        break;

      case MA_GtkWindow_Icon:
        data->icon = (GdkPixbuf *)tag->ti_Data;
      break;

      case MA_GtkWindow_Modal:
        // switch off "modal mode" if window is open, however currently switching on
        // the modal mode doesnt work

        if (tag->ti_Data == FALSE && data->is_modal && xget(obj, MUIA_Window_Open))
        {
          set((APTR)xget(obj, MUIA_ApplicationObject), MUIA_Application_Sleep, FALSE);
        }

        data->is_modal = tag->ti_Data;
        break;

			case MA_GtkWindow_Role:
				data->role = (CONST_STRPTR)tag->ti_Data;
				data->window_id = calc_id(data->role);
				set(obj, MUIA_ObjectID, data->window_id);
				break;

			case MA_GtkWindow_TypeHint:
				data->typehint = tag->ti_Data;
				switch (data->typehint)
				{
					case GDK_WINDOW_TYPE_HINT_MENU	:
					case GDK_WINDOW_TYPE_HINT_TOOLBAR:
					case GDK_WINDOW_TYPE_HINT_UTILITY:
					case GDK_WINDOW_TYPE_HINT_NORMAL	:
					case GDK_WINDOW_TYPE_HINT_DIALOG	:
						SetAttrs(obj,
							MUIA_Window_CloseGadget, TRUE,
							MUIA_Window_DepthGadget, TRUE,
							MUIA_Window_DragBar, TRUE,
							MUIA_Window_SizeGadget, TRUE,
							TAG_DONE);
						set((Object *)xget(obj, MUIA_Window_RootObject), MUIA_Background, data->typehint == GDK_WINDOW_TYPE_HINT_NORMAL ? MUII_WindowBack : MUII_RequesterBack);
						break;

					case GDK_WINDOW_TYPE_HINT_DESKTOP      :
					case GDK_WINDOW_TYPE_HINT_SPLASHSCREEN	:
					case GDK_WINDOW_TYPE_HINT_DOCK         :
						SetAttrs(obj,
							MUIA_Window_Backdrop, data->typehint == GDK_WINDOW_TYPE_HINT_DESKTOP ? TRUE : FALSE,
							MUIA_Window_CloseGadget, FALSE,
							MUIA_Window_DepthGadget, FALSE,
							MUIA_Window_DragBar, FALSE,
							MUIA_Window_SizeGadget, FALSE,
							TAG_DONE);
				}
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
    case MA_GtkWindow_Decorated         : rc = data->is_decorated; break;
    case MA_GtkWindow_DestroyWithParent : rc = data->destroywithparent; break;
    case MA_GtkWindow_Parent    : rc = (ULONG)data->parentwindow; break;
    case MA_GtkWindow_Icon      : rc = (ULONG)data->icon; break;
    case MA_GtkWindow_Modal     : rc = data->is_modal; break;
    case MA_GtkWindow_Role      : rc = (ULONG)data->role; break;
    case MA_GtkWindow_TypeHint  : rc = data->typehint; break;

    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * mAddChild
 *******************************************/

STATIC ULONG mAddChild(struct Data *data, APTR obj, struct MUIP_GtkWindow_AddChild *msg, struct IClass *cl)
{
  struct ChildWindow *child = mgtk_allocmem(sizeof(*child), MEMF_ANY);

  if (child)
  {
    struct Data *cdata;

    child->window = msg->ChildWindow;
    ADDTAIL(&data->childlist, child);

    cdata = (struct Data *)INST_DATA(cl, msg->ChildWindow);
    cdata->parentwindow = obj;
  }

  return 0;
}

/*******************************************
 * mRemChild
 *******************************************/

STATIC ULONG mRemChild(struct Data *data, APTR obj, struct MUIP_GtkWindow_RemChild *msg, struct IClass *cl)
{
  struct ChildWindow *child;
  APTR remobj;

  remobj = msg->ChildWindow;

  ForeachNode(&data->childlist, child)
  {
    if (child->window == remobj)
    {
      struct Data *cdata = (struct Data *)INST_DATA(cl, msg->ChildWindow);
      cdata->parentwindow = NULL;

      REMOVE(child);
      mgtk_freemem(child, sizeof(*child));
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


  switch (msg->MethodID) {

    case OM_DISPOSE     :        mDispose   (data, obj, cl); break;
    case OM_NEW         : return mNew       (cl, obj, (APTR)msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
    case MM_GtkWindow_AddChild    : return mAddChild(data, obj, (APTR)msg, cl);
    case MM_GtkWindow_RemChild    : return mRemChild(data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_window_class(void)
{
  CL_Window = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Window ? 1 : 0;
}

void mgtk_delete_window_class(void) {

  if (CL_Window) {
    MUI_DeleteCustomClass(CL_Window);
    CL_Window=NULL;
  }
}
