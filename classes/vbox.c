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
 * $Id: vbox.c,v 1.1 2007/11/20 10:42:00 o1i Exp $
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

void gtk_vbox_size_request (GtkWidget *widget, GtkRequisition *requisition);
void gtk_vbox_size_allocate (GtkWidget *widget, GtkAllocation *allocation);

struct Data {

  struct Hook LayoutHook;
  LONG defwidth, defheight;
};

#ifndef __AROS__
HOOKPROTO(HLayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm)
{
#else 
AROS_UFH3(static ULONG, HLayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif

  GtkWidget *vbox;

  DebOut("HLayoutHook\n");

  switch (lm->lm_Type) {

    case MUILM_MINMAX: {
      /* ======== MUILM_MINMAX ======= */
      DebOut(" vvvvvvvvvvvvvvvvvvv MUILM_MINMAX vvvvvvvvvvvvvvvvvvv\n");

      vbox=mgtk_get_widget_from_obj(obj);

      if( !xget(obj,MUIA_Parent) || 
	  !vbox ||
	  !gtk_widget_get_parent(vbox) ||
	  !GTK_WIDGET_VISIBLE(vbox)) {
	DebOut("  mui obj/widget has no parent, setting default values\n");
	DebOut("    xget(obj,MUIA_Parent): %lx\n",xget(obj,MUIA_Parent));
	DebOut("    vbox: %lx\n",vbox);
	DebOut("    gtk_widget_get_parent(vbox): %lx\n",gtk_widget_get_parent(vbox));
	DebOut("    GTK_WIDGET_VISIBLE(vbox): %lx\n",GTK_WIDGET_VISIBLE(vbox));

	lm->lm_MinMax.MinWidth  = 1;
	lm->lm_MinMax.MinHeight = 1;
	lm->lm_MinMax.DefWidth  = 1;
	lm->lm_MinMax.DefHeight = 1;
	lm->lm_MinMax.MaxWidth  = MUI_MAXMAX;
	lm->lm_MinMax.MaxHeight = MUI_MAXMAX;
      }
      else {
	GtkRequisition *requisition;


	requisition=mgtk_allocmem(sizeof(GtkRequisition),MEMF_CLEAR);

	/* 
	 * MUI knows three kinds of requisitions: min, def and max.
	 * gtk_vbox_size_request only one.
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
	gtk_vbox_size_request(vbox,requisition);
	lm->lm_MinMax.MinWidth  = requisition->width;
	lm->lm_MinMax.MinHeight = requisition->height;
	DebOut("  MUILM_MINMAX: min width: %d height: %d\n",requisition->width,requisition->height);

	requisition->gtkmui_type=3;
	gtk_vbox_size_request(vbox,requisition);
	lm->lm_MinMax.MaxWidth  = requisition->width;
	lm->lm_MinMax.MaxHeight = requisition->height;
	DebOut("  MUILM_MINMAX: max width: %d height: %d\n",requisition->width,requisition->height);
   
	/* we call type 2 after all others, as those values
	 * are "the best fitting" for GTK and the call to gtk_vbox_size_request
	 * sets some values, which are used later on.
	 */
	requisition->gtkmui_type=2;
	gtk_vbox_size_request(vbox,requisition);
	lm->lm_MinMax.DefWidth  = (LONG) requisition->width;
	lm->lm_MinMax.DefHeight = (LONG) requisition->height;
	DebOut("  MUILM_MINMAX: def width: %d height: %d\n",requisition->width,requisition->height);

	mgtk_freemem(requisition,sizeof(GtkRequisition));
      }
      DebOut(" ^^^^^^^^^^^^^^^^^^^ MUILM_MINMAX ^^^^^^^^^^^^^^^^^^^\n");
      return 0;
    }
    /* ======== MUILM_LAYOUT ======= */

    case MUILM_LAYOUT: {
      GtkAllocation *allocation;

      DebOut(" vvvvvvvvvvvvvvvvvvv MUILM_LAYOUT vvvvvvvvvvvvvvvvvvv\n");

      vbox=mgtk_get_widget_from_obj(obj);

      if( !xget(obj,MUIA_Parent) || 
	  !vbox ||
	  !GTK_WIDGET_VISIBLE(vbox) ||
	  !gtk_widget_get_parent(vbox)) {
	WarnOut("  mui object has no parent, no need to care for it !? -> possible freeze is caused here.\n");
	/* WHAT TO DO HERE? */
	/* here we crash.. */
      }
      else {
	allocation=mgtk_allocmem(sizeof(GtkAllocation),MEMF_CLEAR);

	allocation->width = (gint) lm->lm_Layout.Width; 
	allocation->height= (gint) lm->lm_Layout.Height;
	allocation->x= 0;
	allocation->y= 0;

	/* up to here, everything seems ok */
	DebOut("MUILM_LAYOUT: x=y=0, width=%d height=%d\n",allocation->width,allocation->height);

	gtk_vbox_size_allocate(vbox,allocation);

	mgtk_freemem(allocation,sizeof(GtkAllocation));
      }
   
      DebOut(" ^^^^^^^^^^^^^^^^^^^ MUILM_LAYOUT ^^^^^^^^^^^^^^^^^^^\n");
      return TRUE;
    }
  }

  /* ? */
  return MUILM_UNKNOWN;
}

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  DebOut("mNew: %lx\n",obj);

  if (obj) {
    GETDATA;

#if 0
    /* default is a vertical group */
    data->horiz        =  0;
    data->homogeneous  = -1;
    data->spacing      = -1;
#endif
    data->defheight    = AskMin_Unlimited; /* use all space */
    data->defwidth     = AskMin_Unlimited;
#if 0
    data->is_resizable = -1;
    data->fill         =  1;

    if(xget(obj,MUIA_Group_Horiz)) {
      data->horiz        =  1;
    }
#endif
    SETUPHOOK(&data->LayoutHook, HLayoutHook, data);
    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
  }

  return (ULONG)obj;
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, struct IClass *cl) {

  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

  DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

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
#if 0
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
            DebOut("TODO: MA_Group_Fill,TRUE is to be tested!\n");
            data->defwidth  = AskMin_Unlimited;
            data->defheight = AskMin_Unlimited;
          }
          relayout = 1;
        }
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
#endif
        /* the break is missing with a reason ;) */
      default:
        DoSuperMethodA(cl, obj, (Msg)msg);
        break;
    }
  }

