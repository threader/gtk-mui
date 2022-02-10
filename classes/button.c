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
 * $Id: button.c,v 1.14 2012/05/10 08:48:00 o1i Exp $
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
  Object *button;
  char *label;
  struct MUI_EventHandlerNode HandlerNode;
  LONG activate;
};

#if 0
/* not needed anymore? */

/* sorry, no vargs here */
void mgtk_signal_emit(GtkObject *object, guint signal_id, guint type) {

  mgtk_stored_signal *s;
  GETGLOBAL

  DebOut("mgtk_signal_emit(%lx,..)\n",object);

  s=g_new0(mgtk_stored_signal,1);

  s->object=object;
  s->signal_id=signal_id;
  s->type=type;

  mgtk->delayed_signals=g_list_append(mgtk->delayed_signals,(gpointer) s);
}

#endif

/*******************************************
 * MUIHook_button
 *
 * emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_button, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;
  guint pressed;

  DebOut("MUIHook_button called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%lx\n",widget);

  pressed=xget(obj,MUIA_Selected);

  if(pressed) {
    DebOut("  emit newstyle pressed signal \n");
    g_signal_emit_by_name(widget,"pressed");
  }
  else { /* released */
    /*
    mgtk_signal_emit(widget,g_signal_lookup("released",GTK_TYPE_BUTTON),GTK_TYPE_BUTTON);
    mgtk_signal_emit(widget,g_signal_lookup("clicked",GTK_TYPE_BUTTON),GTK_TYPE_BUTTON);
    */
    DebOut("  emit newstyle released signal \n");
    g_signal_emit_by_name(widget,"released");
    DebOut("  emit newstyle clicked signal\n");
    g_signal_emit_by_name(widget,"clicked");
  }
  DebOut("  signals finished\n");

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_button, MUIHook_button);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  Object *button;
  GtkWidget *widget;
  const char *label;
  char *mylabel;
  struct TagItem *tstate, *tag;

  DebOut("mNew (button)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("button: unable to create object!");
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
      case MA_Button_Label:
        label = (const char *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%d\n",widget);

  if(label) {
    mylabel=g_strdup(label);
    button=MUI_MakeObject(MUIO_Button, (ULONG)mylabel);
    SetAttrs(button, MUIA_CycleChain, 1, MUIA_Weight, 0, TAG_DONE);
  }
  else {
    mylabel=NULL;
    button=VGroup,
            GroupSpacing(0),
            MUIA_CycleChain, 1,
            MUIA_Weight, 0,
            MUIA_Background, MUII_ButtonBack,
            ButtonFrame,
            MUIA_InputMode , MUIV_InputMode_RelVerify,
          End;
  }

  /* setup internal hooks */
  DoMethod(button,MUIM_Notify,MUIA_Selected,MUIV_EveryTime, (ULONG) button,2,MUIM_CallHook,(ULONG) &MyMuiHook_button);

  GETDATA;

  data->label=mylabel;

  data->button=NULL;
  DoMethod(obj,OM_ADDMEMBER,(ULONG) button);
  data->button=button;

  return (ULONG)obj;
}

static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->button) {
    /* now someone added a new object to us, so don't add it to the
     * group object, but to the button!*/
    DebOut("  data->button,OM_ADDMEMBER,msg->opam_Object\n");
    DoMethod(data->button,OM_ADDMEMBER,(ULONG) msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->button, we want to add the button to the object,
     * so just pass it to the original method */
    DebOut("  DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}

/*******************************************
 * mSet
 *******************************************/
STATIC VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {

    switch (tag->ti_Tag) {
      /* We can set notify to MA_GtkWidget_Activate now */

      case MA_GtkWidget_Activate:
	data->activate = tag->ti_Data;
      break;

      case MUIA_Pressed:
	if (tag->ti_Data == FALSE) {
	  set(obj, MA_GtkWidget_Activate, TRUE);
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
		case MA_GtkWidget_Activate:
			rc = data->activate;
			break;
		default: 
			return DoSuperMethodA(cl, obj, (Msg)msg);
	}

	*msg->opg_Storage = rc;
	return TRUE;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID)
  {
    case OM_NEW          : return mNew        (cl, obj, msg);
    case OM_SET          :        mSet        (data, obj, (APTR)msg); break;
    case OM_GET          : return mGet        (data, obj, (APTR)msg, cl);
    case OM_ADDMEMBER    : return mAddMember  (data, obj, (APTR)msg, cl);
//    case MUIM_AskMinMax  : return mgtk_askminmax (cl, obj, (APTR)msg, 0, 0);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_button_class(void)
{
  DebOut("mgtk_create_button_class()\n");

  //CL_Button = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  CL_Button = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Button=%lx\n",CL_Button);

  return CL_Button ? 1 : 0;
}

void mgtk_delete_button_class(void)
{
  if (CL_Button)
  {
    MUI_DeleteCustomClass(CL_Button);
    CL_Button=NULL;
  }
}
