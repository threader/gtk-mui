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
 * $Id: frame.c,v 1.7 2009/05/17 18:37:48 stefankl Exp $
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
  Object *frame;     /* frame object */
  GSList *childlist; /* list of frame children */
  char *label;
  int init;          /* if 1, we are in init mode, children are 
                      * added to the group object and not the frame object */
};

void mgtk_frame_new(struct Data *data,char *label) {

  if(label) {
    data->label=g_strdup(label);
    data->frame=  HGroup,
                    MUIA_Frame, MUIV_Frame_Group, 
                    MUIA_FrameTitle, data->label, 
                    MUIA_Background, MUII_GroupBack,
                  End;
  }
  else {
    data->label=NULL;
    data->frame=  HGroup,
                    MUIA_Frame, MUIV_Frame_Group, 
                    MUIA_Background, MUII_GroupBack,
                  End;
  }
}

/*******************************************
 * mNew
 * 
 * should have MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  char *label;
  struct TagItem *tstate, *tag;

  DebOut("mNew (frame)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("frame: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  label=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {
    switch (tag->ti_Tag) {
      case MA_GtkFrame_Label:
        label = (char *) tag->ti_Data;
        break;
    }
  }
    {
  GETDATA;

  data->init=1;
  mgtk_frame_new(data,label);

  data->childlist=NULL;


//  DoMethod(obj,OM_ADDMEMBER,(LONG) HVSpace);
  DoMethod(obj,OM_ADDMEMBER,(LONG) data->frame);

#if 0
  set(obj,MUIA_MaxWidth,MUI_MAXMAX);
  set(data->frame,MUIA_MaxWidth,MUI_MAXMAX);
  set(obj,MUIA_Weight,100);
  set(data->frame,MUIA_Weight,100);
#endif

  DebOut("  group=%lx\n",obj);
  DebOut("  frame=%lx\n",data->frame);

  data->init=0; }
  return (ULONG)obj;
}

/*******************************************
 * mGet
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_GtkFrame_Label : 
      rc = (ULONG) data->label;
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

void mgtk_frame_set_label(Object *obj, struct Data *data, char *label) {
  int i;

  DebOut("mgtk_frame_set_label(%lx,%lx,%s)\n",obj,data,label);

  if((label && data->label && !strcmp(data->label,label)) || (!data->label && !data)) {
    DebOut(" labels are identical (%s,%s)\n",data->label,label);
    return;
  }

  /* discard everything */
  DoMethod(obj,OM_REMMEMBER,(ULONG) data->frame);
  for(i=0;i<g_slist_length(data->childlist);i++) {
    DebOut("  rem: g_slist_nth_data(%d): %lx\n",i,g_slist_nth_data(data->childlist,i));
    DoMethod(data->frame,OM_REMMEMBER,(ULONG) g_slist_nth_data(data->childlist,i));
  }
  MUI_DisposeObject(data->frame);

  data->init=1;

  if(data->label) {
    g_free(data->label);
  }
  DebOut("  frame group is now empty\n");

  mgtk_frame_new(data,label);

  /* and rebuild it again */
  for(i=0;i<g_slist_length (data->childlist);i++) {
    DebOut("  add: g_slist_nth_data(%d): %lx\n",i,g_slist_nth_data(data->childlist,i));
    DoMethod(data->frame,OM_ADDMEMBER,(ULONG) g_slist_nth_data(data->childlist,i));
  }
  DoMethod(obj,OM_ADDMEMBER,(ULONG) data->frame);

  data->init=0;
}

/*******************************************
 * mSet
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

	DebOut("frame mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_GtkFrame_Label:
        relayout=1;
        mgtk_frame_set_label(obj,data,(char *) tag->ti_Data);
        break;
    }
  }

  if (relayout) {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
  DebOut("left mSet\n");
}

static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("frame.c: mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(!data->init) {
    /* now someone added a new object to us, so don't add it to the
     * group object, but to the frame!*/
    DebOut("  data->frame,OM_ADDMEMBER,msg->opam_Object: %lx\n",msg->opam_Object);
    DoMethod(data->frame,OM_ADDMEMBER,(ULONG) msg->opam_Object);
    data->childlist = g_slist_append (data->childlist, msg->opam_Object);
    return TRUE;
  }
  else {
    /* if data->init, we want to add the frame to the group,
     * so just pass it to the original method */
    DebOut("  DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}

STATIC VOID mDispose(struct Data *data) {

  DebOut("frame.c: mDispose\n");

  g_slist_free(data->childlist);
  data->childlist=NULL;

  if(data->label) {
    g_free(data->label);
    data->label=NULL;
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
    case OM_GET          : return mGet           (data, obj, (APTR)msg, cl);
    case OM_SET          :        mSet           (data, obj, (APTR)msg); break;
    case OM_ADDMEMBER    : return mAddMember     (data, obj, (APTR)msg, cl);
    case OM_DISPOSE      :        mDispose       (data); break;
    case MUIM_AskMinMax  : return mgtk_askminmax (cl, obj, (APTR)msg, AskMin_Unlimited, AskMin_Unlimited);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_frame_class(void) {

  DebOut("mgtk_create_frame_class()\n");

  //CL_Frame = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  CL_Frame = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Frame=%lx\n",CL_Frame);

  return CL_Frame ? 1 : 0;
}

void mgtk_delete_frame_class(void) {

  if (CL_Frame) {
    MUI_DeleteCustomClass(CL_Frame);
    CL_Frame=NULL;
  }
}
