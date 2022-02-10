/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005, 2006 Oliver Brunner
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
 * $Id: notebook.c,v 1.18 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "gtk/gtkmui.h"

#define DUMMYLABEL "MAGIC DUMMY LABEL"

struct Data
{
  Object    *registergroup;
  APTR      *mychild;
  char     **mylabel;
  unsigned int nrchilds;
  unsigned int show_label;
};

static int mgtk_empty_child_label(APTR  **childs,char ***labels);

/************************************************
 * mgtk_note_new
 *
 * create a new registergroup with the supplied
 * childs/labels
 *************************************************/
static Object *mgtk_note_new(APTR *child,char **label,LONG show_label) {
  Object *reg;
  int i;

  DebOut("mgtk_note_new(%lx,%lx)\n",child,label);

  if(show_label) {
    reg=RegisterGroup(label),
            MUIA_Register_Frame, TRUE,
        End;
  }
  else {
    reg=HGroup,
          MUIA_Group_PageMode,TRUE,
        End;
  }
   
  i=0;
  while(label[i]) {
    DebOut("  label[%d]=%s, child[%d]=%lx\n",i,label[i],i,child[i]);
    if(!child[i]) {
      ErrOut("mgtk_note_new\n\n#labels > #childs!?\n");
    }
    else {
      DoMethod(reg,OM_ADDMEMBER,(ULONG) child[i]);
    }
    i++;
  }

  DebOut("  new notebook: %lx\n",reg);

  return reg;
}

void mgtk_debug_list_childs(APTR obj) {
#if defined(MGTK_DEBUG)
  struct List *list;
  struct Node *state;
  Object *o;

  DebOut("---------------V-----------------\n");
  DebOut("mgtk_debug_list_childs(%lx)\n",obj);
  list=(struct List *) xget(obj,MUIA_Group_ChildList);
  state = list->lh_Head;
  while ( (o = NextObject( &state )) ) {
    DebOut(" child: %lx\n",o);
  }
  DebOut("---------------^-----------------\n");
#endif
}

/************************************************
 * mgtk_note_add_child
 *
 * add one child widget to the head/tail of 
 * the given obj
 * 
 * MUI4 has a nice MUIM_Group_MoveMember, to
 * move a new member to the head, MUI3/Zune
 * needs slightly more dangerous methods
 * to do it..
 *
 * There were (still are?) random crashes in
 * mgtk_note_add_child, hope I have fixed them,
 * but who knows.. oli
 *
 * e-uae crashes randomly here. Maybe 1 out of 5 times:
 *
 * classes/notebook.c:mgtk_debug_list_childs: ---------------V-----------------
 * classes/notebook.c:mgtk_debug_list_childs: mgtk_debug_list_childs(ad5eb0c4)
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad3929ec
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad39369c
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad3948bc
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad39573c
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad62058c
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad620afc
 * classes/notebook.c:mgtk_debug_list_childs:  child: ad62106c
 * classes/notebook.c:mgtk_debug_list_childs: ---------------^-----------------
 * classes/notebook.c:mDispatcher: (ad5d6fb4)->msg->MethodID: 80420887
 * classes/notebook.c:mgtk_note_add_child:   data->nrchilds: 7
 * classes/notebook.c:mgtk_note_add_child:    remove nr 0: ad3929ec,Floppy disks
 * classes/notebook.c:mgtk_note_add_child:    remove nr 1: ad39369c,Memory
 * classes/notebook.c:mgtk_note_add_child:    remove nr 2: ad3948bc,CPU
 *
 * -> core:
 *  #0  0xad67c478 in ?? ()
 *  #1  0xab9fffee in DoMethodA (obj=0xad60b3cc, message=0xad29f8b4)
 *      at domethod.c:61
 *  #2  0xab9c6f3a in Group__MUIM_DisconnectParent (cl=0xaba92500,
 *      obj=0xad602ec4, msg=0xad29f8b4) at classes/group.c:647
 *  #3  0xab9cd738 in Group_Dispatcher (cl=0xaba92500, obj=0xad602ec4,
 *      msg=0xad29f8b4) at classes/group.c:2915
 *  #4  0xab9b1e66 in metaDispatcher (cl=0xaba92500, obj=0xad602ec4,
 *      msg=0xad29f8b4) at support_classes.c:258
 *  #5  0xab9b1e66 in metaDispatcher (cl=0xab7b2268, obj=0xad602ec4,
 *      msg=0xad29f8b4) at support_classes.c:258
 *
 * Zune bug?
 ************************************************/

