/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

/*
 * Modified by the GTK-MUI Team 2007
 *
 * $Id: gtk_hbox.c,v 1.6 2007/07/30 14:44:50 o1i Exp $
 */

#include <config.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#ifdef __AROS__
#include <dos/dos.h>
#endif
#include <mui.h>

#include "gtk/gtk.h"
#include "gtk/gtkhbox.h"
#include "classes/classes.h"
#include "gtk_globals.h"
#include "debug.h"


static void gtk_hbox_class_init    (GtkHBoxClass   *klass);
static void gtk_hbox_init          (GtkHBox        *box);
void gtk_hbox_size_request         (GtkWidget      *widget,
				    GtkRequisition *requisition);
void gtk_hbox_size_allocate        (GtkWidget      *widget,
				    GtkAllocation  *allocation);


GType gtk_hbox_get_type (void) {

  static GType hbox_type = 0;

  if (!hbox_type) {
    static const GTypeInfo hbox_info = {
      sizeof (GtkHBoxClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_hbox_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkHBox),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_hbox_init,
    };

    hbox_type = g_type_register_static (GTK_TYPE_BOX, "GtkHBox",
					  &hbox_info, 0);
  }

  return hbox_type;
}

static void
gtk_hbox_class_init (GtkHBoxClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;

  widget_class->size_request = gtk_hbox_size_request;
  widget_class->size_allocate = gtk_hbox_size_allocate;
}

static void gtk_hbox_init (GtkHBox *hbox) {

  GTK_MUI(hbox)->mainclass=CL_AREA;

}

GtkWidget* gtk_hbox_new (gboolean homogeneous, gint spacing) {

  GtkHBox *hbox;

  DebOut("gtk_hbox_new(%d,%d)\n",homogeneous,spacing);

  hbox = g_object_new (GTK_TYPE_HBOX, NULL);

  GTK_BOX (hbox)->spacing = spacing;
  GTK_BOX (hbox)->homogeneous = homogeneous ? TRUE : FALSE;

  GtkSetObj(GTK_WIDGET(hbox), NewObject(CL_HBox->mcc_Class, NULL,
                                              MA_Widget,(LONG) hbox,
                                              MUIA_Group_Horiz,TRUE,
                                              TAG_DONE));

  /* does this work!? */
  if(homogeneous) {
    SetAttrs(GtkObj(hbox),MUIA_Group_SameWidth,(LONG) 1, TAG_DONE);
  }

  return GTK_WIDGET (hbox);
}


void gtk_hbox_size_request (GtkWidget *widget, GtkRequisition *requisition) {
  GtkBox *box;
  GtkBoxChild *child;
  GList *children;
  gint nvis_children;
  gint width;

  DebOut("gtk_hbox_size_request(%lx,%lx)\n",widget,requisition);

  box = GTK_BOX (widget);
  requisition->width = 0;
  requisition->height = 0;
  nvis_children = 0;

  children = box->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      DebOut("  child: %lx\n",child);

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  GtkRequisition child_requisition;
	  DebOut("   child is visible\n");
	  child_requisition.gtkmui_type=requisition->gtkmui_type;

	  gtk_widget_size_request (child->widget, &child_requisition);

	  DebOut("   child_requisition.width:  %d\n",child_requisition.width);
	  DebOut("   child_requisition.height: %d\n",child_requisition.height);

	  if (box->homogeneous)
	    {
	      width = child_requisition.width + child->padding * 2;
	      requisition->width = MAX (requisition->width, width);
	    }
	  else
	    {
	      requisition->width += child_requisition.width + child->padding * 2;
	    }

	  requisition->height = MAX (requisition->height, child_requisition.height);

	  nvis_children += 1;
	}
    }

  if (nvis_children > 0)
    {
      if (box->homogeneous)
	requisition->width *= nvis_children;
      requisition->width += (nvis_children - 1) * box->spacing;
    }

  requisition->width += GTK_CONTAINER (box)->border_width * 2;
  requisition->height += GTK_CONTAINER (box)->border_width * 2;
}

