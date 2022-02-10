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
 * $Id: combo.c,v 1.10 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

 /* TODO: on dispose, g_free(muistrings) */
 /* TODO: gtk signals */

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data {

  Object *box;
  Object *cycle;
  char **muistrings;
};

/*******************************************
 * MUIHook_list
 *
 * emit the right signal(s)
 *
 * This is one hell of casts, sorry.
 * please care for the differences of
 * GList and GtkList!
 *******************************************/
HOOKPROTO(MUIHook_cycle, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer) {
  MGTK_USERFUNC_INIT

  GtkWidget *widget;
  LONG selected;
  GList *children;

  DebOut("MUIHook_cycle called\n");

  /* get widget */
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) widget);

  children =GTK_LIST(GTK_COMBO (widget)->list)->children;
  DebOut("  #children:  %d\n",g_list_length(children));

  /* get selection nr */
  selected=xget(obj,MUIA_Cycle_Active);
  DebOut("  selected=%d\n",selected);

  /* clear old selection */
  g_list_free(GTK_LIST(GTK_COMBO (widget)->list)->selection);

  /* create new list with just one selection */
  GTK_LIST(GTK_COMBO (widget)->list)->selection =
    g_list_append(NULL, g_list_nth_data(children,selected));

  /* this does *not* put the selected item in list->selection,
   * as I alway thought. It just sets the state of the item
   * to SELECTED and emits the right signals
   */
  gtk_list_select_item(GTK_LIST(GTK_COMBO (widget)->list),selected);

#if MGTK_DEBUG
  /* self checks */
  {
  GList   *choice = GTK_LIST(GTK_COMBO (widget)->list)->selection;
  if (choice) {
    DebOut("list->selection->data: %s\n",choice->data);
    if(selected != gtk_list_child_position (GTK_LIST(GTK_COMBO (widget)->list),
                                            choice->data)) {
      ErrOut("selected %d != gtk_list_child_position %d\n",
	  selected, 
	  gtk_list_child_position (GTK_LIST(GTK_COMBO (widget)->list), choice->data));
    }
    else {
      DebOut("child position is ok: %d\n",gtk_list_child_position (GTK_LIST(GTK_COMBO (widget)->list), choice->data));
    }
  }
  else {
    ErrOut("no child position !?\n");
  }
}
#endif

  g_signal_emit_by_name(GTK_COMBO(widget)->popwin,"hide");

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_cycle, MUIHook_cycle);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

#if 0
  struct TagItem *tstate, *tag;
#endif

  DebOut("mNew (combo)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("combo: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);
   {
  GETDATA;

#if 0
  tstate=((struct opSet *)msg)->ops_AttrList;
  data->widget=NULL;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {
    switch (tag->ti_Tag) {
      case MA_Widget:
        data->widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!data->widget) {
    ErrOut("classes/combo.c: mNew: MA_Widget not supplied!\n");
    CoerceMethod(cl, obj, OM_DISPOSE);
    return (ULONG) NULL;
  }

  DebOut("  widget=%d\n",data->widget);
#endif

  data->box=HGroup,End;

  /* init with one empty string */
  data->muistrings=g_new0(char *,1+1);
#ifdef MGTK_DEBUG
  data->muistrings[0]=g_strdup((gchar *) "dummy cycle entry");
#else
  data->muistrings[0]=g_strdup((gchar *) " ");
#endif
  data->muistrings[1]=NULL;

  data->cycle=CycleObject,
                MUIA_Cycle_Entries, data->muistrings,
              End;

  data->muistrings=NULL;

  DoMethod(obj,OM_ADDMEMBER,(ULONG) data->box);
  DoMethod(data->box,OM_ADDMEMBER,(ULONG) data->cycle);

#if 0
  /* setup internal hooks */
  /* selection hook */
  DoMethod(data->list,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime, (ULONG) data->list,2,MUIM_CallHook,(ULONG) &MyMuiHook_list);
#endif
  DoMethod(data->cycle,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime, (ULONG) data->cycle,2,MUIM_CallHook,(ULONG) &MyMuiHook_cycle);
       }
  return (ULONG)obj;
}


/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this radio
 *******************************************/
