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
 * $Id: group.c,v 1.9 2008/11/03 15:02:59 o1i Exp $
 *
 *****************************************************************************/

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>
#include "gtk/gtk.h"
#include "gtk/gtkaccellabel.h"
#include "gtk/gtkmenuitem.h"

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

struct Data {
  LONG homogeneous;
  LONG spacing;
  LONG is_resizable;
  LONG fill;
  LONG defwidth, defheight;
  LONG horiz;
  LONG has_menu;
  APTR context_menu;
  struct Hook LayoutHook;
};


#if 0
/*************************** LayoutHook *****************************/

/*
 * would be nice, if we could call the size_request, if there
 * is one and the default group Layout Hook, if not.
 * There is sadly no way (at least I did not find one)
 * to get the Layout Hook of a parent class (you can set
 * a hook and you can clear it. You cannot get it :().
 * xget(obj,MUIA_Group_LayoutHook) would be so nice ..
 *
 * The following was a bad hack try, which can't work..
 */

#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm)
{
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif

  GtkWidget *w;
  struct Data *data;

  DebOut("LayoutHook\n");

  /* this one is evil, I know! */
  w=mgtk_get_widget_from_obj(obj);
  if(w && GTK_WIDGET_GET_CLASS(w)->size_request) {
    DebOut("found size_request for widget %lx (%s)\n",w,w);
  }

  data=(struct Data *) xget(obj,MA_Data);

  DebOut(" data: %lx\n",data);

  DebOut("LayoutHook: set to NULL \n");
  set(obj, MUIA_Group_LayoutHook, NULL);

  DebOut("LayoutHook: call again \n");
  //case MUILM_MINMAX: {
    DoMethod(_parent(obj),MUIM_Layout,NULL);
  //}

  //case MUILM_LAYOUT: {
    //DoMethod(obj,MUIM_Layout);
  //}
  set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);

  DebOut("LayoutHook: set to %lx \n",&data->LayoutHook);

#if 0
  switch (lm->lm_Type) {

    case MUILM_MINMAX: {
      /* ======== MUILM_MINMAX ======= */
      DebOut(" vvvvvvvvvvvvvvvvvvv MUILM_MINMAX vvvvvvvvvvvvvvvvvvv\n");

      GtkWidget *table;
      GtkRequisition *requisition;

      table=mgtk_get_widget_from_obj(obj);
      requisition=mgtk_allocmem(sizeof(GtkRequisition),MEMF_CLEAR);

      /* 
       * MUI knows three kinds of requisitions: min, def and max.
       * gtk_table_size_request only one.
       *
       * So in order to work, we need a third parameter here,
       * we extended the requistion structure by one flag:
       *
       * struct _GtkRequisition {
       *   gint width;
       *   gint height;
       *   gint gtkmui_type;   <-- new
       * };
       *
       * 1: minwidth
       * 2: defwidth
       * 3: maxwidth
       */

      requisition->gtkmui_type=1;
      gtk_table_size_request(table,requisition);
      lm->lm_MinMax.MinWidth  = requisition->width;
      lm->lm_MinMax.MinHeight = requisition->height;
      DebOut("  MUILM_MINMAX: min width: %d height: %d\n",requisition->width,requisition->height);

      requisition->gtkmui_type=3;
      gtk_table_size_request(table,requisition);
      lm->lm_MinMax.MaxWidth  = requisition->width;
      lm->lm_MinMax.MaxHeight = requisition->height;
      DebOut("  MUILM_MINMAX: max width: %d height: %d\n",requisition->width,requisition->height);
 
      /* we call type 2 after all others, as those values
       * are "the best fitting" for GTK and the call to gtk_table_size_request
       * sets some values, which are used later on.
       */
      requisition->gtkmui_type=2;
      gtk_table_size_request(table,requisition);
      lm->lm_MinMax.DefWidth  = (LONG) requisition->width;
      lm->lm_MinMax.DefHeight = (LONG) requisition->height;
      DebOut("  MUILM_MINMAX: def width: %d height: %d\n",requisition->width,requisition->height);

      mgtk_freemem(requisition,sizeof(GtkRequisition));
      DebOut(" ^^^^^^^^^^^^^^^^^^^ MUILM_MINMAX ^^^^^^^^^^^^^^^^^^^\n");
      return 0;
    }
    /* ======== MUILM_LAYOUT ======= */

    case MUILM_LAYOUT: {
      GtkWidget *table;
      GtkAllocation *allocation;

      DebOut(" vvvvvvvvvvvvvvvvvvv MUILM_LAYOUT vvvvvvvvvvvvvvvvvvv\n");

      table=mgtk_get_widget_from_obj(obj);
      allocation=mgtk_allocmem(sizeof(GtkAllocation),MEMF_CLEAR);

      allocation->width = (gint) lm->lm_Layout.Width; 
      allocation->height= (gint) lm->lm_Layout.Height;
      allocation->x= 0;
      allocation->y= 0;

      /* up to here, everything seems ok */
      DebOut("MUILM_LAYOUT: x=y=0, width=%d height=%d\n",allocation->width,allocation->height);

      gtk_table_size_allocate(table,allocation);

      mgtk_freemem(allocation,sizeof(GtkAllocation));
 
      DebOut(" ^^^^^^^^^^^^^^^^^^^ MUILM_LAYOUT ^^^^^^^^^^^^^^^^^^^\n");
      return TRUE;
    }
  }