#if 0
  if (relayout) {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
#endif
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


/********************************************
 * mDispose
 *
 * Dispose Context Menu, if present
 ********************************************/

STATIC VOID mDispose(struct Data *data, APTR obj) {

  DebOut("mDispose(%lx)\n",obj);

}

static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {
  int ret;
  DebOut("mAddMember(%lx,%lx)\n",obj,msg->opam_Object);
  ret=DoSuperMethodA(cl, obj, (Msg)msg);
  DebOut("mAddMember(%lx,%lx) returns: %d\n",obj,msg->opam_Object,ret);
  return ret;
}

BEGINMTABLE
GETDATA;

  DebOut("msg->MethodID: %lx\n",(ULONG) msg->MethodID);
  switch (msg->MethodID) {

    case OM_NEW                : return mNew           (cl,   obj, msg);
    case OM_ADDMEMBER          : return mAddMember     (data, obj, (APTR)msg, cl);

#if 0
    case OM_SET                :        mSet           (data, obj, (APTR)msg, cl); break;
#endif
    case MUIM_AskMinMax        : return mgtk_askminmax (cl,   obj, (APTR)msg, data->defwidth, data->defheight);
    case OM_DISPOSE            :        mDispose       (data, obj); break;
  }

  DebOut("  msg->MethodID: %lx done\n",(ULONG) msg->MethodID);


ENDMTABLE

int mgtk_create_vbox_class(void) {

  CL_VBox = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
//  CL_VBox = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_VBox ? 1 : 0;
}

void mgtk_delete_vbox_class(void) {

  if (CL_VBox) {
    MUI_DeleteCustomClass(CL_VBox);
    CL_VBox=NULL;
  }
}
