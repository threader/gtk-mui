/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
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
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: gtk_frame.c,v 1.12 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "gtk.h"
#include "gtk/gtkframe.h"

#include "debug.h"
#include "gtk_globals.h"
#include "classes/classes.h"

enum {
  PROP_0,
  PROP_LABEL,
  PROP_LABEL_XALIGN,
  PROP_LABEL_YALIGN,
  PROP_SHADOW,
  PROP_SHADOW_TYPE,
  PROP_LABEL_WIDGET
};

static GtkBinClass *parent_class = NULL;

static void gtk_frame_class_init    (GtkFrameClass  *klass);
static void gtk_frame_init          (GtkFrame       *frame);
static void gtk_frame_init          (GtkFrame       *frame);
static void gtk_frame_set_property (GObject      *object,
				    guint         param_id,
				    const GValue *value,
				    GParamSpec   *pspec);
static void gtk_frame_get_property (GObject     *object,
				    guint        param_id,
				    GValue      *value,
				    GParamSpec  *pspec);

static void gtk_frame_forall        (GtkContainer   *container,
				     gboolean	     include_internals,
			             GtkCallback     callback,
			             gpointer        callback_data);

GType gtk_frame_get_type (void) {

  static GType frame_type = 0;

  if (!frame_type) {
    static const GTypeInfo frame_info = {
      sizeof (GtkFrameClass),
      NULL,    /* base_init */
      NULL,    /* base_finalize */
      (GClassInitFunc) gtk_frame_class_init,
      NULL,    /* class_finalize */
      NULL,    /* class_data */
      sizeof (GtkFrame),
      0,    /* n_preallocs */
      (GInstanceInitFunc) gtk_frame_init,
    };

    frame_type = g_type_register_static (GTK_TYPE_BIN, "GtkFrame",
             &frame_info, 0);
  }

  return frame_type;
}

