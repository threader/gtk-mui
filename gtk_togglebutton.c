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
 * $Id: gtk_togglebutton.c,v 1.6 2008/10/29 14:33:11 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

/* cross platform hooks */
#ifdef __AROS__
#include "include/SDI_hook.h"
#else
#include <SDI_hook.h>
#endif

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

#if NEED_GLIB
GtkToggleButton *GTK_TOGGLE_BUTTON(GtkWidget *widget) {

  return (GtkToggleButton *) widget;
}
#endif

#define DEFAULT_LEFT_POS  4
#define DEFAULT_TOP_POS   4
#define DEFAULT_SPACING   7

enum {
  TOGGLED,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_ACTIVE,
  PROP_INCONSISTENT,
  PROP_DRAW_INDICATOR
};


static void gtk_toggle_button_class_init    (GtkToggleButtonClass *klass);
static void gtk_toggle_button_init          (GtkToggleButton      *toggle_button);
#if 0
static gint gtk_toggle_button_expose        (GtkWidget            *widget,
					     GdkEventExpose       *event);
static gboolean gtk_toggle_button_mnemonic_activate  (GtkWidget            *widget,
                                                      gboolean              group_cycling);
static void gtk_toggle_button_pressed       (GtkButton            *button);
static void gtk_toggle_button_released      (GtkButton            *button);
static void gtk_toggle_button_clicked       (GtkButton            *button);
static void gtk_toggle_button_set_property  (GObject              *object,
					     guint                 prop_id,
					     const GValue         *value,
					     GParamSpec           *pspec);
static void gtk_toggle_button_get_property  (GObject              *object,
					     guint                 prop_id,
					     GValue               *value,
					     GParamSpec           *pspec);
static void gtk_toggle_button_update_state  (GtkButton            *button);
#endif

static guint toggle_button_signals[LAST_SIGNAL] = { 0 };
static GtkContainerClass *parent_class = NULL;

GType gtk_toggle_button_get_type (void) {
  static GType toggle_button_type = 0;

  if (!toggle_button_type) {
    static const GTypeInfo toggle_button_info = {
      sizeof (GtkToggleButtonClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_toggle_button_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkToggleButton),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_toggle_button_init,
    };

    toggle_button_type = g_type_register_static (GTK_TYPE_BUTTON, "GtkToggleButton",
      &toggle_button_info, 0);
  }

  return toggle_button_type;
}

static void gtk_toggle_button_class_init (GtkToggleButtonClass *class) {
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;
  GtkButtonClass *button_class;

  gobject_class = G_OBJECT_CLASS (class);
  widget_class = (GtkWidgetClass*) class;
  container_class = (GtkContainerClass*) class;
  button_class = (GtkButtonClass*) class;

  parent_class = g_type_class_peek_parent (class);

#if 0
  gobject_class->set_property = gtk_toggle_button_set_property;
  gobject_class->get_property = gtk_toggle_button_get_property;

  widget_class->expose_event = gtk_toggle_button_expose;
  widget_class->mnemonic_activate = gtk_toggle_button_mnemonic_activate;

  button_class->pressed = gtk_toggle_button_pressed;
  button_class->released = gtk_toggle_button_released;
  button_class->clicked = gtk_toggle_button_clicked;
  button_class->enter = gtk_toggle_button_update_state;
  button_class->leave = gtk_toggle_button_update_state;
#endif

  class->toggled = NULL;

#if 0
  g_object_class_install_property (gobject_class,
                                   PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
							 P_("Active"),
							 P_("If the toggle button should be pressed in or not"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_INCONSISTENT,
                                   g_param_spec_boolean ("inconsistent",
							 P_("Inconsistent"),
							 P_("If the toggle button is in an \"in between\" state"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_DRAW_INDICATOR,
                                   g_param_spec_boolean ("draw_indicator",
							 P_("Draw Indicator"),
							 P_("If the toggle part of the button is displayed"),
							 FALSE,
							 G_PARAM_READWRITE));
#endif

  toggle_button_signals[TOGGLED] =
    g_signal_new ("toggled",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkToggleButtonClass, toggled),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
}

static void gtk_toggle_button_init (GtkToggleButton *toggle_button) {

  toggle_button->active = FALSE;
  toggle_button->draw_indicator = FALSE;
  GTK_BUTTON (toggle_button)->depress_on_activate = TRUE;

  GtkSetObj(GTK_WIDGET(toggle_button), NewObject(CL_ToggleButton->mcc_Class, NULL,
                                                  MA_Widget,(ULONG) toggle_button,
                                                  TAG_DONE
                                                ));
  GTK_MUI (toggle_button)->mainclass = CL_AREA;

  nnset(GtkObj(toggle_button),MA_Toggle_Value,toggle_button->active);
}

/* to be tested.. */
GtkWidget *gtk_toggle_button_new(void) {
  GtkWidget *button;

  DebOut("gtk_toggle_button_new()\n");

  button=g_object_new (GTK_TYPE_TOGGLE_BUTTON, NULL);

  return button;
}

void gtk_toggle_button_set_active( GtkToggleButton *toggle_button, gboolean is_active) {

  DebOut("gtk_toggle_button_set_active(%lx,%ld)\n",toggle_button,is_active);

  if(GTK_TOGGLE_BUTTON(toggle_button)->active != is_active) {
    DebOut("  mui: %lx\n",GtkObj(toggle_button));
    GTK_TOGGLE_BUTTON(toggle_button)->active=is_active;
    set(GtkObj(toggle_button),MA_Toggle_Value,(ULONG) is_active);
  }
  else {
    DebOut("  GTK_TOGGLE_BUTTON(toggle_button)->active: %d\n",GTK_TOGGLE_BUTTON(toggle_button)->active);
    DebOut("  ==> nothing to do\n");
  }

  return;
}

gboolean gtk_toggle_button_get_active(GtkToggleButton *toggle_button) {

  DebOut("gtk_toggle_button_get_active(%lx)\n",toggle_button);

  return xget(GtkObj(toggle_button),MA_Toggle_Value);
}

/* to be tested.. */
GtkWidget *gtk_toggle_button_new_with_label(const gchar *label) {
  GtkWidget *tb;
  GtkWidget *l;

  DebOut("gtk_toggle_button_new_with_label(%s)\n",label);

  tb=gtk_toggle_button_new();
  l=gtk_label_new(label);
  gtk_container_add (GTK_CONTAINER (tb), l);

  return tb;
}
