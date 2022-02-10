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
 * Modified by the GTK+ Team and others 1997-2001.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: gtk_arrow.c,v 1.16 2007/03/19 09:00:06 o1i Exp $
 */

#include <config.h>
#include <math.h>
#include <stdio.h>

#include "gtk/gtk.h"
#include "gtk/gtkarrow.h"

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk_globals.h"
#include "classes/classes.h"


#define MIN_ARROW_SIZE  15

enum {
  PROP_0,

  PROP_ARROW_TYPE,
  PROP_SHADOW_TYPE,
  
  PROP_LAST
};


static void gtk_arrow_class_init (GtkArrowClass  *klass);
static void gtk_arrow_init       (GtkArrow       *arrow);
#if 0
static gint gtk_arrow_expose     (GtkWidget      *widget,
				  GdkEventExpose *event);
#endif
static void gtk_arrow_set_property (GObject         *object,
				    guint            prop_id,
				    const GValue    *value,
				    GParamSpec      *pspec);
static void gtk_arrow_get_property (GObject         *object,
				    guint            prop_id,
				    GValue          *value,
				    GParamSpec      *pspec);

GType gtk_arrow_get_type (void) {

  static GType arrow_type = 0;

  DebOut("gtk_arrow_get_type()\n");

  if (!arrow_type) {
    static const GTypeInfo arrow_info = {
      sizeof (GtkArrowClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_arrow_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkArrow),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_arrow_init,
    };

    arrow_type = g_type_register_static (GTK_TYPE_MISC, "GtkArrow",
					   &arrow_info, 0);
  }

  return arrow_type;
}

