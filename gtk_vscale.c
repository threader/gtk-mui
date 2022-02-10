/* GTK - The GIMP Toolkit
 * Copyright (C) 2001 Red Hat, Inc.
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
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: gtk_vscale.c,v 1.4 2007/06/19 13:33:18 o1i Exp $
 */


#include <config.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"
#include "gtk/gtkvscale.h"

#include "gtk_globals.h"
#include "mui.h"
#include "classes/classes.h"
#include "debug.h"
/*
#include "gtkintl.h"
#include "gtkalias.h"
*/

#define VALUE_SPACING 2

static gpointer parent_class;

static void     gtk_vscale_class_init       (GtkVScaleClass *klass);
static void     gtk_vscale_init             (GtkVScale      *vscale);
#if 0
static gboolean gtk_vscale_expose           (GtkWidget      *widget,
                                             GdkEventExpose *event);

static void	gtk_vscale_get_layout_offsets (GtkScale		*scale,
                                               gint		*x,
                                               gint		*y);
#endif

GType gtk_vscale_get_type (void) {
  static GType vscale_type = 0;

  DebOut("gtk_vscale_get_type()\n");
  
  if (!vscale_type) {
    static const GTypeInfo vscale_info = {
      sizeof (GtkVScaleClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_vscale_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkVScale),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_vscale_init,
    };
      
    vscale_type = g_type_register_static (GTK_TYPE_SCALE, "GtkVScale",
					    &vscale_info, 0);
  }
  
  return vscale_type;
}

static void gtk_vscale_class_init (GtkVScaleClass *class) {

  GtkWidgetClass *widget_class;
  GtkRangeClass *range_class;
  GtkScaleClass *scale_class;

  DebOut("gtk_vscale_class_init(%lx)\n",class);
  
  widget_class = GTK_WIDGET_CLASS (class);
  range_class = GTK_RANGE_CLASS (class); 
  scale_class = GTK_SCALE_CLASS (class); 

  parent_class = g_type_class_peek_parent (class);

  range_class->slider_detail = "vscale";
  
#if 0
  scale_class->get_layout_offsets = gtk_vscale_get_layout_offsets;

  widget_class->expose_event = gtk_vscale_expose;
#endif
}

static void gtk_vscale_init (GtkVScale *vscale) {

  GtkRange *range;

  DebOut("gtk_vscale_init(%lx)\n",vscale);

  range = GTK_RANGE (vscale);
  
  range->orientation = GTK_ORIENTATION_VERTICAL;

  GtkSetObj(GTK_WIDGET(vscale), NewObject(CL_Scale->mcc_Class, NULL,
                                       MA_Widget,(ULONG) vscale, 
                                       MUIA_Slider_Horiz, FALSE,
                                       MUIA_Numeric_Min,0,
                                       MUIA_Numeric_Max,10,
                                       MUIA_Numeric_Value,0,
                                      TAG_DONE));

  if(!GTK_MUI(vscale)->MuiObject) {
    ErrOut("unable to create GTK_MUI(vscale)->MuiObject\n");
  }

  GTK_MUI(vscale)->mainclass=CL_AREA;
  DebOut("gtk_vscale_init ok\n");

}

GtkWidget* gtk_vscale_new (GtkAdjustment *adjustment) {

  GtkWidget *w;

  DebOut("gtk_vscale_new(%lx)\n",adjustment);

  w=g_object_new (GTK_TYPE_VSCALE, "adjustment", adjustment, NULL);

  if(!GTK_RANGE(w)->adjustment->step_increment) {
    ErrOut("gtk_vscale.c: step_increment is zero (Division by zero error avoided)!");
    GTK_RANGE(w)->adjustment->step_increment=1;
  }

  GtkSetObj(w, NewObject(CL_Scale->mcc_Class, NULL,
                                       MA_Widget,(ULONG) w, 
                                       MUIA_Slider_Horiz, FALSE,
                                       MUIA_Numeric_Min,GTK_RANGE(w)->adjustment->lower / GTK_RANGE(w)->adjustment->step_increment,
                                       MUIA_Numeric_Max,GTK_RANGE(w)->adjustment->upper / GTK_RANGE(w)->adjustment->step_increment,
                                       MUIA_Numeric_Value,GTK_RANGE(w)->adjustment->value / GTK_RANGE(w)->adjustment->step_increment,
                                       TAG_DONE));

  if(!GTK_MUI(w)->MuiObject) {
    ErrOut("unable to create GTK_MUI(w)->MuiObject\n");
  }

  DoMethod(GtkObj(w),MM_Scale_Redraw,(ULONG) 1);

  return w;
}


