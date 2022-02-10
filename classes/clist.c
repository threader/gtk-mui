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
 * $Id: clist.c,v 1.7 2012/05/10 08:48:00 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtkclist.h"
#include "gtk_globals.h"

struct Data {
  Object *box;
  Object *list;
  Object *listview;
  char   *format;
  struct  MUI_EventHandlerNode HandlerNode;
  LONG    defwidth, defheight;
};

/****************************************************************
 * MUIHook_clist_display
 *
 * build mui strings for display of a row
 *
 * Titles:
 *    However, if you have
 *    a multi column list with a custom display hook and you
 *    want to have seperate titles for each of your columns,
 *    you can set this attribute to TRUE. In this case, whenever
 *    MUI feels that the list title has to be drawn, it will
 *    call your display hook with a NULL entry pointer. Your
 *    hook has to check for this NULL entry and fill the
 *    given string array with your column titles. 
 ****************************************************************/

AROS_UFH3(void, MUIHook_clist_display,
    AROS_UFHA(struct Hook *, h,       A0),
    AROS_UFHA(char **,       strings, A2),
    AROS_UFHA(APTR,          entry,   A1)) {

  AROS_USERFUNC_INIT
  int i;
  GtkCList *clist;
  GtkCListRow *row;

  DebOut("MUIHook_clist_display(..)\n");

  clist=(GtkCList *) h->h_Data;
  DebOut("  widget=      %lx\n",clist);
  DebOut("  entry =      %lx\n",entry);

  if(entry) { /* normal row */
    row=(GtkCListRow *) g_list_nth_data(clist->row_list,(guint) strings[-1]);

    DebOut("  row_list=    %lx\n",clist->row_list);
    DebOut("  row number=  %ld\n",(guint) strings[-1]);
    DebOut("  GtkCListRow= %lx\n",row);
    DebOut("  columns=     %ld\n",clist->columns);

    if(!row) {
      /* still empty */
      DebOut("  => no row: display -empty-\n");
      for(i=0;i < GTK_CLIST(clist)->columns;i++) {
#ifdef MGTK_DEBUG
        strings[i] = "-empty-";
#else
        strings[i] = "   ";
#endif
      }
    }
    else { 
      /* now we have everything to display */
      DebOut("  fill ListView row:\n");
      for(i=0 ; i<GTK_CLIST(clist)->columns ; i++) {
        DebOut("    i: %d\n",i);
        DebOut("    row: %lx\n",row);
        DebOut("    row->cell[%d]: %lx\n",i,row->cell[i]);
        DebOut("    strings[%d]=%s\n",i,GTK_CELL_TEXT (row->cell[i])->text);
        strings[i]=GTK_CELL_TEXT (row->cell[i])->text;
      }
    }
  } 
  else { /*title */
    DebOut("  fill ListView Title:\n");
    for(i=0;i < GTK_CLIST(clist)->columns;i++) {
      DebOut("    strings[%d]=%s\n",i,clist->column[i].title);
      strings[i]=clist->column[i].title;
    }

  }
 
  AROS_USERFUNC_EXIT
}
struct Hook hook_display;

/*******************************************
 * MUIHook_clist
 *
 * emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_clist, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkCList *widget;
  int row;
  int column;

  DebOut("MUIHook_clist called\n");
  widget=(GtkCList *) mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%lx\n",(int) widget);

  row=xget(obj,MUIA_List_Active);
  column=xget(obj,MUIA_NList_ClickColumn);
  DebOut("  row=%d\n",row);
  DebOut("  column=%d\n",column);
  DebOut("  rows=%d\n",GTK_CLIST(widget)->rows);

  if(row > GTK_CLIST(widget)->rows) {
    ErrOut("row > GTK_CLIST(widget)->rows\n");
    return 0;
  }

  /* clear old selection */
  if(widget->selection) {
    g_list_free(widget->selection);
    widget->selection=NULL;
  }

  /* zune seems to return -1, if you click on a list with no entries..*/
  if(row<0) {
    DebOut("  nothing selected\n");
    return 0;
  }

  /* we should have a valid row selection */
  widget->selection=g_list_append(widget->selection,g_list_nth_data(widget->row_list,row));

  /* but it is impossible, to find out the column for the moment..*/
  DebOut("  emit: gtk_signal_emit_by_name(%lx,select_row,row=%d,column=%d,NULL)\n",widget,row, column);
  gtk_signal_emit_by_name (GTK_OBJECT (widget), "select_row", row, column, NULL);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_clist, MUIHook_clist);

/*******************
 * In GTK you select with the left mouse and you deselect with the
 * right mouse. Hmm. We select with left and deselect with doubleclicks..
 * This is not yet perfect..
 */