void gtk_hbox_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {
  GtkBox *box;
  GtkBoxChild *child;
  GList *children;
  GtkAllocation child_allocation;
  gint nvis_children;
  gint nexpand_children;
  gint child_width;
  gint width;
  gint extra;
  gint x;
  GtkTextDirection direction;

  DebOut("gtk_hbox_size_allocate(%lx,%lx)\n",widget,allocation);

  box = GTK_BOX (widget);
  widget->allocation = *allocation;

  direction = gtk_widget_get_direction (widget);
  
  nvis_children = 0;
  nexpand_children = 0;
  children = box->children;

  while (children) {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  nvis_children += 1;
	  if (child->expand)
	    nexpand_children += 1;
	}
    }

  DebOut("  visible children: %d\n",nvis_children);

  if (nvis_children > 0) {
    if (box->homogeneous) {
      width = (allocation->width -
		GTK_CONTAINER (box)->border_width * 2 -
	     	(nvis_children - 1) * box->spacing);
      extra = width / nvis_children;
    }
    else if (nexpand_children > 0) {
#if 0
      width = (gint) allocation->width - (gint) widget->requisition.width;
#endif
      width = (gint) allocation->width - (gint) _defwidth(GtkObj(widget));
      extra = width / nexpand_children;
    }
    else {
      width = 0;
      extra = 0;
    }

    DebOut("    width: %d\n",width);

    x = allocation->x + GTK_CONTAINER (box)->border_width;
    child_allocation.y = allocation->y + GTK_CONTAINER (box)->border_width;
    child_allocation.height = MAX (1, (gint) allocation->height - (gint) GTK_CONTAINER (box)->border_width * 2);

    DebOut("    x: %d\n",x);

    children = box->children;

    while (children) {

      child = children->data;
      children = children->next;
      DebOut("  w-child: %lx\n",child);

      if ((child->pack == GTK_PACK_START) && GTK_WIDGET_VISIBLE (child->widget)) {
	if (box->homogeneous) {
	  //DebOut("  box->homogeneous\n");
	  if (nvis_children == 1)
	    child_width = width;
	  else
    	    child_width = extra;

	  nvis_children -= 1;
	  width -= extra;
	}
	else {
	  GtkRequisition child_requisition;
	  //DebOut("  !box->homogeneous\n");
	  child_requisition.gtkmui_type=2; /* defwidth*/

	  gtk_widget_get_child_requisition (child->widget, &child_requisition);
	  child_width = child_requisition.width + child->padding * 2;
	  //DebOut("  child_requisition.width: %d\n",child_requisition.width);
	  //DebOut("  child->padding: %d\n",child->padding);

	  if (child->expand) {
	    if (nexpand_children == 1) {
	      child_width += width;
	    }
	    else {
	      child_width += extra;
	    }

	    nexpand_children -= 1;
	    width -= extra;
	  }
	}
    	//DebOut("  child_width 1: %d\n",child_width);

	if (child->fill) {
	  child_allocation.width = MAX (1, (gint) child_width - (gint) child->padding * 2);
	  child_allocation.x = x + child->padding;
	  //DebOut("  2=> child_allocation.x: %d\n",child_allocation.x);
	}
	else {
	  GtkRequisition child_requisition;
	  child_requisition.gtkmui_type=2; /* defwidth*/

	  //DebOut("calling !fill gtk_widget_get_child_requisition\n");
	  gtk_widget_get_child_requisition (child->widget, &child_requisition);
	  child_allocation.width = child_requisition.width;
	  child_allocation.x = x + (child_width - child_allocation.width) / 2;

	  //DebOut("  x: %d\n",x);
	  //DebOut("  child_width: %d\n",child_width);
	  //DebOut("  child_allocation.width: %d\n",child_allocation.width);
	  //DebOut("  1=> child_allocation.x: %d\n",child_allocation.x);
	}

	//DebOut("  allocation->x: %d\n",allocation->x);
	//DebOut("  allocation->width: %d\n",allocation->width);
	//DebOut("  child_allocation.x: %d\n",child_allocation.x);
	//DebOut("  child_allocation.width: %d\n",child_allocation.width);
	//DebOut("  => child_allocation.x: %d\n",child_allocation.x);

	if (direction == GTK_TEXT_DIR_RTL) {
	  child_allocation.x = allocation->x + allocation->width - (child_allocation.x - allocation->x) - child_allocation.width;
	}

	DebOut("-> gtk_widget_size_allocate 1\n");
	gtk_widget_size_allocate (child->widget, &child_allocation);

	x += child_width + box->spacing;
      }
    }

    x = allocation->x + allocation->width - GTK_CONTAINER (box)->border_width;

    children = box->children;

    while (children) {
      child = children->data;
      children = children->next;

      if ((child->pack == GTK_PACK_END) && GTK_WIDGET_VISIBLE (child->widget)) {
	GtkRequisition child_requisition;
	child_requisition.gtkmui_type=2; /* defwidth*/

	gtk_widget_get_child_requisition (child->widget, &child_requisition);

	if (box->homogeneous) {
	  if (nvis_children == 1)
	    child_width = width;
	  else
	    child_width = extra;

	  nvis_children -= 1;
	  width -= extra;
	}
	else {
	  child_width = child_requisition.width + child->padding * 2;

       	  if (child->expand) {
	    if (nexpand_children == 1)
	      child_width += width;
	    else
      	      child_width += extra;

	    nexpand_children -= 1;
	    width -= extra;
	  }
	}

	if (child->fill) {
	  child_allocation.width = MAX (1, (gint)child_width - (gint)child->padding * 2);
	  child_allocation.x = x + child->padding - child_width;
	}
	else {
	  child_allocation.width = child_requisition.width;
       	  child_allocation.x = x + (child_width - child_allocation.width) / 2 - child_width;
	}

	if (direction == GTK_TEXT_DIR_RTL)
	  child_allocation.x = allocation->x + allocation->width - (child_allocation.x - allocation->x) - child_allocation.width;


	DebOut("-> gtk_widget_size_allocate 2\n");
	gtk_widget_size_allocate (child->widget, &child_allocation);

	x -= (child_width + box->spacing);
      }
    }
  }
}

#define __GTK_HBOX_C__