/* remove all childs from reg */
static void mgtk_note__unparent_childs(Object *obj,struct Data *data) {
  int i;

  DebOut("remove children of mui obj %lx\n",obj);

  /* dispose dummy */
  if(!strcmp(DUMMYLABEL,data->mylabel[0])) {
    DebOut("dispose dummy: %s\n",data->mylabel[0]);
    DoMethod(obj,OM_REMMEMBER,(ULONG) data->mychild[0]);
    MUI_DisposeObject(data->mychild[0]);
    data->mychild[0]=(APTR) 0xdead;   /* don't access later by accident*/
    data->mylabel[0][0]='!';          /* don't access later by accident*/
    data->nrchilds=0;
  }

#if !defined(__MORPHOS__)
  /* remove all old  children of the notebook*/
  i=0;
  DebOut("  data->nrchilds: %d\n",data->nrchilds);
  while(i<data->nrchilds) {
    DebOut("   remove nr %d: %lx,%s\n",i,data->mychild[i],data->mylabel[i]);
    DoMethod(obj,OM_REMMEMBER,(ULONG) data->mychild[i]);
    i++;
  }
#endif /* __MORPHOS__ */
  return;
}

static int mgtk_note_add_child(Object *obj,struct Data *data,GtkWidget *newchild, GtkLabel *newlabel,int prepend) {
  APTR    reg;
  APTR   *childs;
  char  **labels;
  int     i;
  int     pos = -1;

  if(newlabel) {
    DebOut("(%lx,%lx,%lx (%s),%d)\n",obj,newchild,newlabel,newlabel->text,prepend);
  }
  else {
    DebOut("(%lx,%lx,%lx (no label widget supplied),%d)\n",obj,newchild,newlabel,prepend);
  }

  //mgtk_debug_list_childs(data->registergroup);
#if 0
  gtk_mui_list_child_tree(GtkObj(obj));
#endif

  reg=data->registergroup;
  DebOut("reg: %lx\n",reg);

  DoMethod(obj,MUIM_Group_InitChange);

  mgtk_note__unparent_childs(reg,data);

  /* Create new child and label tables - leave room for new entry and NULL */
  childs=g_new0(APTR  , (data->nrchilds)+2);
  labels=g_new0(char *, (data->nrchilds)+2);

  for(i=0;i<data->nrchilds;i++) {
    DebOut("copy %d to %d\n",i,i+prepend);
    labels[i+prepend]=data->mylabel[i];
    childs[i+prepend]=data->mychild[i];
  }
  
  pos = prepend ? 0 : data->nrchilds;
  DebOut("pos: %d\n",pos);
  
  if (newlabel && newlabel->text) {
    labels[pos] = g_strdup(newlabel->text);
  }
  else {
    labels[pos] = g_strdup_printf("page %d", pos + 1);
  }
  
  childs[pos] = GtkObj(newchild);
  
  data->nrchilds++;
  DebOut("data->nrchilds now: %d\n",data->nrchilds);
  
  /* terminate list */
  childs[data->nrchilds] = NULL;
  labels[data->nrchilds] = NULL;

  /* free old list */
  g_free(data->mychild);
  g_free(data->mylabel);

  /* use new list */
  data->mychild=childs;
  data->mylabel=labels;
  
  if(data->show_label) {
    set(reg, MUIA_Register_Titles, labels);
  }

#if defined(__MORPHOS__)
  /* life seems to be much easier here, but MUIM_Group_MoveMember seems to
   * be a morphOS/MUI4 only feature. (upto now? ;) )
   */
  DoMethod(reg, OM_ADDMEMBER, GtkObj(newchild));
  DoMethod(reg, MUIM_Group_MoveMember, GtkObj(newchild), prepend ? 0 : -1);
#else
  /* now we have everything,
   * so get rid of the old (now empty) register 
   */

  DoMethod(obj,OM_REMMEMBER,(LONG) reg);
  MUI_DisposeObject(reg);
  reg=NULL;

  /* create and add new reg */
  data->registergroup=mgtk_note_new(childs,labels,data->show_label);
  DoMethod(obj,OM_ADDMEMBER,(LONG) data->registergroup);

#endif /* __MORPHOS__ */

  DoMethod(obj,MUIM_Group_ExitChange);

  //gtk_mui_list_childs(data->registergroup);
#if 0
  gtk_mui_list_child_tree(GtkObj(obj));
#endif

  return pos;
}

