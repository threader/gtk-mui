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
 * $Id: entry.c,v 1.8 2011/07/04 12:16:21 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

struct Data
{
  GtkEntry *entry;
};

/*******************************************
 * MUIHook_entry
 *******************************************/
HOOKPROTO(MUIHook_entry, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_entry(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  g_signal_emit_by_name(widget,"activate",0);
  g_signal_emit_by_name(widget,"changed",0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_entry, MUIHook_entry);

#if 0
/*******************************************
 * MUIHook_entry2
 *******************************************/
HOOKPROTO(MUIHook_entry2, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_entry2(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  g_signal_emit_by_name(widget,"changed",0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_entry2, MUIHook_entry2);
#endif

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew (entry)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("entry: unable to create object!");
    return (ULONG) NULL;
  }
  {
  GETDATA;

  set(obj,MUIA_Frame, MUIV_Frame_String);
  set(obj,MUIA_CycleChain, 1);
  set(obj,MUIA_Weight, 110);

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {
    switch (tag->ti_Tag) {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/entry.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%lx\n",widget);

  data->entry=GTK_ENTRY(widget);

  /* setup internal hooks */
  DoMethod(obj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG) obj,2,MUIM_CallHook,(ULONG) &MyMuiHook_entry);
//  DoMethod(obj,MUIM_Notify,,MUIV_EveryTime, obj,2,MUIM_CallHook,&MyMuiHook_entry2);
        }
  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
  GtkWidget *widget;

  DebOut("mSet(%lx,entry %lx,%lx)\n",data,obj,msg);
  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate))) {

    switch (tag->ti_Tag) {
      case MUIA_String_Acknowledge:
        DebOut("  content: (%lx) >%s<\n",tag->ti_Data,tag->ti_Data);
#warning TODO TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO
        break;
    }
  }
}

#if 0
/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID) {
//    case MUIA_String_Contents: 
//      DebOut("h1..\n");
//      return DoSuperMethodA(cl, obj, (Msg)msg);
//      break;
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}
#endif

/*
 * not used at the moment
 */

#if 0
ULONG	AskMinMax	(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg) {
  struct InstData *data = (struct InstData *)INST_DATA(cl, obj);
  WORD Height;

	DoSuperMethodA(cl, obj, (Msg)msg);

/*
	struct TextFont *font = data->Font ? data->Font : muiAreaData(obj)->mad_Font;
	Height = font->tf_YSize;

	msg->MinMaxInfo->MinHeight += Height;
	msg->MinMaxInfo->DefHeight += Height;
	msg->MinMaxInfo->MaxHeight += Height;

	if(data->Width) {
		ULONG width = data->Width * MyTextLength(font, "n", 1);

		msg->MinMaxInfo->MinWidth  += width;
		msg->MinMaxInfo->DefWidth  += width;
		msg->MinMaxInfo->MaxWidth  += width;
	}
	else {
		msg->MinMaxInfo->MinWidth  += 10;
		msg->MinMaxInfo->DefWidth  += 100;
		msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX;
	}
  */
	return(0);
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
    case OM_NEW         : return mNew       (cl, obj, msg);
    case MUIM_AskMinMax : return mgtk_askminmax (cl,   obj, (APTR)msg, AskMin_Unlimited, AskMin_Limited);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
/*    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);*/
/*    case MUIM_Draw     : return(mDraw     (cl,obj,(APTR)msg));*/
/*    case MUIM_AskMinMax : return AskMinMax  (cl, obj, (struct MUIP_AskMinMax *)msg);*/
  }

  if(data->entry) {
    DebOut("  content: %lx: >%s<\n",data->entry->text,data->entry->text);
    DebOut("  content length: %d\n",data->entry->text_length);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_entry_class(void)
{
  DebOut("mgtk_create_entry_class()\n");

  CL_Entry = MUI_CreateCustomClass(NULL, MUIC_String, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Entry=%lx\n",CL_Entry);

  return CL_Entry ? 1 : 0;
}

void mgtk_delete_entry_class(void)
{
  if (CL_Entry)
  {
    MUI_DeleteCustomClass(CL_Entry);
    CL_Entry=NULL;
  }
}