static void gtk_frame_class_init (GtkFrameClass *class) {

  //DebOut("gtk_frame_class_init(%lx)\n",class);

  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = (GObjectClass*) class;
  widget_class = GTK_WIDGET_CLASS (class);
  container_class = GTK_CONTAINER_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  gobject_class->set_property = gtk_frame_set_property;
  gobject_class->get_property = gtk_frame_get_property;

  g_object_class_install_property (gobject_class,
          PROP_LABEL,
          g_param_spec_string ("label",
          P_("Label"),
          P_("Text of the frame's label"),
          NULL,
          G_PARAM_READABLE |
          G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
          PROP_LABEL_XALIGN,
          g_param_spec_float ("label_xalign",
          P_("Label xalign"),
          P_("The horizontal alignment of the label"),
          0.0,
          1.0,
          0.5,
          G_PARAM_READABLE |
          G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
          PROP_LABEL_YALIGN,
          g_param_spec_float ("label_yalign",
          P_("Label yalign"),
          P_("The vertical alignment of the label"),
          0.0,
          1.0,
          0.5,
          G_PARAM_READABLE |
          G_PARAM_WRITABLE));

#ifdef WEHAVE_SHADOWTYPE
  g_object_class_install_property (gobject_class,
          PROP_SHADOW_TYPE,
          g_param_spec_enum ("shadow_type",
          P_("Frame shadow"),
          P_("Appearance of the frame border"),
          GTK_TYPE_SHADOW_TYPE,
          GTK_SHADOW_ETCHED_IN,
          G_PARAM_READABLE | G_PARAM_WRITABLE));
#endif

  g_object_class_install_property (gobject_class,
          PROP_LABEL_WIDGET,
          g_param_spec_object ("label_widget",
          P_("Label widget"),
          P_("A widget to display in place of the usual frame label"),
          GTK_TYPE_WIDGET,
          G_PARAM_READABLE | G_PARAM_WRITABLE));
  
#if 0
  widget_class->expose_event = gtk_frame_expose;
  widget_class->size_request = gtk_frame_size_request;
  widget_class->size_allocate = gtk_frame_size_allocate;

  container_class->remove = gtk_frame_remove;
#endif
  container_class->forall = gtk_frame_forall;

#if 0
  class->compute_child_allocation = gtk_frame_real_compute_child_allocation;
#endif
}

static void gtk_frame_init (GtkFrame *frame) {

  DebOut("gtk_frame_init(%lx)\n",frame);

  frame->label_widget = NULL;
  frame->shadow_type = GTK_SHADOW_ETCHED_IN;
  frame->label_xalign = 0.0;
  frame->label_yalign = 0.5;
#if MGTK_DEBUG
  frame->title = g_strdup("-empty title-");
#else
  frame->title = g_strdup("");
#endif

  GtkSetObj(GTK_WIDGET(frame),NewObject(CL_Frame->mcc_Class,0L,TAG_DONE));
  set(GtkObj(frame),MA_Widget, (ULONG) frame);
  GTK_MUI(frame)->mainclass=CL_AREA;

}

static void gtk_frame_set_property (GObject         *object,
      guint            prop_id,
      const GValue    *value,
      GParamSpec      *pspec) {

  GtkFrame *frame;

  DebOut("gtk_frame_set_property(%lx,%d,..)\n",object,prop_id);

  frame = GTK_FRAME (object);

  switch (prop_id)
    {
    case PROP_LABEL:
      gtk_frame_set_label (frame, g_value_get_string (value));
      break;
    default:      
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_frame_get_property (GObject         *object,
			guint            prop_id,
			GValue          *value,
			GParamSpec      *pspec) {

  GtkFrame *frame;

  DebOut("gtk_frame_get_property(%lx,%d,..)\n",object,prop_id);

  frame = GTK_FRAME (object);

  switch (prop_id)
    {
    case PROP_LABEL:
      g_value_set_string (value, gtk_frame_get_label (frame));
      break;
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/**
 * gtk_frame_get_label:
 * @frame: a #GtkFrame
 * 
 * Return value: the text in the label, or %NULL if there
 *               was no label. This string is owned by GTK+ and
 *               must not be modified or freed.
 **/
G_CONST_RETURN gchar * gtk_frame_get_label (GtkFrame *frame) {

  g_return_val_if_fail (GTK_IS_FRAME (frame), NULL);

  return frame->title;
}

/**
 * gtk_frame_set_label:
 * @frame: a #GtkFrame
 * @label: the text to use as the label of the frame
 * 
 * Sets the text of the label. If @label is %NULL,
 * the current label is removed.
 **/

void gtk_frame_set_label (GtkFrame *frame, const gchar *label) {

  DebOut("gtk_frame_set_label(%lx,%s)\n",frame,label);

  g_return_if_fail (GTK_IS_FRAME (frame));

  if(frame->title) {
    g_free(frame->title);
  }
  frame->title=g_strdup(label);

  if(!GtkObj(frame)) {
    /* during init we don't need to modify mui stuff */
    return;
  }

  set(GtkObj(frame),MA_GtkFrame_Label,label);

}

/**
 * gtk_frame_set_label_widget:
 * @frame: a #GtkFrame
 * @label_widget: the new label widget
 * 
 * Sets the label widget for the frame. This is the widget that
 * will appear embedded in the top edge of the frame as a
 * title.
 **/
void gtk_frame_set_label_widget (GtkFrame  *frame, GtkWidget *label_widget) {

#warning TODO: gtk_frame_set_label_widget

  /* up to now, we don't have real label widgets
   */
  ErrOut("gtk_frame_set_label_widget(%lx,%lx) is TODO!",(ULONG) frame,(ULONG) label_widget);
#if 0
  gboolean need_resize = FALSE;
  
  g_return_if_fail (GTK_IS_FRAME (frame));
  g_return_if_fail (label_widget == NULL || GTK_IS_WIDGET (label_widget));
  g_return_if_fail (label_widget == NULL || label_widget->parent == NULL);
  
  if (frame->label_widget == label_widget)
    return;
  
  if (frame->label_widget)
    {
      need_resize = GTK_WIDGET_VISIBLE (frame->label_widget);
      gtk_widget_unparent (frame->label_widget);
    }

  frame->label_widget = label_widget;
    
  if (label_widget)
    {
      frame->label_widget = label_widget;
      gtk_widget_set_parent (label_widget, GTK_WIDGET (frame));
      need_resize |= GTK_WIDGET_VISIBLE (label_widget);
    }
  
  if (GTK_WIDGET_VISIBLE (frame) && need_resize)
    gtk_widget_queue_resize (GTK_WIDGET (frame));

  g_object_freeze_notify (G_OBJECT (frame));
  g_object_notify (G_OBJECT (frame), "label-widget");
  g_object_notify (G_OBJECT (frame), "label");
  g_object_thaw_notify (G_OBJECT (frame));
#endif
}

/**
 * gtk_frame_new:
 * @label: the text to use as the label of the frame
 * 
 * Creates a new #GtkFrame, with optional label @label.
 * If @label is %NULL, the label is omitted.
 * 
 * Return value: a new #GtkFrame widget
 **/
GtkWidget* gtk_frame_new(const gchar *label) {

  GtkWidget *ret;

  DebOut("gtk_frame_new(%s)\n",label);

  ret=g_object_new (GTK_TYPE_FRAME, "label", label, NULL);

  if(label) {
    g_free(GTK_FRAME(ret)->title);
    GTK_FRAME(ret)->title=g_strdup(label);

    set(GtkObj(ret),MA_GtkFrame_Label,GTK_FRAME(ret)->title);
  }

  return ret;
}

#if 0
static void
gtk_frame_remove (GtkContainer *container,
		  GtkWidget    *child)
{
  GtkFrame *frame = GTK_FRAME (container);

  if (frame->label_widget == child)
    gtk_frame_set_label_widget (frame, NULL);
  else
    GTK_CONTAINER_CLASS (gtk_frame_parent_class)->remove (container, child);
}
#endif

static void
gtk_frame_forall (GtkContainer *container,
		  gboolean      include_internals,
		  GtkCallback   callback,
		  gpointer      callback_data)
{
  GtkBin *bin = GTK_BIN (container);
  GtkFrame *frame = GTK_FRAME (container);

  if (bin->child)
    (* callback) (bin->child, callback_data);

  if (frame->label_widget)
    (* callback) (frame->label_widget, callback_data);

}


void gtk_frame_set_shadow_type(GtkFrame *frame, GtkShadowType type) {

  DebOut("gtk_frame_set_shadow_type(%lx,%d)\n",frame,type);

  if(type == GTK_SHADOW_NONE) {
    DebOut("TODO: type=GTK_SHADOW_NONE\n");
  }

  /* just ignore it.. */
}

void gtk_frame_set_label_align(GtkFrame *frame, gfloat xalign, gfloat yalign) {

  DebOut("gtk_frame_set_label_align(%lx,..)\n",frame);

  DebOut("  gtk_frame_set_label_align is TODO!\n");


  /* just ignore it.. */
}