#endif

  /* ? */
  return MUILM_UNKNOWN;
}
#endif

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  DebOut("group mNew: %lx\n",obj);

  if (obj) {
    GETDATA;

    /* default is a vertical group */
    data->horiz        =  0;
    data->homogeneous  = -1;
    data->spacing      = -1;
    data->defheight    = AskMin_Unlimited; /* use all space */
    data->defwidth     = AskMin_Unlimited;
    data->is_resizable = -1;
    data->fill         =  1;
    data->has_menu     =  0;
    data->context_menu =  NULL;

    if(xget(obj,MUIA_Group_Horiz)) {
      data->horiz        =  1;
    }

#if 0
    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
#endif

  }

  return (ULONG)obj;
}

HOOKPROTO(MUIHookFunc_menu2,ULONG,MGTK_HOOK_OBJECT_POINTER obj,MGTK_HOOK_APTR bla) {
  MGTK_USERFUNC_INIT

  GtkWidget *widget;

  ErrOut("MUIHookFunc_menu called for obj %lx (%lx)\n",obj,bla);

  widget=(GtkWidget *) xget(obj,MA_Widget);

  if(!widget) {
    DebOut("ERROR: widget for obj %lx not found!\n",obj);
  }
  else {
    g_signal_emit_by_name(widget,"activate");
  }
//  call_gtk_hooks_activated(obj);
  return(0);
  MGTK_USERFUNC_EXIT
}

MakeHook(MyMuiHook_menu2, MUIHookFunc_menu2);


static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, struct IClass *cl) {

  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

  DebOut("group mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem((APTR) &tstate))) {

    switch (tag->ti_Tag) {

      /* set data->def* */
      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data) {
          relayout = 1;
          data->defwidth = tag->ti_Data;
        }
        break;
      case MA_DefHeight:
        if (data->defheight != tag->ti_Data) {
          relayout = 1;
          data->defheight = tag->ti_Data;
        }
        break;
      /* MA_Group_Fill */
      case MA_Group_Fill:
        if (data->fill != tag->ti_Data) {
          data->fill = tag->ti_Data;
          if(!data->fill) {
            /* this group is not allowed to grow anymore */
            if(data->horiz) {
              data->defwidth  = AskMin_Limited;
              data->defheight = AskMin_Unlimited;
            }
            else {
              data->defwidth  = AskMin_Unlimited;
              data->defheight = AskMin_Limited;
            }
          }
          else {
            /* TODO ! */
            DebOut("TODO: classes/group.c: MA_Group_Fill,TRUE is to be tested!\n");
            data->defwidth  = AskMin_Unlimited;
            data->defheight = AskMin_Unlimited;
          }
          relayout = 1;
        }
        break;

      case MA_Menu_Attached:
        DebOut("  group->has_menu := %d\n",tag->ti_Data);
        data->has_menu=tag->ti_Data;
        break;

      case MUIA_Group_Horiz:
        if(tag->ti_Data) {
          /* set to horiz */
          if(!data->horiz && !(data->defwidth>0) && !(data->defheight>0)) {
            if(!data->fill) {
              data->defwidth = AskMin_Limited;
              data->defheight= AskMin_Unlimited; /*Limited;*/ /* ?? */
            }
            data->horiz    = 1;
            relayout=1;
          }
        }
        else {
          if(data->horiz && !(data->defwidth>0) && !(data->defheight>0)) {
            if(!data->fill) {
              data->defwidth = AskMin_Unlimited; /*Limited;*/
              data->defheight= AskMin_Limited;
            }
            data->horiz    = 0;
            relayout=1;
          }
        }
        /* the break is missing with a reason ;) */
      default:
        DoSuperMethodA(cl, obj, (Msg)msg);
        break;
    }
  }

  if (relayout) {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
}