/**
 * gtk_vscale_new_with_range:
 * @min: minimum value
 * @max: maximum value
 * @step: step increment (tick size) used with keyboard shortcuts
 * 
 * Creates a new vertical scale widget that lets the user input a
 * number between @min and @max (including @min and @max) with the
 * increment @step.  @step must be nonzero; it's the distance the
 * slider moves when using the arrow keys to adjust the scale value.
 * 
 * Note that the way in which the precision is derived works best if @step 
 * is a power of ten. If the resulting precision is not suitable for your 
 * needs, use gtk_scale_set_digits() to correct it.
 *
 * Return value: a new #GtkVScale
 **/
GtkWidget*
gtk_vscale_new_with_range (gdouble min,
                           gdouble max,
                           gdouble step)
{
  GtkObject *adj;
  GtkScale *scale;
  gint digits;

  g_return_val_if_fail (min < max, NULL);
  g_return_val_if_fail (step != 0.0, NULL);

  adj = (GtkObject *) gtk_adjustment_new (min, min, max, step, 10 * step, 0);
  
  scale = g_object_new (GTK_TYPE_VSCALE,
                        "adjustment", adj,
                        NULL);

  if (fabs (step) >= 1.0 || step == 0.0)
    digits = 0;
  else {
    digits = abs ((gint) floor (log10 (fabs (step))));
    if (digits > 5)
      digits = 5;
  }

  gtk_scale_set_digits (scale, digits);
  
  return GTK_WIDGET (scale);
}

#if 0
static gboolean
gtk_vscale_expose (GtkWidget      *widget,
                   GdkEventExpose *event)
{
  GtkScale *scale;
  
  scale = GTK_SCALE (widget);
  
  /* We need to chain up _first_ so the various geometry members of
   * GtkRange struct are updated.
   */
  if (GTK_WIDGET_CLASS (parent_class)->expose_event)
    GTK_WIDGET_CLASS (parent_class)->expose_event (widget, event);

  if (scale->draw_value)
    {
      PangoLayout *layout;
      gint x, y;
      GtkStateType state_type;

      layout = gtk_scale_get_layout (scale);
      gtk_scale_get_layout_offsets (scale, &x, &y);

      state_type = GTK_STATE_NORMAL;
      if (!GTK_WIDGET_IS_SENSITIVE (scale))
        state_type = GTK_STATE_INSENSITIVE;

      gtk_paint_layout (widget->style,
                        widget->window,
                        state_type,
			FALSE,
                        NULL,
                        widget,
                        "vscale",
                        x, y,
                        layout);
    }
  
  return FALSE;

}

static void
gtk_vscale_get_layout_offsets (GtkScale *scale,
                               gint     *x,
                               gint     *y)
{
  GtkWidget *widget;
  GtkRange *range;
  PangoLayout *layout;
  PangoRectangle logical_rect;
  gint value_spacing;

  widget = GTK_WIDGET (scale);
  layout = gtk_scale_get_layout (scale);
      
  if (!layout)
    {
      *x = 0;
      *y = 0;

      return;
    }

  range = GTK_RANGE (widget);
  scale = GTK_SCALE (widget);

  gtk_widget_style_get (widget, "value_spacing", &value_spacing, NULL);
      
  pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
    
  switch (scale->value_pos)
    {
    case GTK_POS_LEFT:
      *x = range->range_rect.x - logical_rect.width - value_spacing;
      *y = range->slider_start + (range->slider_end - range->slider_start - logical_rect.height) / 2;
      *y = CLAMP (*y, 0, widget->allocation.height - logical_rect.height);
      break;
      
    case GTK_POS_RIGHT:
      *x = range->range_rect.x + range->range_rect.width + value_spacing;
      *y = range->slider_start + (range->slider_end - range->slider_start - logical_rect.height) / 2;
      *y = CLAMP (*y, 0, widget->allocation.height - logical_rect.height);
      break;
          
    case GTK_POS_TOP:
      *x = range->range_rect.x + (range->range_rect.width - logical_rect.width) / 2;
      *y = range->range_rect.y - logical_rect.height - value_spacing;
      break;
          
    case GTK_POS_BOTTOM:
      *x = range->range_rect.x + (range->range_rect.width - logical_rect.width) / 2;
      *y = range->range_rect.y + range->range_rect.height + value_spacing;
      break;

    default:
      g_return_if_reached ();
      *x = 0;
      *y = 0;
      break;
    }

  *x += widget->allocation.x;
  *y += widget->allocation.y;
}
#endif

#define __GTK_VSCALE_C__