static void gtk_arrow_class_init (GtkArrowClass *class) {

  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;

  DebOut("gtk_arrow_class_init(%lx)\n",class);

  gobject_class = (GObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;

  gobject_class->set_property = gtk_arrow_set_property;
  gobject_class->get_property = gtk_arrow_get_property;
  
  g_object_class_install_property (gobject_class,
                                   PROP_ARROW_TYPE,
                                   g_param_spec_enum ("arrow_type",
                                                      P_("Arrow direction"),
                                                      P_("The direction the arrow should point"),
						      GTK_TYPE_ARROW_TYPE,
						      GTK_ARROW_RIGHT,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_SHADOW_TYPE,
                                   g_param_spec_enum ("shadow_type",
                                                      P_("Arrow shadow"),
                                                      P_("Appearance of the shadow surrounding the arrow"),
						      GTK_TYPE_SHADOW_TYPE,
						      GTK_SHADOW_OUT,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));
  
#if 0
  widget_class->expose_event = gtk_arrow_expose;
#endif
}

static void
gtk_arrow_set_property (GObject         *object,
			guint            prop_id,
			const GValue    *value,
			GParamSpec      *pspec) {

  GtkArrow *arrow;
  
  arrow = GTK_ARROW (object);

  switch (prop_id)
    {
    case PROP_ARROW_TYPE:
      gtk_arrow_set (arrow,
		     g_value_get_enum (value),
		     arrow->shadow_type);
      break;
    case PROP_SHADOW_TYPE:
      gtk_arrow_set (arrow,
		     arrow->arrow_type,
		     g_value_get_enum (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
gtk_arrow_get_property (GObject         *object,
			guint            prop_id,
			GValue          *value,
			GParamSpec      *pspec)
{
  GtkArrow *arrow;
  
  arrow = GTK_ARROW (object);
  switch (prop_id)
    {
    case PROP_ARROW_TYPE:
      g_value_set_enum (value, arrow->arrow_type);
      break;
    case PROP_SHADOW_TYPE:
      g_value_set_enum (value, arrow->shadow_type);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_arrow_init (GtkArrow *arrow) {

  DebOut("gtk_arrow_init(%lx)\n",arrow);

  GTK_WIDGET_SET_FLAGS (arrow, GTK_NO_WINDOW);

#if 0
  GTK_WIDGET (arrow)->requisition.width = MIN_ARROW_SIZE + GTK_MISC (arrow)->xpad * 2;
  GTK_WIDGET (arrow)->requisition.height = MIN_ARROW_SIZE + GTK_MISC (arrow)->ypad * 2;
#endif

  arrow->arrow_type = GTK_ARROW_RIGHT;
  arrow->shadow_type = GTK_SHADOW_OUT;

}

GtkWidget* gtk_arrow_new (GtkArrowType  arrow_type, GtkShadowType shadow_type) {

  GtkArrow *arrow;
  ULONG spec;

  DebOut("gtk_arrow_new(%d,%d)\n",arrow_type,shadow_type);

  arrow = g_object_new (GTK_TYPE_ARROW, NULL);

  if(shadow_type!=GTK_SHADOW_OUT) {
    WarnOut("gtk_arrow_new: only GTK_SHADOW_OUT supported (type: %d)\n",shadow_type);
  }

  arrow->arrow_type = arrow_type;
  arrow->shadow_type = shadow_type;

  switch (arrow_type) {
    case GTK_ARROW_UP   : spec = MUII_ArrowUp; break;
    case GTK_ARROW_DOWN : spec = MUII_ArrowDown; break;
    case GTK_ARROW_LEFT : spec = MUII_ArrowLeft; break;
    case GTK_ARROW_RIGHT: spec = MUII_ArrowRight; break;
    default :
      ErrOut("gtk_arrow_new: unknown type: %d\n",arrow_type);
      return(NULL);
      break;
  }

  GtkSetObj(GTK_WIDGET(arrow),ImageObject, ImageButtonFrame,
                              MUIA_InputMode  , MUIV_InputMode_RelVerify,
                              MUIA_Image_Spec , spec,
#ifdef __MORPHOS__
                              MUIA_Background , MUII_ImageButtonBack,
#else
                              MUIA_Background , MUII_ButtonBack,
#endif
                              MUIA_CycleChain , 1,
                              MA_Widget, arrow,
                            End);

  return GTK_WIDGET (arrow);
}

void
gtk_arrow_set (GtkArrow      *arrow,
	       GtkArrowType   arrow_type,
	       GtkShadowType  shadow_type)
{
  g_return_if_fail (GTK_IS_ARROW (arrow));

  DebOut("gtk_arrow_set(%lx,%d,%d)\n",arrow,arrow_type,shadow_type);

  if(shadow_type!=GTK_SHADOW_OUT) {
    WarnOut("gtk_arrow_set: only GTK_SHADOW_OUT supported (type: %d)\n",shadow_type);
  }

#if 0
  if (   ((GtkArrowType) arrow->arrow_type != arrow_type)
      || ((GtkShadowType) arrow->shadow_type != shadow_type))
    {
      g_object_freeze_notify (G_OBJECT (arrow));

      if ((GtkArrowType) arrow->arrow_type != arrow_type)
        {
          arrow->arrow_type = arrow_type;
          g_object_notify (G_OBJECT (arrow), "arrow-type");
        }

      if ((GtkShadowType) arrow->shadow_type != shadow_type)
        {
          arrow->shadow_type = shadow_type;
          g_object_notify (G_OBJECT (arrow), "shadow-type");
        }

      g_object_thaw_notify (G_OBJECT (arrow));

      if (GTK_WIDGET_DRAWABLE (arrow))
	gtk_widget_queue_draw (GTK_WIDGET (arrow));
    }
#endif
}


#if 0
static gboolean 
gtk_arrow_expose (GtkWidget      *widget,
		  GdkEventExpose *event)
{
  GtkArrow *arrow;
  GtkMisc *misc;
  GtkShadowType shadow_type;
  gint width, height;
  gint x, y;
  gint extent;
  gfloat xalign;
  GtkArrowType effective_arrow_type;

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      arrow = GTK_ARROW (widget);
      misc = GTK_MISC (widget);

      width = widget->allocation.width - misc->xpad * 2;
      height = widget->allocation.height - misc->ypad * 2;
      extent = MIN (width, height) * 0.7;
      effective_arrow_type = arrow->arrow_type;

      if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
	xalign = misc->xalign;
      else
	{
	  xalign = 1.0 - misc->xalign;
	  if (arrow->arrow_type == GTK_ARROW_LEFT)
	    effective_arrow_type = GTK_ARROW_RIGHT;
	  else if (arrow->arrow_type == GTK_ARROW_RIGHT)
	    effective_arrow_type = GTK_ARROW_LEFT;
	}

      x = floor (widget->allocation.x + misc->xpad
		 + ((widget->allocation.width - extent) * xalign)
		 + 0.5);
      y = floor (widget->allocation.y + misc->ypad 
		 + ((widget->allocation.height - extent) * misc->yalign)
		 + 0.5);
      
      shadow_type = arrow->shadow_type;

      if (widget->state == GTK_STATE_ACTIVE)
	{
          if (shadow_type == GTK_SHADOW_IN)
            shadow_type = GTK_SHADOW_OUT;
          else if (shadow_type == GTK_SHADOW_OUT)
            shadow_type = GTK_SHADOW_IN;
          else if (shadow_type == GTK_SHADOW_ETCHED_IN)
            shadow_type = GTK_SHADOW_ETCHED_OUT;
          else if (shadow_type == GTK_SHADOW_ETCHED_OUT)
            shadow_type = GTK_SHADOW_ETCHED_IN;
	}

      gtk_paint_arrow (widget->style, widget->window,
		       widget->state, shadow_type,
		       &event->area, widget, "arrow",
		       effective_arrow_type, TRUE,
		       x, y, extent, extent);
    }

  return FALSE;
}
#endif

#define __GTK_ARROW_C__