/*******************************************
 * mGet
 * 
 * MA_Data: instance data
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Data : 
      rc=(ULONG) data;
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * MUIM_ContextMenuBuild
 *
 * create a nice menustrip object and
 * return it
 *******************************************/

static GtkWidget* mgtk_get_accel_parent(GtkWidget *w) {
  Object *o;
  GtkWidget *parent;

   /* seems not to work..
  i=gtk_container_get_children(GTK_CONTAINER(menu_item));
    *
    * try the mui way ;) :
    */

  DebOut("mgtk_get_accel_parent(%lx)\n",w);

  o=(Object *) xget(GtkObj(w),MUIA_Parent);
  
  if(!o) {
    ErrOut("mgtk_get_accel_parent: widget %lx has no parent mui object !?\n",w);
    return NULL;
  }
 
  parent=(GtkWidget *) xget(o,MA_Widget);

  if(!parent) {
    ErrOut("mgtk_get_accel_parent: mui object %lx has no MA_Widget !?\n",parent);
    return NULL;
  }

  DebOut("  parent=%lx\n",parent);
 
  return parent;
}

/*****************************************
 * mgtk_menu_append(mui obj, GtkMenuItem)
 *
 * recursively attach a GtkMenuItem to
 * a mui menu item/menu strip
 *****************************************/
static void mgtk_menu_append(APTR menuitem, GtkWidget *child, unsigned int level) {

  GList        *i=NULL;
  const gchar  *label;
  GtkWidget     *menu_shell;

  DebOut("mgtk_menu_append(%lx,%lx,%d)\n",menuitem,child,level);

  /* create new menuitem */
  label=gtk_label_get_text(GTK_LABEL(mgtk_get_accel(child)));

  DebOut("  label=%s\n",label);

  menu_shell=gtk_menu_item_get_submenu(GTK_MENU_ITEM(child));
  DebOut("  mgtk_menu_append::menu_shell=%lx\n",menu_shell);

  /* care for submenu, if present */
  if(menu_shell) {
    i=GTK_MENU_SHELL(menu_shell)->children;
    DebOut("  menu_shell->children=%lx\n",GTK_MENU_SHELL(menu_shell)->children);
  }


  switch(level) {
    case 1:
      /* we need a MenuObject */
      if(i || menu_shell) { /* there will be children */
	DebOut("CASE 1: %s\n",label);
        GTK_MUI(child)->MuiMenu=MenuObjectT(label), 
                                  MA_Widget, (ULONG) child,
                                End;
      }
      else {
        /* no childs, that sux, we are at the top level in mui, the things you
         * normally see *in* the screen bar, but we need an item here. 
         */
	DebOut("CASE 2: %s\n",label);
        GTK_MUI(child)->MuiMenu=MenuObjectT(label), 
                                  MA_Widget, (ULONG) child,
                                  Child,
                                  MenuitemObject,
                                    MUIA_Menuitem_Title, (ULONG) label,
                                    MA_Widget, (ULONG) child,
                                  End,
                                End;
      }
      break;
    case 2:
    case 3:
      /* menuitem object */

      GTK_MUI(child)->MuiMenu=NewObject(CL_Menu->mcc_Class,NULL,
                                MUIA_Menuitem_Title, (ULONG) label,
                                MA_Widget, (ULONG) child, TAG_DONE);
      break;
    default:
      ErrOut("cannot build menu level %d\n",level);
  }

  /* add it */
  DoMethod(menuitem,MUIM_Family_AddTail,(ULONG) GTK_MUI(child)->MuiMenu);

  if(!menu_shell || !i) {
    return;          /* terminate */
  }

  level++;
  while(i) {
    mgtk_menu_append(GTK_MUI(child)->MuiMenu,GTK_WIDGET(i->data),level); /* ;) */
    i=i->next;
  }
}