/***************************************************/

void mgtk_show_label(Object *obj,struct Data *data,LONG show) {
  int     i;
  LONG active;

  DebOut("mgtk_show_label(%lx,%lx,%d)\n",obj,data,show);

  active=xget(obj,MUIA_Group_ActivePage);

  DoMethod(obj,MUIM_Group_InitChange);

  if(!data->nrchilds) {
    APTR   *childs;
    char  **labels;

    DebOut("  !data->nrchilds\n");
    /* no real childs yet*/
    DoMethod(obj,OM_REMMEMBER,(LONG) data->registergroup);
    MUI_DisposeObject(data->registergroup);

    i=mgtk_empty_child_label(&childs,&labels);
    data->registergroup=mgtk_note_new(childs,labels,show);
    data->nrchilds=i;
    DoMethod(obj,OM_ADDMEMBER,(LONG) data->registergroup);
    return;
  }

  /* remove all old children of the notebook*/
  i=0;
  DebOut("  data->nrchilds: %d\n",data->nrchilds);
  while(i<data->nrchilds) {
    DebOut("   remove nr %d: %lx,%s\n",i,data->mychild[i],data->mylabel[i]);
    DoMethod(data->registergroup,OM_REMMEMBER,(LONG) data->mychild[i]);
    i++;
  }

  /* 
   * get rid of the old (now empty) register 
   */
  DoMethod(obj,OM_REMMEMBER,(LONG) data->registergroup);
  MUI_DisposeObject(data->registergroup);

  /* create and add new reg */
  data->show_label=show;
  data->registergroup=mgtk_note_new(data->mychild,data->mylabel,data->show_label);
  DoMethod(obj,OM_ADDMEMBER,(LONG) data->registergroup);

  DoMethod(obj,MUIM_Group_ExitChange);
  nnset(obj,MUIA_Group_ActivePage,active);
}

/*******************************************
 * MUIHook_notebook
 *******************************************/