#if 0
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("classes/combo.c: mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}
#endif

/*set(GtkObj(combo,MA_Combo_Itemlist,(ULONG) strings));
*/

/*******************************************
 * mSet
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
#if 0
  GList     *strings;
  char **muistrings;
  gint nr;
  gint i;
#endif

  DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Cycle_Active:
	DebOut("  nr=%d\n",tag->ti_Data);
#if 0
      case MA_Combo_Itemlist:
        /* build new MUIA_Cycle_Entries list */
        strings=(GList *) tag->ti_Data;
        nr=g_list_length(strings);
        muistrings=g_new0(char *,nr+1);
        i=0;
        DebOut("while..\n");
        while(strings) {
          DebOut("  add: %s\n",(gchar *) strings->data);
          muistrings[i]=(gchar *) strings->data;
          i++;
          strings = strings->next;
        }
        DebOut("..elihw\n");
        muistrings[i]=NULL;

        DoMethod(data->box, MUIM_Group_InitChange);

        /* remove and dispose old object */
        DoMethod(data->box,OM_REMMEMBER,(ULONG) data->cycle);
        MUI_DisposeObject(data->cycle);

        /* create and add new object */
        data->cycle=CycleObject,
                  MUIA_Cycle_Entries, muistrings,
              End;
        DoMethod(obj,OM_ADDMEMBER,(ULONG) data->cycle);

        DoMethod(data->box, MUIM_Group_ExitChange);
        break;
#endif
    }
  }
}

/* free old strings */
static void free_strings(struct Data *data) {
  int i;

  if(data->muistrings) {
    i=0;
    while(data->muistrings[i]) {
      g_free(data->muistrings[i]);
      data->muistrings[i]=NULL;
      i++;
    }
    g_free(data->muistrings);
  }
}

static ULONG mNewList(struct Data *data, APTR obj, struct MUIP_Itemlist_new *msg) {
  GList     *strings;
  gint nr;
  gint i;

  DebOut("mNewList(.., %lx,..)\n");

  DoMethod(data->box, MUIM_Group_InitChange);

  /* remove and dispose old object */
  DoMethod(data->box,OM_REMMEMBER,(ULONG) data->cycle);
  MUI_DisposeObject(data->cycle);
  data->cycle=NULL;

  free_strings(data);

  /* build new MUIA_Cycle_Entries list */
  strings=msg->list;

  nr=g_list_length(strings);
  data->muistrings=g_new0(char *,nr+1);
  i=0;
  while(strings) {
    DebOut("  combo text: %s\n",(gchar *) strings->data);
    /* is it enough to copy the pointers or do we have to copy
     * the strings to own buffers, too ..?
     * It is *not* enough, we need to make our own copy.
     */
    data->muistrings[i]=g_strdup((gchar *) strings->data);
    i++;
    strings = strings->next;
  }
  data->muistrings[i]=NULL;

  /* create and add new object */
  data->cycle=CycleObject,
            MUIA_Cycle_Entries, data->muistrings,
        End;

  /* add hook again */
  DoMethod(data->cycle,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime, (ULONG) data->cycle,2,MUIM_CallHook,(ULONG) &MyMuiHook_cycle);

  if(data->cycle) {
    DoMethod(data->box,OM_ADDMEMBER,(ULONG) data->cycle);
  }
  else {
    ErrOut("classes/combo.c: unable to create CycleObject\n");
  }

  DoMethod(data->box, MUIM_Group_ExitChange);

  return 0;
}

STATIC VOID mDispose(struct Data *data, APTR obj, struct IClass *cl) {

  DebOut("entered\n");

  free_strings(data);
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

//DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID)
  {
    case OM_NEW               : return mNew        (cl, obj, msg);
    case MM_Combo_Itemlist_new: return mNewList    (data, obj, (APTR)msg);
    case OM_DISPOSE           :        mDispose    (data, obj, cl); break;
    case OM_SET               :        mSet        (data, obj, (APTR)msg); break;
#if 0
    case OM_GET               : return mGet        (data, obj, (APTR)msg, cl);
#endif
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_combo_class(void) {

  DebOut("mgtk_create_combo_class()\n");

  //CL_Combo=MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  CL_Combo=MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Combo=%lx\n",CL_Combo);

  return CL_Combo ? 1 : 0;
}

void mgtk_delete_combo_class(void) {

  DebOut("mgtk_delete_combo_class(): CL_Combo=%lx\n",CL_Combo);
  if (CL_Combo) {
    MUI_DeleteCustomClass(CL_Combo);
    CL_Combo=NULL;
  }
  DebOut("mgtk_delete_combo_class() done\n");
}
