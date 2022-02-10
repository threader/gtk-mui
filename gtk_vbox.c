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
 * $Id: gtk_vbox.c,v 1.5 2007/11/20 10:42:00 o1i Exp $
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
#include "gtk/gtkvbox.h"
#include "classes/classes.h"
#include "gtk_globals.h"

#include "debug.h"

static void gtk_vbox_class_init    (GtkVBoxClass   *klass);
static void gtk_vbox_init          (GtkVBox        *box);
void gtk_vbox_size_request         (GtkWidget      *widget,
				    GtkRequisition *requisition);
void gtk_vbox_size_allocate        (GtkWidget      *widget,
				    GtkAllocation  *allocation);


GType gtk_vbox_get_type (void) {

  static GType vbox_type = 0;

  if (!vbox_type) {
    static const GTypeInfo vbox_info = {
      sizeof (GtkVBoxClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_vbox_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkVBox),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_vbox_init,
    };

    vbox_type = g_type_register_static (GTK_TYPE_BOX, "GtkVBox",
					  &vbox_info, 0);
  }

  return vbox_type;
}

static void
gtk_vbox_class_init (GtkVBoxClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;

  widget_class->size_request = gtk_vbox_size_request;
  widget_class->size_allocate = gtk_vbox_size_allocate;
}

static void
gtk_vbox_init (GtkVBox *vbox)
{
}

GtkWidget* gtk_vbox_new (gboolean homogeneous, gint spacing) {

  GtkVBox *vbox;

  DebOut("gtk_vbox_new(%d,%d)\n",homogeneous,spacing);

  vbox = g_object_new (GTK_TYPE_VBOX, NULL);

  GTK_BOX (vbox)->spacing = spacing;
  GTK_BOX (vbox)->homogeneous = homogeneous ? TRUE : FALSE;

  GtkSetObj(GTK_WIDGET(vbox), NewObject(CL_VBox->mcc_Class, NULL,
                                              MA_Widget,(LONG) vbox,
                                              MUIA_Group_Horiz,FALSE,
                                              TAG_DONE));
  /* does this work!? */
  if(homogeneous) {
    SetAttrs(GtkObj(vbox),MUIA_Group_SameHeight,(LONG) 1, TAG_DONE);
  }
  
  DebOut("gtk_vbox_new: %lx\n",vbox);

  GTK_MUI(vbox)->mainclass = CL_AREA;

  return GTK_WIDGET (vbox);
}


void gtk_vbox_size_request (GtkWidget *widget, GtkRequisition *requisition) {

  GtkBox *box;
  GtkBoxChild *child;
  GList *children;
  gint nvis_children;
  gint height;

  DebOut("gtk_vbox_size_request(%lx,%lx)\n",widget,requisition);

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
	  DebOut("  child is visible\n");
	  child_requisition.gtkmui_type=requisition->gtkmui_type;

	  gtk_widget_size_request (child->widget, &child_requisition);

	  DebOut("   child_requisition.width:  %d\n",child_requisition.width);
	  DebOut("   child_requisition.height: %d\n",child_requisition.height);

	  if (box->homogeneous)
	    {
	      height = child_requisition.height + child->padding * 2;
	      requisition->height = MAX (requisition->height, height);
	    }
	  else
	    {
	      requisition->height += child_requisition.height + child->padding * 2;
	    }

	  requisition->width = MAX (requisition->width, child_requisition.width);

	  nvis_children += 1;
	}
    }

  if (nvis_children > 0)
    {
      if (box->homogeneous)
	requisition->height *= nvis_children;
      requisition->height += (nvis_children - 1) * box->spacing;
    }

  requisition->width += GTK_CONTAINER (box)->border_width * 2;
  requisition->height += GTK_CONTAINER (box)->border_width * 2;
}

