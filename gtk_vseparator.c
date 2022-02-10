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

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk/gtk.h"
#include "gtkvseparator.h"


static void gtk_vseparator_class_init (GtkVSeparatorClass *klass);
static void gtk_vseparator_init       (GtkVSeparator      *vseparator);
static gint gtk_vseparator_expose     (GtkWidget          *widget,
				       GdkEventExpose     *event);


GType
gtk_vseparator_get_type (void)
{
  static GType vseparator_type = 0;

  if (!vseparator_type)
    {
      static const GTypeInfo vseparator_info = {
	sizeof (GtkVSeparatorClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
	(GClassInitFunc) gtk_vseparator_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_init */      	
        sizeof (GtkVSeparator),
	0,
	(GInstanceInitFunc) gtk_vseparator_init,
      };

      vseparator_type = g_type_register_static (GTK_TYPE_SEPARATOR, "GtkVSeparator", &vseparator_info, 0);
    }

  return vseparator_type;
}

static void
gtk_vseparator_class_init (GtkVSeparatorClass *klass)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) klass;

  widget_class->expose_event = gtk_vseparator_expose;

}

static void
gtk_vseparator_init (GtkVSeparator *vseparator)
{
  DebOut("gtk_vseparator_init(lx)\n",vseparator);

  GTK_WIDGET (vseparator)->requisition.height = 1;
  GTK_WIDGET (vseparator)->requisition.width = GTK_WIDGET (vseparator)->style->xthickness;

  GtkSetObj(GTK_WIDGET(vseparator),RectangleObject, MUIA_Rectangle_VBar, TRUE, MUIA_FixHeight, 8, End);
}

GtkWidget*
gtk_vseparator_new (void)
{
  return GTK_WIDGET (gtk_type_new (GTK_TYPE_VSEPARATOR));
}

static gint
gtk_vseparator_expose (GtkWidget      *widget,
		       GdkEventExpose *event)
{
  if (GTK_WIDGET_DRAWABLE (widget))
    gtk_paint_vline (widget->style, widget->window, GTK_STATE_NORMAL,
		     &event->area, widget, "vseparator",
		     widget->allocation.y,
		     widget->allocation.y + widget->allocation.height,
		     widget->allocation.x + (widget->allocation.width -
					     widget->style->xthickness) / 2);

  return FALSE;
}

#define __GTK_VSEPARATOR_C__
