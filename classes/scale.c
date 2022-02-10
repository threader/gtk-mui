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
 * $Id: scale.c,v 1.6 2009/05/17 18:37:48 stefankl Exp $
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
  ULONG alien_step_increment;
  char *string;
};

static void stringify_update_scroll_text(GtkWidget *widget,Object *obj,struct Data *data) {
  LONG muival;
  gdouble value;

/*
 * DebOut("stringify_update_scroll_text(%lx)\n",widget);
 */

  muival=xget(obj,MUIA_Numeric_Value);

  if(!GTK_RANGE(widget)->adjustment) {
    ErrOut("classes/scale.c: range widget %lx has no adjustment!\n",widget);
    return;
  }

  /* convert MUI's integer value to GTK float value */
  value = muival * GTK_RANGE(widget)->adjustment->step_increment;
  
  if(data->string) {
    g_free(data->string);
  }

  if(GTK_SCALE(widget)->draw_value && (GTK_SCALE(widget)->digits || GTK_RANGE(widget)->adjustment->step_increment) ) {
    /* we have to draw the correct value */
    data->string=_gtk_scale_format_value(GTK_SCALE(widget),value);
  }
  else {
#ifndef MGTK_DEBUG
    data->string=g_strdup(" ");
#else
    data->string=g_strdup("E");
#endif
  }
}

/*******************************************
 * MUIHook_scroll
 *
 * emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_scroll, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;
  ULONG muival;
  struct Data *data;
  gdouble value;

  widget=mgtk_get_widget_from_obj(obj);

  data=(struct Data *) xget(GtkObj(widget),MA_Data);
  muival=xget(obj,MUIA_Numeric_Value);

  if(!GTK_RANGE(widget)->adjustment) {
    ErrOut("classes/scale.c: range widget %lx has no adjustment!\n",widget);
    return 0;
  }
  
  /* convert MUI's integer value to GTK float value */
  value = muival * GTK_RANGE(widget)->adjustment->step_increment; 

  /* update GTK structure */
  GTK_RANGE(widget)->adjustment->value=value;

  DebOut("emit value_changed for scale %lx and adj %lx\n",widget,GTK_RANGE(widget)->adjustment);
  g_signal_emit_by_name(widget,"value_changed");
  g_signal_emit_by_name(GTK_RANGE(widget)->adjustment,"value_changed");

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_scroll, MUIHook_scroll);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  DebOut("mNew (scale: cl %lx,obj %lx,msg %lx)\n",cl,obj,msg);

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("scale: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);
       {
  GETDATA;

  data->string=g_strdup(" ");

  /* setup internal hooks */
  DoMethod(obj,MUIM_Notify,MUIA_Numeric_Value,MUIV_EveryTime, (ULONG) obj,2,MUIM_CallHook,(ULONG) &MyMuiHook_scroll);
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
    case MA_Data : 
      DebOut("mGet: data=%x\n",(int) data);
      rc = (ULONG) data;
      break;

    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * mSet
 * 
 *******************************************/
#if 0
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
  int redraw=0;

	DebOut("scale mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
        /*
      case MA_Scale_Redraw:
        if((int) tag->ti_Data) {
          redraw=1;
        }
        break;
      case MUIA_Slider_Horiz:
        horiz= (int) tag->ti_Data;
        break;
      case MUIA_Numeric_Min:
        set(data->scroll,MUIA_Numeric_Min,(ULONG) tag->ti_Data);
        redraw=1;
        break;
      case MUIA_Numeric_Max:
        set(data->scroll,MUIA_Numeric_Max,(ULONG) tag->ti_Data);
        redraw=1;
        break;
      case MUIA_Numeric_Value:
        level= (int) tag->ti_Data;
        found++;
        break;
        */
    }
  }

}
#endif

static ULONG mRedraw(struct Data *data, APTR obj, struct MUIP_ScaleDraw *msg) {
  ULONG i;

  DebOut("scale mRedraw(..)\n");

  if(msg->redraw) {
    stringify_update_scroll_text((GtkWidget *)xget(obj,MA_Widget),obj,data);
    /* this should not be required..*/
    i=xget(obj,MUIA_Numeric_Value);
    set(obj,MUIA_Numeric_Value,i+1);
    set(obj,MUIA_Numeric_Value,i);
    /* this should be enough */
    DoMethod(obj,MUIM_Draw,MADF_DRAWOBJECT);
  }
  return 0;
}


static ULONG mStringify(struct Data *data, APTR obj, struct MUIP_Numeric_Stringify *msg) {
  GtkWidget *w;

/*
 * DebOut("classes/scale: mStringify\n");
 */

  w=(GtkWidget *)xget(obj,MA_Widget);

  if(w) {
    stringify_update_scroll_text(w,obj,data);
  }
  else {
    WarnOut("classes/scale.c:mStringify: w==NULL\n");
  }

  if(!data->string) {
    data->string=g_strdup(" ");
  }

  return (ULONG) data->string;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  //DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID)
  {
    case OM_NEW                 : return mNew        (cl, obj, msg);
//    case OM_SET                 :        mSet        (data, obj, (APTR)msg); break;
    case OM_GET                 : return mGet        (data, obj, (APTR)msg, cl);
    case MM_Scale_Redraw        : return mRedraw     (data, obj, (APTR)msg);
    case MUIM_Numeric_Stringify : return mStringify  (data, obj, (APTR)msg);
    default                     : return DoSuperMethodA(cl, obj, (Msg)msg);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_scale_class(void) {

  DebOut("mgtk_create_scale_class()\n");

  CL_Scale=MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, (ULONG) sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Scale=%lx\n",CL_Scale);

  return CL_Scale ? 1 : 0;
}

void mgtk_delete_scale_class(void) {

  if (CL_Scale) {
    MUI_DeleteCustomClass(CL_Scale);
    CL_Scale=NULL;
  }
}
