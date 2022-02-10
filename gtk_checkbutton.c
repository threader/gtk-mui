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
 * $Id: gtk_checkbutton.c,v 1.20 2007/03/19 09:00:06 o1i Exp $
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
#include "gtk/gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

#if NEED_GLIB
GtkToggleButton *GTK_TOGGLE_BUTTON(GtkWidget *widget) {

  return (GtkToggleButton *) widget;
}
#endif

static GtkToggleButtonClass *parent_class = NULL;

static void gtk_check_button_class_init (GtkCheckButtonClass *class) {
  GtkWidgetClass *widget_class;
  
  widget_class = (GtkWidgetClass*) class;
  parent_class = g_type_class_peek_parent (class);
#if 0 
  widget_class->size_request = gtk_check_button_size_request;
  widget_class->size_allocate = gtk_check_button_size_allocate;
  widget_class->expose_event = gtk_check_button_expose;

  class->draw_indicator = gtk_real_check_button_draw_indicator;

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_int ("indicator_size",
							     P_("Indicator Size"),
							     P_("Size of check or radio indicator"),
							     0,
							     G_MAXINT,
							     INDICATOR_SIZE,
							     G_PARAM_READABLE));
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_int ("indicator_spacing",
							     P_("Indicator Spacing"),
							     P_("Spacing around check or radio indicator"),
							     0,
							     G_MAXINT,
							     INDICATOR_SPACING,
							     G_PARAM_READABLE));
#endif
}

static void
gtk_check_button_init (GtkCheckButton *check_button)
{
#if 0
  GTK_WIDGET_SET_FLAGS (check_button, GTK_NO_WINDOW);
  GTK_WIDGET_UNSET_FLAGS (check_button, GTK_RECEIVES_DEFAULT);
#endif
  GTK_TOGGLE_BUTTON (check_button)->draw_indicator = TRUE;
  GTK_BUTTON (check_button)->depress_on_activate = FALSE;
  GTK_MUI(check_button)->mainclass=CL_AREA;
}

GType gtk_check_button_get_type (void) {

  static GType check_button_type = 0;
  
  if (!check_button_type) {
    static const GTypeInfo check_button_info = {
      sizeof (GtkCheckButtonClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_check_button_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkCheckButton),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_check_button_init,
    };
      
    check_button_type = g_type_register_static (GTK_TYPE_TOGGLE_BUTTON, "GtkCheckButton",
				&check_button_info, 0);
  }
  
  return check_button_type;
}

GtkWidget *gtk_check_button_new_with_label (const gchar *label) {
  GtkWidget *ret = NULL;
  Object *check;

  DebOut("gtk_check_button_new_with_label(%s)\n",label);

  ret=g_object_new (GTK_TYPE_CHECK_BUTTON, "label", label, NULL);

  check = NewObject(CL_CheckButton->mcc_Class, NULL,MA_Widget,(ULONG) ret,MA_Check_Label,(ULONG) label,TAG_DONE);

  if((!ret) || (!check)) {
    ErrOut("Unable to create gtk_check_button_with_label!\n");
    return NULL;
  }

  DebOut("  check_button: %lx\n",check);

  GtkSetObj(ret,check);

  return ret;
}