HOOKPROTO(MUIHook_notebook, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_notebook(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

/*TODO
  mgtk_notebook_value_update(widget);
  */

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_notebook, MUIHook_notebook);


static int mgtk_empty_child_label(APTR  **in_childs,char ***in_labels) {
  APTR  *childs;
  char **labels;

  /* create dummy child */
  childs=g_new0(APTR,2);
  labels=g_new0(char *,2);

  labels[0]=g_strdup(DUMMYLABEL);
  childs[0]=TextObject,MUIA_Text_Contents,labels[0],End;
  childs[1]=NULL;
  labels[1]=NULL;

  *in_childs=childs;
  *in_labels=labels;

  return 1;
}

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 * we create a empty Notebook, as MUI does
 * not like empty Registers, we fill it
 * with a dummy.
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  GtkWidget *widget;
  APTR   registergroup;
  APTR  *childs;
  char **labels;
  int    i;

  DebOut("mNew (notebook)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("notebook: unable to create object!");
    return (ULONG) NULL;
  }
  DebOut("  new obj=%lx\n",obj);

  widget=(GtkWidget *) xget(obj,MA_Widget);

  if(!widget) {
    ErrOut("classes/notebook.c: mNew: MA_Widget not supplied!\n");
    CoerceMethod(cl, obj, OM_DISPOSE);
    return (ULONG) NULL;
  }
  DebOut("  widget=%lx\n",widget);

  /* setup internal hooks */
  /* TODO   DoMethod(text,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime, text,2,MUIM_CallHook,&MyMuiHook_spinbutton);*/

  i=mgtk_empty_child_label(&childs,&labels);
  registergroup=mgtk_note_new(childs,labels,1);

  if (registergroup) {
    GETDATA;

    DoMethod(obj,OM_ADDMEMBER,(LONG) registergroup);

    data->registergroup=registergroup;
    data->mylabel=labels;
    data->mychild=childs;
    data->nrchilds=i;
    data->show_label=1;
  }
  else {
    ErrOut("unable to create registergroup\n");
    g_free(labels[0]);
    g_free(labels);
    MUI_DisposeObject(childs[0]);
    g_free(childs);
    CoerceMethod(cl, obj, OM_DISPOSE);
    obj=NULL;
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

 

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Group_ActivePage:
        set(data->registergroup,MUIA_Group_ActivePage,tag->ti_Data);
        break;
      case MA_Show_Label:
        mgtk_show_label(obj,data,tag->ti_Data);
        break;
    }

  }
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this spin
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Note_NrChilds: 
      DebOut("mGet: data->nrchilds=%x\n",(int) data->nrchilds);
      rc = (ULONG) data->nrchilds;
      break;
    case MUIA_Group_ActivePage:
      rc=xget(data->registergroup,MUIA_Group_ActivePage);
      DebOut("mGet: MUIA_Group_ActivePage is %d\n",rc);
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * mAppendChild
 *******************************************/
STATIC ULONG mAppendChild(struct Data *data, APTR obj, struct MUIP_NoteBook_AppendChild *msg) {
  return mgtk_note_add_child(obj, data, msg->Widget, msg->Label, 0);
}

/*******************************************
 * mPrependChild
 *******************************************/
STATIC ULONG mPrependChild(struct Data *data, APTR obj, struct MUIP_NoteBook_AppendChild *msg) {
    return mgtk_note_add_child(obj, data, msg->Widget, msg->Label, 1);

}

/*******************************************
 * mPageNr
 *
 * get's the page number of a MUI object
 *
 * if you want to know, on which page a
 * object is, use this!
 *******************************************/
STATIC ULONG mPageNr(struct Data *data, APTR obj, struct MUIP_NoteBook_PageNr *msg) {
  int i=0;

  DebOut("mPageNr(..)\n");

  while(i < data->nrchilds) {
    DebOut("  i=%d data->nrchilds=%d msg->obj=%lx data->mychild[i] GtkObj(widget)=%lx )=%lx\n",i,data->nrchilds, msg->obj, data->mychild[i],GtkObj(msg->widget));
    if(msg->obj == data->mychild[i]) {
      return i;
    }
    i++;
  }

  ErrOut("classes/notebook.c: object %lx is no children of notebook widget %lx\n",msg->obj,msg->widget);
  return 0;
}

/*******************************************
 * mPageNObject
 *
 * get's the object of page nr n
 *******************************************/
STATIC ULONG mPageNObject(struct Data *data, APTR obj, struct MUIP_NoteBook_PageNObject *msg) {

  DebOut("mPageNObject(..)\n");

  return (ULONG) data->mychild[msg->n];
}


/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID) {

    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
    case MM_NoteBook_AppendChild  : return mAppendChild(data, obj, (APTR)msg);
    case MM_NoteBook_PrependChild : return mPrependChild(data, obj, (APTR)msg);
    case MM_NoteBook_PageNr       : return mPageNr(data, obj, (APTR)msg);
    case MM_NoteBook_PageNObject  : return mPageNObject(data, obj, (APTR)msg);
    /* TODO: Dispose */
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_notebook_class(void) {

  DebOut("mgtk_create_notebook_class()\n");

  CL_NoteBook = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_NoteBook=%lx\n",CL_NoteBook);

  return CL_NoteBook ? 1 : 0;
}

void mgtk_delete_notebook_class(void)
{
  if (CL_NoteBook)
  {
    MUI_DeleteCustomClass(CL_NoteBook);
    CL_NoteBook=NULL;
  }
}
