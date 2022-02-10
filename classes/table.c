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
 * $Id: table.c,v 1.23 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

/* table class
**
** emulates table widget in gtk
**
** we are using the gtk data structures and layout algorithms
** here, so this class got rather trivial (compared to the
** first three versions..)
**
*/

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "classes.h"
#include "gtk_globals.h"
#include "gtk.h"
#include "debug.h"

/* we need the size algorithm from gtk_table. It is too complex, to
 * redo it here ;). Better keep it, where it belongs to.
 */
void gtk_table_size_request  (GtkWidget *widget, GtkRequisition *requisition);
void gtk_table_size_allocate (GtkWidget *widget, GtkAllocation *allocation);


/*********************************************************************/

struct Data {

  struct Hook LayoutHook;
};

/*************************** LayoutHook *****************************/

#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm)
{
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif

  DebOut("LayoutHook\n");

  switch (lm->lm_Type) {

    case MUILM_MINMAX: {
      /* ======== MUILM_MINMAX ======= */
      GtkWidget *table;
      GtkRequisition *requisition;

      DebOut(" vvvvvvvvvvvvvvvvvvv MUILM_MINMAX vvvvvvvvvvvvvvvvvvv\n");
     

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

  /* ? */
  return MUILM_UNKNOWN;
}

/*************************** mNew *****************************/

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  DebOut("mNew\n");

  if (obj) {

    GETDATA;

    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
  }

  DebOut("mNew: %lx\n",obj);

  return (ULONG)obj;
}

/*************************** Dispatcher *****************************/

BEGINMTABLE

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID) {

    case OM_NEW         : return mNew       (cl,   obj, (APTR)msg);
  }

ENDMTABLE

/*************************** mgtk_create_table_class ****************/

int mgtk_create_table_class(void) {

  CL_Table = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Table ? 1 : 0;
}

/*************************** mgtk_create_table_class ****************/

void mgtk_delete_table_class(void) {

  if (CL_Table) {
    MUI_DeleteCustomClass(CL_Table);
    CL_Table=NULL;
  }
}