void gtk_vbox_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {

  GtkBox *box;
  GtkBoxChild *child;
  GList *children;
  GtkAllocation child_allocation;
  gint nvis_children;
  gint nexpand_children;
  gint child_height;
  gint height;
  gint extra;
  gint y;

  DebOut("gtk_vbox_size_allocate(%lx,%lx)\n",widget,allocation);

  box = GTK_BOX (widget);
  widget->allocation = *allocation;

  nvis_children = 0;
  nexpand_children = 0;
  children = box->children;

  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  nvis_children += 1;
	  if (child->expand)
	    nexpand_children += 1;
	}
    }

  if (nvis_children > 0)
    {
      if (box->homogeneous)
	{
	  height = (allocation->height -
		   GTK_CONTAINER (box)->border_width * 2 -
		   (nvis_children - 1) * box->spacing);
	  extra = height / nvis_children;
	}
      else if (nexpand_children > 0)
	{
#if 0
	  height = (gint) allocation->height - (gint) widget->requisition.height;
#endif
	  height = (gint) allocation->height - (gint) _defheight(GtkObj(widget));
	  extra = height / nexpand_children;
	}
      else
	{
	  height = 0;
	  extra = 0;
	}

      DebOut("  height: %dn",height);

      y = allocation->y + GTK_CONTAINER (box)->border_width;
      child_allocation.x = allocation->x + GTK_CONTAINER (box)->border_width;
      child_allocation.width = MAX (1, (gint) allocation->width - (gint) GTK_CONTAINER (box)->border_width * 2);

      DebOut("  y: %d\n",y);

      children = box->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  DebOut("  w-child: %lx\n",child);

	  if ((child->pack == GTK_PACK_START) && GTK_WIDGET_VISIBLE (child->widget))
	    {
	      if (box->homogeneous)
		{
		  if (nvis_children == 1)
		    child_height = height;
		  else
		    child_height = extra;

		  nvis_children -= 1;
		  height -= extra;
		}
	      else
		{
		  GtkRequisition child_requisition;
		  child_requisition.gtkmui_type=2; /* defheight */

		  gtk_widget_get_child_requisition (child->widget, &child_requisition);
		  child_height = child_requisition.height + child->padding * 2;

		  if (child->expand)
		    {
		      if (nexpand_children == 1)
			child_height += height;
		      else
			child_height += extra;

		      nexpand_children -= 1;
		      height -= extra;
		    }
		}

	      if (child->fill)
		{
		  child_allocation.height = MAX (1, child_height - (gint)child->padding * 2);
		  child_allocation.y = y + child->padding;
		}
	      else
		{
		  GtkRequisition child_requisition;
		  child_requisition.gtkmui_type=2; /* defheight */

		  gtk_widget_get_child_requisition (child->widget, &child_requisition);
		  child_allocation.height = child_requisition.height;
		  child_allocation.y = y + (child_height - child_allocation.height) / 2;
		}

	      gtk_widget_size_allocate (child->widget, &child_allocation);

	      y += child_height + box->spacing;
	    }
	}

      y = allocation->y + allocation->height - GTK_CONTAINER (box)->border_width;

      children = box->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if ((child->pack == GTK_PACK_END) && GTK_WIDGET_VISIBLE (child->widget))
	    {
	      GtkRequisition child_requisition;
	      gtk_widget_get_child_requisition (child->widget, &child_requisition);
	      child_requisition.gtkmui_type=2; /* defheight */

              if (box->homogeneous)
                {
                  if (nvis_children == 1)
                    child_height = height;
                  else
                    child_height = extra;

                  nvis_children -= 1;
                  height -= extra;
                }
              else
                {
		  child_height = child_requisition.height + child->padding * 2;

                  if (child->expand)
                    {
                      if (nexpand_children == 1)
                        child_height += height;
                      else
                        child_height += extra;

                      nexpand_children -= 1;
                      height -= extra;
                    }
                }

              if (child->fill)
                {
                  child_allocation.height = MAX (1, child_height - (gint)child->padding * 2);
                  child_allocation.y = y + child->padding - child_height;
                }
              else
                {
		  child_allocation.height = child_requisition.height;
                  child_allocation.y = y + (child_height - child_allocation.height) / 2 - child_height;
                }

              gtk_widget_size_allocate (child->widget, &child_allocation);

              y -= (child_height + box->spacing);
	    }
	}
    }
}

#define __GTK_VBOX_C__
