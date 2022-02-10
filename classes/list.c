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
 * $Id: list.c,v 1.6 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data
{
  Object *box;
  Object *list;
  Object *listview;
  char *label;
  struct MUI_EventHandlerNode HandlerNode;
};

/*******************************************
 * MUIHook_list
 *
 * emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_list, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;
  GtkWidget *iw;
  GtkWidget *label;
  GtkWidget *signal_w=NULL;
#if 0
  guint pressed;
  GList   *dlist;
#endif
  int i;
  char *selstr;
  char *str;
  int changed;
  GList *iwchilds;

  DebOut("MUIHook_list called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%lx\n",(int) widget);

  DoMethod(obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG) &selstr);

  if(!selstr) {
    DebOut("  nothing selected\n");
    changed=(int) GTK_LIST (widget)->selection;
    g_list_free(GTK_LIST (widget)->selection);
    GTK_LIST (widget)->selection=NULL;
    if(changed) {
      g_signal_emit_by_name(widget,"selection-changed");
      DebOut("classes/list.c: selection-changed signal emitted (1)\n");
    }
    return 0;
  }
  else {
    DebOut("  selected: %s\n",selstr);
  }
#if 0
  for (id=MUIV_List_NextSelected_Start; id!=MUIV_List_NextSelected_End;DoMethod(list,MUIM_List_NextSelected,&id)) {
    DoMethod(list,MUIM_List_GetEntry,id,&selstr);
    DebOut("selected2: %s\n",selstr);
  }
#endif

  /* we have to add the selected list_items to GTK_LIST (widget)->selection, 
   * which is not trivial ;)
   * use the widget->children list and strcmp to do it! 
   */
  for(i=0;i<g_list_length(GTK_LIST (widget)->children);i++) {
    iw=(GtkWidget *) g_list_nth_data(GTK_LIST (widget)->children,i);
    iwchilds=gtk_container_get_children(GTK_CONTAINER(iw));
    label=g_list_nth_data(iwchilds,0);
    gtk_label_get(GTK_LABEL(label),&str);

    if(!strcmp(str,selstr)) {
      DebOut("  found list item nr: %d (%lx)\n",i,iw);
      signal_w=iw;
      break;
    }
  }

  if(!signal_w) {
    ErrOut("selection-changed on unknown widget?\n");
  }
  else {
    if(g_list_find(GTK_LIST (widget)->selection,signal_w)) {
      DebOut("  deselect!\n");
      /* this item was already selected, we will deselect it..? */
      nnset(obj, MUIA_List_Active, MUIV_List_Active_Off);
      g_list_free(GTK_LIST (widget)->selection);
      GTK_LIST (widget)->selection=NULL;

    }
    else {

      DebOut("  new selection!\n");

      g_list_free(GTK_LIST (widget)->selection);
      GTK_LIST (widget)->selection=NULL;

      GTK_LIST (widget)->selection=g_list_append(GTK_LIST (widget)->selection, signal_w);
    }

    g_signal_emit_by_name(widget,"selection-changed");
    DebOut("classes/list.c: selection-changed signal emitted <==\n");
#warning TODO (REALLY!): enable button_release_event again!
//    g_signal_emit_by_name(widget,"button_release_event");
//    g_signal_emit(widget,g_signal_lookup("button_release_event",GTK_TYPE_WIDGET),GTK_TYPE_WIDGET);
//    DebOut("classes/list.c: button_release_event signal emitted <==\n");
  }


#if 0
  pressed=xget(obj,MUIA_Selected);

  if(pressed) {
    DebOut(" classes/list.c: emit newstyle pressed signal \n");
    g_signal_emit(widget,g_signal_lookup("pressed",GTK_TYPE_BUTTON),GTK_TYPE_BUTTON);
    DebOut(" done.\n");
  }
  else { /* released */
    DebOut("  emit newstyle released signal \n");
    g_signal_emit(widget,g_signal_lookup("released",GTK_TYPE_BUTTON),GTK_TYPE_BUTTON);
    DebOut("  emit newstyle clicked signal\n");
    g_signal_emit(widget,g_signal_lookup("clicked",GTK_TYPE_BUTTON),GTK_TYPE_BUTTON);
  }
#endif

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_list, MUIHook_list);

/*******************
 * In GTK you select with the left mouse and you deselect with the
 * right mouse. Hmm. We select with left and deselect with doubleclicks..
 * This is not yet perfect..
 */
HOOKPROTO(MUIHook_list_double, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer) {

  MGTK_USERFUNC_INIT

  DebOut("MUIHook_list_double called\n");

  set(obj, MUIA_List_Active, MUIV_List_Active_Off);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_list_double, MUIHook_list_double);


/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  const char *label;
  struct TagItem *tstate, *tag;

  DebOut("mNew (list)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("list: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  label=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
#if 0
      case MA_List_Label:
        label = (const char *) tag->ti_Data;
        break;
#endif
    }
  }

  if(!widget) {
    ErrOut("classes/list.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%lx\n",widget);
    {
  GETDATA;

  data->list=ListObject,
                MUIA_List_AdjustWidth, TRUE,
              End;

  data->listview=ListviewObject, 
                    MUIA_Listview_List,data->list,
                    MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_None,
                    MUIA_Listview_ScrollerPos,MUIV_Listview_ScrollerPos_None,
                  End;


  /* setup internal hooks */
  /* selection hook */
  DoMethod(data->listview,MUIM_Notify,MUIA_Listview_SelectChange,MUIV_EveryTime, (ULONG) data->list,2,MUIM_CallHook,(ULONG) &MyMuiHook_list);
  DoMethod(data->listview,MUIM_Notify,MUIA_Listview_DoubleClick,MUIV_EveryTime, (ULONG) data->list,2,MUIM_CallHook,(ULONG) &MyMuiHook_list_double);


  set(data->list,MA_Widget,widget);
  set(data->listview,MA_Widget,widget);

  DoMethod(obj,OM_ADDMEMBER,(ULONG) data->listview);
          }
  return (ULONG)obj;
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this radio
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

#if 0
static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("list.c: mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->list) {
    /* now someone added a new object to us, so don't add it to the
     * group object, but to the list!*/
    DebOut("  data->list,OM_ADDMEMBER,msg->opam_Object\n");
    DoMethod(data->list,OM_ADDMEMBER,msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->list, we want to add the list to the object,
     * so just pass it to the original method */
    DebOut("  DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}
#endif

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID)
  {
    case OM_NEW          : return mNew        (cl, obj, msg);
    case OM_GET          : return mGet        (data, obj, (APTR)msg, cl);
#if 0
    case OM_ADDMEMBER    : return mAddMember  (data, obj, (APTR)msg, cl);
#endif
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_list_class(void)
{
  DebOut("mgtk_create_list_class()\n");

  //CL_List = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  CL_List = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_List=%lx\n",CL_List);

  return CL_List ? 1 : 0;
}

void mgtk_delete_list_class(void)
{
  if (CL_List)
  {
    MUI_DeleteCustomClass(CL_List);
    CL_List=NULL;
  }
}
