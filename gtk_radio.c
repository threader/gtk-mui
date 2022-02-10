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
 * $Id: gtk_radio.c,v 1.12 2007/03/19 09:00:09 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkradiobutton.h>

#include "debug.h"
#include "gtk_globals.h"
#include "classes/classes.h"

/*****************************************************************************
 * The MUI Radio stuff is quite limited. I tried to use it, it
 * was *more than complicated* to update the lists dynamically
 * and adding hooks to the single radio buttons was not even
 * done. 
 *
 * After rewriting this thing for three times, I choose
 * to implement radio buttons on my own.. much better ;).
 *
 * After seeing, how ugly the hook code gets over the time,
 * I made a MUI custom class for radio widgets.
 *****************************************************************************/


enum {
  PROP_0,
  PROP_GROUP
};

static void     gtk_radio_button_class_init     (GtkRadioButtonClass *klass);
static void     gtk_radio_button_init           (GtkRadioButton      *radio_button);
#if 0
static void     gtk_radio_button_destroy        (GtkObject           *object);
static gboolean gtk_radio_button_focus          (GtkWidget           *widget,
						 GtkDirectionType     direction);
static void     gtk_radio_button_clicked        (GtkButton           *button);
static void     gtk_radio_button_draw_indicator (GtkCheckButton      *check_button,
						 GdkRectangle        *area);
static void     gtk_radio_button_set_property   (GObject             *object,
						 guint                prop_id,
						 const GValue        *value,
						 GParamSpec          *pspec);
static void     gtk_radio_button_get_property   (GObject             *object,
						 guint                prop_id,
						 GValue              *value,
						 GParamSpec          *pspec);
#endif

static GtkCheckButtonClass *parent_class = NULL;

static guint group_changed_signal = 0;

GType gtk_radio_button_get_type (void) {

  static GType radio_button_type = 0;
  DebOut("gtk_radio_button_get_type()\n");

  if (!radio_button_type) {
    static const GTypeInfo radio_button_info = {
        sizeof (GtkRadioButtonClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
        (GClassInitFunc) gtk_radio_button_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_data */
        sizeof (GtkRadioButton),
        0,		/* n_preallocs */
        (GInstanceInitFunc) gtk_radio_button_init,
      };

    radio_button_type = g_type_register_static (GTK_TYPE_CHECK_BUTTON, "GtkRadioButton", &radio_button_info, 0);
  }
  return radio_button_type;
}

static void gtk_radio_button_class_init (GtkRadioButtonClass *class) {

  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkButtonClass *button_class;
  GtkCheckButtonClass *check_button_class;
  GtkWidgetClass *widget_class;

  DebOut("gtk_radio_button_class_init(%lx)\n",class);

  gobject_class = G_OBJECT_CLASS (class);
  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  button_class = (GtkButtonClass*) class;
  check_button_class = (GtkCheckButtonClass*) class;

  parent_class = g_type_class_peek_parent (class);

#if 0
  gobject_class->set_property = gtk_radio_button_set_property;
  gobject_class->get_property = gtk_radio_button_get_property;

  g_object_class_install_property (gobject_class,
				   PROP_GROUP,
				   g_param_spec_object ("group",
							P_("Group"),
							P_("The radio button whose group this widget belongs to."),
							GTK_TYPE_RADIO_BUTTON,
							G_PARAM_WRITABLE));
  object_class->destroy = gtk_radio_button_destroy;

  widget_class->focus = gtk_radio_button_focus;

  button_class->clicked = gtk_radio_button_clicked;

  check_button_class->draw_indicator = gtk_radio_button_draw_indicator;

#endif
  class->group_changed = NULL;

  /**
   * GtkStyle::group-changed:
   * @style: the object which received the signal
   *
   * Emitted when the group of radio buttons that a radio button belongs
   * to changes. This is emitted when a radio button switches from
   * being alone to being part of a group of 2 or more buttons, or
   * vice-versa, and when a buttton is moved from one group of 2 or
   * more buttons to a different one, but not when the composition
   * of the group that a button belongs to changes.
   *
   * Since: 2.4
   */
  group_changed_signal = g_signal_new ("group-changed",
				       G_OBJECT_CLASS_TYPE (object_class),
				       G_SIGNAL_RUN_FIRST,
				       G_STRUCT_OFFSET (GtkRadioButtonClass, group_changed),
				       NULL, NULL,
				       _gtk_marshal_VOID__VOID,
				       G_TYPE_NONE, 0);
}

static void gtk_radio_button_init (GtkRadioButton *radio_button) {

  DebOut("gtk_radio_button_init(%lx)\n",radio_button);

#if 0
  GTK_WIDGET_SET_FLAGS (radio_button, GTK_NO_WINDOW);
  GTK_WIDGET_UNSET_FLAGS (radio_button, GTK_RECEIVES_DEFAULT);

  GTK_TOGGLE_BUTTON (radio_button)->active = TRUE;
#endif

  /* active is set to TRUE in classes/radio.c -> mNew */
  GTK_TOGGLE_BUTTON (radio_button)->active = FALSE;

  GTK_BUTTON (radio_button)->depress_on_activate = FALSE;

  radio_button->group = g_slist_prepend (NULL, radio_button);

  GTK_MUI(radio_button)->mainclass=CL_AREA;

#if 0
  _gtk_button_set_depressed (GTK_BUTTON (radio_button), TRUE);
  gtk_widget_set_state (GTK_WIDGET (radio_button), GTK_STATE_ACTIVE);
#endif
}


void mgtk_update_group(GtkWidget *widget, GSList *group) {

  DebOut("mgtk_update_group(%lx,%lx)\n",widget,group);

  GTK_RADIO_BUTTON(widget)->group=(APTR) group;
}

GtkWidget *gtk_radio_button_new_with_label( GSList *group, const gchar  *label ) {
  GtkWidget *ret;
  Object *radio;

  DebOut("gtk_radio_button_new_with_label(%lx,%s)\n",group,label);

  ret=g_object_new (GTK_TYPE_RADIO_BUTTON, "label", label, NULL) ;

  if(!ret) {
    ErrOut("Unable to create new GTK_TYPE_RADIO_BUTTON object\n");
    return NULL;
  }
  GTK_BUTTON(ret)->label_text=g_strdup(label);


  radio=NewObject(CL_RadioButton->mcc_Class, NULL,MA_Widget,(ULONG) ret,MA_Radio_Label,(ULONG) label,TAG_DONE);
      
  GtkSetObj(GTK_WIDGET(ret), radio);

  if(group) {
    DebOut("  add another element to group %lx\n",group);
    DebOut("  group length: %ld\n",g_slist_length(group));
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (ret), group);
  }
  else {
    DebOut("  first element in this radio group.\n");
  }

  group=g_slist_append(group,ret);
  mgtk_update_group(ret,group);

  /* This is normaly done in gtk_radio_button_init, but we have no
   * MuiObject at this time. So better do it here.
   */
  gtk_widget_set_state (GTK_WIDGET (ret), GTK_STATE_ACTIVE);

  return ret;
}