HOOKPROTO(MUIHook_clist_double, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer) {

  MGTK_USERFUNC_INIT

  DebOut("MUIHook_clist_double called\n");

  set(obj, MUIA_List_Active, MUIV_List_Active_Off);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_clist_double, MUIHook_clist_double);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  struct TagItem *tstate, *tag;
  GString *t;
  ULONG columns;
  ULONG i;

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("clist: unable to create object!");
    return (ULONG) NULL;
  }

  GETDATA;
  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  columns=0;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
      case MA_CList_Columns:
        columns = (ULONG) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/clist.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%lx\n",widget);
  DebOut("  columns=%d\n",columns);

  hook_display.h_Entry = (HOOKFUNC) MUIHook_clist_display;
  hook_display.h_Data  = (APTR) widget;

  /*
   * there maybe are more elegant ways, but this works..
   *
   * we need one ',' between every column
   */

  if(columns > 1) {
    t=g_string_new("BAR,");
    for(i=2;i<columns;i++) {
#if 0
      DebOut("i: %d\n",i);
#endif
      t=g_string_append(t,"BAR,");
#if 0
      DebOut("t=%s\n",t->str);
#endif
    }
    t=g_string_append(t,"BAR");
    data->format=g_string_free(t,FALSE);

    DebOut("  format: >%s<\n",data->format);
    
#if BLA
    data->list=NListObject,
                ReadListFrame,
#if 0
                  MUIA_NList_AdjustHeight, FALSE,
                  MUIA_NList_AdjustWidth, FALSE,
#endif
                  MUIA_NList_Format,      (ULONG) data->format,
                  MUIA_NList_Title,       TRUE,
                  MUIA_NList_DisplayHook, (ULONG) &hook_display,
               End;
#endif
    data->list=MUI_NewObject("NList.mcc", ReadListFrame, 
                                          MUIA_NList_Format,      (ULONG) data->format,
                                          MUIA_NList_Title,       TRUE,
                                          MUIA_NList_DisplayHook, (ULONG) &hook_display);
#if 0
    g_free(c);
#endif
  }
  else {
    data->list=NListObject,
                ReadListFrame,
#if 0
                 MUIA_List_AdjustWidth,  TRUE,
#endif
                 MUIA_NList_DisplayHook,  (ULONG) &hook_display,
               End;
  }

  data->listview=NListviewObject, 
                    MUIA_Listview_List,data->list,
                    MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_None,
#if 0
                    MUIA_Listview_ScrollerPos,MUIV_Listview_ScrollerPos_None,
#endif
                  End;

  if((!data->list) || (!data->listview)) {
    ErrOut("Couldn't find NListview.mcc\n");
    /* should be safe to continue here..*/
  }

  data->defheight    = AskMin_Unlimited; /* use all space */
  data->defwidth     = AskMin_Unlimited;

#if 0
#ifdef MGTK_DEBUG
  DoMethod(data->list,MUIM_List_InsertSingle,"-empty init-",MUIV_List_Insert_Bottom);
#else
  DoMethod(data->list,MUIM_List_InsertSingle," ",MUIV_List_Insert_Bottom);
#endif
#endif

  /* setup internal hooks */
  /* selection hook */
  DoMethod(data->list,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime, (ULONG) data->list,2,MUIM_CallHook,(ULONG) &MyMuiHook_clist);
#if 0
  DoMethod(data->listview,MUIM_Notify,MUIA_Listview_DoubleClick,MUIV_EveryTime, (ULONG) data->list,2,MUIM_CallHook,(ULONG) &MyMuiHook_clist_double);
#endif

  set(data->list,MA_Widget,widget);
  set(data->listview,MA_Widget,widget);

  DoMethod(obj,OM_ADDMEMBER,(ULONG) data->listview);

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

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {

  struct TagItem *tstate, *tag;
  ULONG relayoutlist = 0;

  DebOut("clist->mSet\n");

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem((APTR) &tstate))) {

    switch (tag->ti_Tag) {

      case MUIA_List_Title:
        set(data->list,MUIA_List_Title, tag->ti_Data);
        relayoutlist=1;
        DebOut("  set(%lx,MUIA_List_Title, %d)\n",obj,tag->ti_Data);
        break;
      /* set data->def* */
      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data) {
          relayoutlist = 1;
          data->defwidth = tag->ti_Data;
        }
        break;
      case MA_DefHeight:
        if (data->defheight != tag->ti_Data) {
          relayoutlist = 1;
          data->defheight = tag->ti_Data;
        }
        break;
    }
  }

  if (relayoutlist) {
    DoMethod(data->list, MUIM_Group_InitChange);
    DoMethod(data->list, MUIM_Group_ExitChange);
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);

  }
}

#if 0
IPTR mList_InsertSingle(struct Data *data, Object *obj, struct MUIP_List_InsertSingle *msg,struct IClass *cl) {

  DebOut("classes/clist.c: mList_InsertSingle(%lx,..)\n",obj);


//  return DoSuperMethodA(obj,MUIM_List_InsertSingle, (IPTR)&msg->entry, 1, msg->pos);
  return (IPTR)DoSuperMethodA(cl, obj, msg);
}
#endif

STATIC VOID mDispose(struct Data *data, APTR obj, struct IClass *cl) {

  DebOut("classes/clist.c: mDispose()\n");

  if(data->format) {
    g_free(data->format);
  }
  data->format=NULL;
}


/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID)
  {
    case OM_NEW          : return mNew              (cl,   obj, (APTR)msg);
    case OM_GET          : return mGet              (data, obj, (APTR)msg, cl);
    case OM_SET          :        mSet              (data, obj, (APTR)msg); break;
    case OM_DISPOSE      :        mDispose          (data, obj, cl); break;
#if 0
    case MUIM_AskMinMax  : return mgtk_askminmax    (cl, obj, (APTR)msg, data->defwidth, data->defheight);
    case MUIM_List_InsertSingle: return mList_InsertSingle(data, obj, (APTR)msg, cl);
    case OM_ADDMEMBER    : return mAddMember  (data, obj, (APTR)msg, cl);
#endif
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_clist_class(void)
{
  DebOut("mgtk_create_clist_class()\n");

  CL_CList = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_CList=%lx\n",CL_CList);

  return CL_CList ? 1 : 0;
}

void mgtk_delete_clist_class(void)
{
  if (CL_CList)
  {
    MUI_DeleteCustomClass(CL_CList);
    CL_CList=NULL;
  }
}