static ULONG mContextMenu(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {

  GtkWidget    *menu_shell;
  GtkWidget    *menu_item;
  GtkWidget    *menu_item_accel;
  GList        *i=NULL;
  APTR          menustrip;
  APTR          menuitem1;
  const gchar  *label;

  if(!data->has_menu || !xget(obj,MA_Widget)) {
    return DoSuperMethodA(cl, obj, msg);
  }

  DebOut("group->mContextMenu\n");

  if(data->context_menu) {
    MUI_DisposeObject(data->context_menu);
    data->context_menu = NULL;
  }

  /* we are invoked by the accel widget obj */
  menu_item_accel=(GtkWidget *) xget(obj,MA_Widget); 

  /* The accel widget contains the (root) label of our new menu */
  label=gtk_label_get_text(GTK_LABEL(menu_item_accel));
  DebOut("root label=%s\n",label);

  menu_item=mgtk_get_accel_parent(menu_item_accel);

  /* new menustrip, level 0 */
  menustrip=MenustripObject,
              MA_Widget, (ULONG) menu_item_accel,
	      Child,
	      menuitem1=MenuObjectT(label), /* level 1 */
		MA_Widget, (ULONG) menu_item_accel,
	      End,
            End;
 
  DebOut("  new menustrip=%lx\n",menustrip);

  /* care for submenus  */
  menu_shell=gtk_menu_item_get_submenu(GTK_MENU_ITEM(menu_item));
  DebOut("  init mgtk_menu_append=%lx\n",menu_shell);

  if(!menu_shell) {
    WarnOut("  mContextMenu: GtkAccel %lx of GtkMenuItem %lx has empty menu!?",(ULONG) menu_item_accel,(ULONG) menu_item);
    return (ULONG) NULL; 
  }

  i=GTK_MENU_SHELL(menu_shell)->children;
  DebOut("  root menu_shell->children=%lx\n",GTK_MENU_SHELL(menu_shell)->children);

  /* fill menustrip */
  while(i) {
    mgtk_menu_append(menuitem1,GTK_WIDGET(i->data),2); /* ;) */
    i=i->next;
  }

  GTK_MUI(menu_item_accel)->MuiMenu=menustrip;

  DebOut("  mContextMenu returns: %lx\n",menustrip);

 /*  
  *  Remember: Even when overriding MUIM_ContextMenuBuild, you *must* set
  *  MUIA_ContextMenu of your object to something different from NULL.
  */

  data->context_menu=menustrip;

  return (ULONG) data->context_menu;
}

static ULONG mContextMenuC(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  GtkWidget *menu_item;
  struct MUIP_ContextMenuChoice *cmc=(struct MUIP_ContextMenuChoice *) msg;;

  DebOut("mContextMenuChoice(obj %lx)\n",obj);

  DebOut("  menuitem obj: %lx\n",cmc->item);

  menu_item=(GtkWidget *) xget(cmc->item,MA_Widget); 

  if(!menu_item) {
    ErrOut("classes/group.c: accel widget %lx has no parent widget !?\n",menu_item);
    return 0;
  }

  DebOut("  emit activate signal for %s widget %lx\n",g_type_name(G_OBJECT_TYPE(menu_item)),menu_item);

  g_signal_emit_by_name(menu_item,"activate");

  return 0;
}
/********************************************
 * mDispose
 *
 * Dispose Context Menu, if present
 ********************************************/

STATIC VOID mDispose(struct Data *data, APTR obj) {

  GtkWidget *menu_item_accel;

  DebOut("classes/group.c: mDispose(%lx)\n",obj);

  if(data->context_menu) {
    MUI_DisposeObject(data->context_menu);
    DebOut("                 ContextMenu %lx disposed\n",data->context_menu);
    data->context_menu = NULL;
    menu_item_accel=(GtkWidget *) xget(obj,MA_Widget); /* we are invoked by the accel widget obj */
    GTK_MUI(menu_item_accel)->MuiMenu=NULL;
  }
}

BEGINMTABLE
GETDATA;


  if(msg->MethodID >104) {
    DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);
  }

#if 0
  DebOut("  %lx parent: %lx\n",obj,_parent(obj));
  APTR foo=obj;

  while(foo) {
    DebOut("    parents: %lx\n",foo);
    foo=_parent(foo);
  }
#endif

  /*
   * DebOut("msg->MethodID: %lx\n",(ULONG) msg->MethodID);
   */
  switch (msg->MethodID) {

    case OM_NEW                : return mNew           (cl,   obj, msg);
    case OM_SET                :        mSet           (data, obj, (APTR)msg, cl); break;
    case OM_GET                : return mGet           (data, obj, (APTR)msg, cl); 
    case MUIM_AskMinMax        : return mgtk_askminmax (cl,   obj, (APTR)msg, data->defwidth, data->defheight);
    case MUIM_ContextMenuBuild : return mContextMenu   (data, obj, (APTR)msg, cl);
    case MUIM_ContextMenuChoice:        mContextMenuC  (data, obj, (APTR)msg, cl); break;
    case OM_DISPOSE            :        mDispose       (data, obj); break;
  }


ENDMTABLE

int mgtk_create_group_class(void) {

  CL_Group = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Group ? 1 : 0;
}

void mgtk_delete_group_class(void) {

  if (CL_Group) {
    MUI_DeleteCustomClass(CL_Group);
    CL_Group=NULL;
  }
}