void gtk_radio_button_set_group (GtkRadioButton *radio_button, GSList *group) {
  GtkWidget *old_group_singleton = NULL;
  GtkWidget *new_group_singleton = NULL;

  DebOut("gtk_radio_button_set_group(%lx,%lx)\n",radio_button,group);
  
  g_return_if_fail (GTK_IS_RADIO_BUTTON (radio_button));
  g_return_if_fail (!g_slist_find (group, radio_button));

  if (radio_button->group) {
    GSList *slist;

    radio_button->group = g_slist_remove (radio_button->group, radio_button);

    if (radio_button->group && !radio_button->group->next) {
      old_group_singleton = g_object_ref (radio_button->group->data);
    }

    for (slist = radio_button->group; slist; slist = slist->next) {
      GtkRadioButton *tmp_button;
	  
      tmp_button = slist->data;
	  
      tmp_button->group = radio_button->group;
    }
  }
  
  if (group && !group->next) {
    new_group_singleton = g_object_ref (group->data);
  }
  
  radio_button->group = g_slist_prepend (group, radio_button);
  
  if (group) {
    GSList *slist;
      
    for (slist = group; slist; slist = slist->next) {
      GtkRadioButton *tmp_button;
	  
      tmp_button = slist->data;
	  
      tmp_button->group = radio_button->group;
    }
  }

  g_object_ref (radio_button);
  
  g_signal_emit (radio_button, group_changed_signal, 0);
  if (old_group_singleton) {
    g_signal_emit (old_group_singleton, group_changed_signal, 0);
    g_object_unref (old_group_singleton);
  }
  if (new_group_singleton) {
    g_signal_emit (new_group_singleton, group_changed_signal, 0);
    g_object_unref (new_group_singleton);
  }

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio_button), group == NULL);

  g_object_unref (radio_button);
}

GSList *gtk_radio_button_get_group(GtkRadioButton *widget) {

  DebOut("gtk_radio_button_get_group(%lx): %lx\n",widget,GTK_RADIO_BUTTON(widget)->group);

  return(GTK_RADIO_BUTTON(widget)->group);
}

GtkWidget* gtk_radio_button_new_with_label_from_widget( GtkRadioButton *group, const gchar    *label ) {

  DebOut("gtk_radio_button_new_with_label_from_widget(%lx,%s)\n",group,label);

  return(gtk_radio_button_new_with_label(GTK_RADIO_BUTTON(group)->group,label));
}

