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
 * $Id: gtk_entry.c,v 1.20 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>
#undef ACTIVATE
/* cross platform hooks */
#ifdef __AROS__
#include "include/SDI_hook.h"
#else
#include <SDI_hook.h>
#endif

#include <gtk.h>
#include "gtk/gtk.h"
#include <gtk/gtkentry.h>
#include <gtk/gtkentryprivate.h>
#include <gtk/gtkmarshalers.h>

#include "debug.h"
#include "gtk_globals.h"
#include "classes/classes.h"

/* Initial size of buffer, in bytes */
#define MIN_SIZE 16

/* Maximum size of text buffer, in bytes */
#define MAX_SIZE G_MAXUSHORT

typedef struct _GtkEntryPrivate GtkEntryPrivate;

#define GTK_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_ENTRY, GtkEntryPrivate))

struct _GtkEntryPrivate 
{
  gfloat xalign;
  gint insert_pos;
};

enum {
  ACTIVATE,
  POPULATE_POPUP,
  MOVE_CURSOR,
  INSERT_AT_CURSOR,
  DELETE_FROM_CURSOR,
  BACKSPACE,
  CUT_CLIPBOARD,
  COPY_CLIPBOARD,
  PASTE_CLIPBOARD,
  TOGGLE_OVERWRITE,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_CURSOR_POSITION,
  PROP_SELECTION_BOUND,
  PROP_EDITABLE,
  PROP_MAX_LENGTH,
  PROP_VISIBILITY,
  PROP_HAS_FRAME,
  PROP_INVISIBLE_CHAR,
  PROP_ACTIVATES_DEFAULT,
  PROP_WIDTH_CHARS,
  PROP_SCROLL_OFFSET,
  PROP_TEXT,
  PROP_XALIGN
};

static void gtk_entry_insert_text (GtkEditable *editable,
		       const gchar *new_text,
		       gint         new_text_length,
		       gint        *position);
static void gtk_entry_set_selection_bounds (GtkEditable *editable,
				gint         start,
				gint         end);

gchar* gtk_entry_get_chars(GtkEditable *editable, gint start_pos, gint end_pos);


/* Default signal handlers
 */
static void gtk_entry_real_insert_text   (GtkEditable     *editable,
					  const gchar     *new_text,
					  gint             new_text_length,
					  gint            *position);


static guint signals[LAST_SIGNAL] = { 0 };
static GtkWidgetClass *parent_class = NULL;

/*****************************************************************************/

static void gtk_entry_set_property (GObject         *object,
                        guint            prop_id,
                        const GValue    *value,
                        GParamSpec      *pspec) {

  GtkEntry *entry = GTK_ENTRY (object);

  DebOut("gtk_entry_set_property(%lx,%d,..)\n",entry,prop_id);

  switch (prop_id)
    {
#if 0
    case PROP_EDITABLE:
      {
        gboolean new_value = g_value_get_boolean (value);

      	if (new_value != entry->editable)
	  {
	    if (!new_value)
	      {
		gtk_entry_reset_im_context (entry);
		if (GTK_WIDGET_HAS_FOCUS (entry))
		  gtk_im_context_focus_out (entry->im_context);

		entry->preedit_length = 0;
		entry->preedit_cursor = 0;
	      }

	    entry->editable = new_value;

	    if (new_value && GTK_WIDGET_HAS_FOCUS (entry))
	      gtk_im_context_focus_in (entry->im_context);
	    
	    gtk_entry_queue_draw (entry);
	  }
      }
      break;

    case PROP_MAX_LENGTH:
      gtk_entry_set_max_length (entry, g_value_get_int (value));
      break;
#endif
      
    case PROP_VISIBILITY:
      gtk_entry_set_visibility (entry, g_value_get_boolean (value));
      break;
#if 0
    case PROP_HAS_FRAME:
      gtk_entry_set_has_frame (entry, g_value_get_boolean (value));
      break;

    case PROP_INVISIBLE_CHAR:
      gtk_entry_set_invisible_char (entry, g_value_get_uint (value));
      break;

    case PROP_ACTIVATES_DEFAULT:
      gtk_entry_set_activates_default (entry, g_value_get_boolean (value));
      break;

    case PROP_WIDTH_CHARS:
      gtk_entry_set_width_chars (entry, g_value_get_int (value));
      break;

    case PROP_TEXT:
      gtk_entry_set_text (entry, g_value_get_string (value));
      break;

    case PROP_XALIGN:
      gtk_entry_set_alignment (entry, g_value_get_float (value));
      break;

    case PROP_SCROLL_OFFSET:
    case PROP_CURSOR_POSITION:
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_entry_get_property (GObject         *object,
                        guint            prop_id,
                        GValue          *value,
                        GParamSpec      *pspec)
{
  GtkEntry *entry = GTK_ENTRY (object);

  DebOut("gtk_entry_get_property(%lx,%d,..)\n",entry,prop_id);

  switch (prop_id)
    {
#if 0
    case PROP_CURSOR_POSITION:
      g_value_set_int (value, entry->current_pos);
      break;
    case PROP_SELECTION_BOUND:
      g_value_set_int (value, entry->selection_bound);
      break;
    case PROP_EDITABLE:
      g_value_set_boolean (value, entry->editable);
      break;
    case PROP_MAX_LENGTH:
      g_value_set_int (value, entry->text_max_length); 
      break;
    case PROP_VISIBILITY:
      g_value_set_boolean (value, entry->visible);
      break;
    case PROP_HAS_FRAME:
      g_value_set_boolean (value, entry->has_frame);
      break;
    case PROP_INVISIBLE_CHAR:
      g_value_set_uint (value, entry->invisible_char);
      break;
    case PROP_ACTIVATES_DEFAULT:
      g_value_set_boolean (value, entry->activates_default);
      break;
    case PROP_WIDTH_CHARS:
      g_value_set_int (value, entry->width_chars);
      break;
    case PROP_SCROLL_OFFSET:
      g_value_set_int (value, entry->scroll_offset);
      break;
    case PROP_TEXT:
      g_value_set_string (value, gtk_entry_get_text (entry));
      break;
    case PROP_XALIGN:
      g_value_set_float (value, gtk_entry_get_alignment (entry));
      break;
#endif  
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}
static void gtk_entry_class_init (GtkEntryClass *class) {

  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class;
  GtkObjectClass *gtk_object_class;
#if 0
  GtkBindingSet *binding_set;
#endif

  DebOut("gtk_entry_class_init(%lx)\n",class);

  widget_class = (GtkWidgetClass*) class;
  gtk_object_class = (GtkObjectClass *)class;
  parent_class = g_type_class_peek_parent (class);

#if 0
  gobject_class->finalize = gtk_entry_finalize;
#endif
  gobject_class->set_property = gtk_entry_set_property;
  gobject_class->get_property = gtk_entry_get_property;

#if 0
  widget_class->realize = gtk_entry_realize;
  widget_class->unrealize = gtk_entry_unrealize;
  widget_class->size_request = gtk_entry_size_request;
  widget_class->size_allocate = gtk_entry_size_allocate;
  widget_class->expose_event = gtk_entry_expose;
  widget_class->button_press_event = gtk_entry_button_press;
  widget_class->button_release_event = gtk_entry_button_release;
  widget_class->motion_notify_event = gtk_entry_motion_notify;
  widget_class->key_press_event = gtk_entry_key_press;
  widget_class->key_release_event = gtk_entry_key_release;
  widget_class->focus_in_event = gtk_entry_focus_in;
  widget_class->focus_out_event = gtk_entry_focus_out;
  widget_class->grab_focus = gtk_entry_grab_focus;
  widget_class->style_set = gtk_entry_style_set;
  widget_class->direction_changed = gtk_entry_direction_changed;
  widget_class->state_changed = gtk_entry_state_changed;
  widget_class->screen_changed = gtk_entry_screen_changed;
  widget_class->mnemonic_activate = gtk_entry_mnemonic_activate;

  widget_class->drag_drop = gtk_entry_drag_drop;
  widget_class->drag_motion = gtk_entry_drag_motion;
  widget_class->drag_leave = gtk_entry_drag_leave;
  widget_class->drag_data_received = gtk_entry_drag_data_received;
  widget_class->drag_data_get = gtk_entry_drag_data_get;
  widget_class->drag_data_delete = gtk_entry_drag_data_delete;

  widget_class->popup_menu = gtk_entry_popup_menu;

  gtk_object_class->destroy = gtk_entry_destroy;

  class->move_cursor = gtk_entry_move_cursor;
  class->insert_at_cursor = gtk_entry_insert_at_cursor;
  class->delete_from_cursor = gtk_entry_delete_from_cursor;
  class->backspace = gtk_entry_backspace;
  class->cut_clipboard = gtk_entry_cut_clipboard;
  class->copy_clipboard = gtk_entry_copy_clipboard;
  class->paste_clipboard = gtk_entry_paste_clipboard;
  class->toggle_overwrite = gtk_entry_toggle_overwrite;
  class->activate = gtk_entry_real_activate;
#endif
  
  g_object_class_install_property (gobject_class,
                                   PROP_CURSOR_POSITION,
                                   g_param_spec_int ("cursor_position",
                                                     P_("Cursor Position"),
                                                     P_("The current position of the insertion cursor in chars"),
                                                     0,
                                                     MAX_SIZE,
                                                     0,
                                                     G_PARAM_READABLE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTION_BOUND,
                                   g_param_spec_int ("selection_bound",
                                                     P_("Selection Bound"),
                                                     P_("The position of the opposite end of the selection from the cursor in chars"),
                                                     0,
                                                     MAX_SIZE,
                                                     0,
                                                     G_PARAM_READABLE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_EDITABLE,
                                   g_param_spec_boolean ("editable",
							 P_("Editable"),
							 P_("Whether the entry contents can be edited"),
                                                         TRUE,
							 G_PARAM_READABLE | G_PARAM_WRITABLE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_MAX_LENGTH,
                                   g_param_spec_int ("max_length",
                                                     P_("Maximum length"),
                                                     P_("Maximum number of characters for this entry. Zero if no maximum"),
                                                     0,
                                                     MAX_SIZE,
                                                     0,
                                                     G_PARAM_READABLE | G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_VISIBILITY,
                                   g_param_spec_boolean ("visibility",
							 P_("Visibility"),
							 P_("FALSE displays the \"invisible char\" instead of the actual text (password mode)"),
                                                         TRUE,
							 G_PARAM_READABLE | G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_HAS_FRAME,
                                   g_param_spec_boolean ("has_frame",
							 P_("Has Frame"),
							 P_("FALSE removes outside bevel from entry"),
                                                         TRUE,
							 G_PARAM_READABLE | G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_INVISIBLE_CHAR,
                                   g_param_spec_unichar ("invisible_char",
							 P_("Invisible character"),
							 P_("The character to use when masking entry contents (in \"password mode\")"),
							 '*',
							 G_PARAM_READABLE | G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_ACTIVATES_DEFAULT,
                                   g_param_spec_boolean ("activates_default",
							 P_("Activates default"),
							 P_("Whether to activate the default widget (such as the default button in a dialog) when Enter is pressed"),
                                                         FALSE,
							 G_PARAM_READABLE | G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_WIDTH_CHARS,
                                   g_param_spec_int ("width_chars",
                                                     P_("Width in chars"),
                                                     P_("Number of characters to leave space for in the entry"),
                                                     -1,
                                                     G_MAXINT,
                                                     -1,
                                                     G_PARAM_READABLE | G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_SCROLL_OFFSET,
                                   g_param_spec_int ("scroll_offset",
                                                     P_("Scroll offset"),
                                                     P_("Number of pixels of the entry scrolled off the screen to the left"),
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
							P_("Text"),
							P_("The contents of the entry"),
							"",
							G_PARAM_READABLE | G_PARAM_WRITABLE));

  /**
   * GtkEntry:xalign:
   *
   * The horizontal alignment, from 0 (left) to 1 (right). 
   * Reversed for RTL layouts.
   * 
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_XALIGN,
                                   g_param_spec_float ("xalign",
						       P_("X align"),
						       P_("The horizontal alignment, from 0 (left) to 1 (right). Reversed for RTL layouts."),
						       0.0,
						       1.0,
						       0.0,
						       G_PARAM_READABLE | G_PARAM_WRITABLE));
  
#if 0
  signals[POPULATE_POPUP] =
    g_signal_new ("populate_popup",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtkEntryClass, populate_popup),
		  NULL, NULL,
		  _gtk_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
		  GTK_TYPE_MENU);
#endif
  
 /* Action signals */
  
  signals[ACTIVATE] =
    g_signal_new ("activate",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, activate),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  widget_class->activate_signal = signals[ACTIVATE];

#if 0
  signals[MOVE_CURSOR] = 
    g_signal_new ("move_cursor",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, move_cursor),
		  NULL, NULL,
		  _gtk_marshal_VOID__ENUM_INT_BOOLEAN,
		  G_TYPE_NONE, 3,
		  GTK_TYPE_MOVEMENT_STEP,
		  G_TYPE_INT,
		  G_TYPE_BOOLEAN);
#endif

  signals[INSERT_AT_CURSOR] = 
    g_signal_new ("insert_at_cursor",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, insert_at_cursor),
		  NULL, NULL,
		  _gtk_marshal_VOID__STRING,
		  G_TYPE_NONE, 1,
		  G_TYPE_STRING);

#if 0
  signals[DELETE_FROM_CURSOR] = 
    g_signal_new ("delete_from_cursor",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, delete_from_cursor),
		  NULL, NULL,
		  _gtk_marshal_VOID__ENUM_INT,
		  G_TYPE_NONE, 2,
		  GTK_TYPE_DELETE_TYPE,
		  G_TYPE_INT);
#endif

  signals[BACKSPACE] =
    g_signal_new ("backspace",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, backspace),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  signals[CUT_CLIPBOARD] =
    g_signal_new ("cut_clipboard",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, cut_clipboard),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  signals[COPY_CLIPBOARD] =
    g_signal_new ("copy_clipboard",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, copy_clipboard),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  signals[PASTE_CLIPBOARD] =
    g_signal_new ("paste_clipboard",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, paste_clipboard),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  signals[TOGGLE_OVERWRITE] =
    g_signal_new ("toggle_overwrite",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkEntryClass, toggle_overwrite),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /*
   * Key bindings
   */

#if 0
  binding_set = gtk_binding_set_by_class (class);

  /* Moving the insertion point */
  add_move_binding (binding_set, GDK_Right, 0,
		    GTK_MOVEMENT_VISUAL_POSITIONS, 1);
  
  add_move_binding (binding_set, GDK_Left, 0,
		    GTK_MOVEMENT_VISUAL_POSITIONS, -1);

  add_move_binding (binding_set, GDK_KP_Right, 0,
		    GTK_MOVEMENT_VISUAL_POSITIONS, 1);
  
  add_move_binding (binding_set, GDK_KP_Left, 0,
		    GTK_MOVEMENT_VISUAL_POSITIONS, -1);
  
  add_move_binding (binding_set, GDK_Right, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, 1);

  add_move_binding (binding_set, GDK_Left, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, -1);

  add_move_binding (binding_set, GDK_KP_Right, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, 1);

  add_move_binding (binding_set, GDK_KP_Left, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, -1);
  
  add_move_binding (binding_set, GDK_Home, 0,
		    GTK_MOVEMENT_DISPLAY_LINE_ENDS, -1);

  add_move_binding (binding_set, GDK_End, 0,
		    GTK_MOVEMENT_DISPLAY_LINE_ENDS, 1);

  add_move_binding (binding_set, GDK_KP_Home, 0,
		    GTK_MOVEMENT_DISPLAY_LINE_ENDS, -1);

  add_move_binding (binding_set, GDK_KP_End, 0,
		    GTK_MOVEMENT_DISPLAY_LINE_ENDS, 1);
  
  add_move_binding (binding_set, GDK_Home, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_BUFFER_ENDS, -1);

  add_move_binding (binding_set, GDK_End, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_BUFFER_ENDS, 1);

  add_move_binding (binding_set, GDK_KP_Home, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_BUFFER_ENDS, -1);

  add_move_binding (binding_set, GDK_KP_End, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_BUFFER_ENDS, 1);

  /* Select all
   */
  gtk_binding_entry_add_signal (binding_set, GDK_a, GDK_CONTROL_MASK,
                                "move_cursor", 3,
                                GTK_TYPE_MOVEMENT_STEP, GTK_MOVEMENT_BUFFER_ENDS,
                                G_TYPE_INT, -1,
				G_TYPE_BOOLEAN, FALSE);
  gtk_binding_entry_add_signal (binding_set, GDK_a, GDK_CONTROL_MASK,
                                "move_cursor", 3,
                                GTK_TYPE_MOVEMENT_STEP, GTK_MOVEMENT_BUFFER_ENDS,
                                G_TYPE_INT, 1,
				G_TYPE_BOOLEAN, TRUE);


  /* Activate
   */
  gtk_binding_entry_add_signal (binding_set, GDK_Return, 0,
				"activate", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Enter, 0,
				"activate", 0);
  
  /* Deleting text */
  gtk_binding_entry_add_signal (binding_set, GDK_Delete, 0,
				"delete_from_cursor", 2,
				G_TYPE_ENUM, GTK_DELETE_CHARS,
				G_TYPE_INT, 1);

  gtk_binding_entry_add_signal (binding_set, GDK_KP_Delete, 0,
				"delete_from_cursor", 2,
				G_TYPE_ENUM, GTK_DELETE_CHARS,
				G_TYPE_INT, 1);
  
  gtk_binding_entry_add_signal (binding_set, GDK_BackSpace, 0,
				"backspace", 0);

  /* Make this do the same as Backspace, to help with mis-typing */
  gtk_binding_entry_add_signal (binding_set, GDK_BackSpace, GDK_SHIFT_MASK,
				"backspace", 0);

  gtk_binding_entry_add_signal (binding_set, GDK_Delete, GDK_CONTROL_MASK,
				"delete_from_cursor", 2,
				G_TYPE_ENUM, GTK_DELETE_WORD_ENDS,
				G_TYPE_INT, 1);

  gtk_binding_entry_add_signal (binding_set, GDK_KP_Delete, GDK_CONTROL_MASK,
				"delete_from_cursor", 2,
				G_TYPE_ENUM, GTK_DELETE_WORD_ENDS,
				G_TYPE_INT, 1);
  
  gtk_binding_entry_add_signal (binding_set, GDK_BackSpace, GDK_CONTROL_MASK,
				"delete_from_cursor", 2,
				G_TYPE_ENUM, GTK_DELETE_WORD_ENDS,
				G_TYPE_INT, -1);

  /* Cut/copy/paste */

  gtk_binding_entry_add_signal (binding_set, GDK_x, GDK_CONTROL_MASK,
				"cut_clipboard", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_c, GDK_CONTROL_MASK,
				"copy_clipboard", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_v, GDK_CONTROL_MASK,
				"paste_clipboard", 0);

  gtk_binding_entry_add_signal (binding_set, GDK_Delete, GDK_SHIFT_MASK,
				"cut_clipboard", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_Insert, GDK_CONTROL_MASK,
				"copy_clipboard", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_Insert, GDK_SHIFT_MASK,
				"paste_clipboard", 0);

  /* Overwrite */
  gtk_binding_entry_add_signal (binding_set, GDK_Insert, 0,
				"toggle_overwrite", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Insert, 0,
				"toggle_overwrite", 0);

  gtk_settings_install_property (g_param_spec_boolean ("gtk-entry-select-on-focus",
						       P_("Select on focus"),
						       P_("Whether to select the contents of an entry when it is focused"),
						       TRUE,
						       G_PARAM_READWRITE));
#endif

  g_type_class_add_private (gobject_class, sizeof (GtkEntryPrivate));
}

static void gtk_entry_editable_init (GtkEditableClass *iface) {

/* TODO */
  iface->do_insert_text = gtk_entry_insert_text;
  iface->get_chars = gtk_entry_get_chars;
  iface->set_selection_bounds = gtk_entry_set_selection_bounds;
#if 0
  iface->do_delete_text = gtk_entry_delete_text;
#endif
  iface->insert_text = gtk_entry_real_insert_text;
#if 0
  iface->delete_text = gtk_entry_real_delete_text;
  iface->get_selection_bounds = gtk_entry_get_selection_bounds;
  iface->set_position = gtk_entry_real_set_position;
  iface->get_position = gtk_entry_get_position;
#endif
}


/*****************************************************************************/

static void gtk_entry_init (GtkEntry *entry) {

#if 0
  GtkEntryPrivate *priv = GTK_ENTRY_GET_PRIVATE (entry);
  
  GTK_WIDGET_SET_FLAGS (entry, GTK_CAN_FOCUS);
#endif

  entry->text_size = MIN_SIZE;
  entry->text = g_malloc (entry->text_size);
  entry->text[0] = '\0';
  entry->editable = TRUE;
  entry->visible = TRUE;
  entry->invisible_char = '*';
  entry->dnd_position = -1;
  entry->width_chars = -1;
  entry->is_cell_renderer = FALSE;
  entry->editing_canceled = FALSE;
  entry->has_frame = TRUE;

  GTK_MUI(entry)->mainclass=CL_AREA;
#if 0
  priv->xalign = 0.0;

  gtk_drag_dest_set (GTK_WIDGET (entry),
                     GTK_DEST_DEFAULT_HIGHLIGHT,
                     NULL, 0,
                     GDK_ACTION_COPY | GDK_ACTION_MOVE);
  gtk_drag_dest_add_text_targets (GTK_WIDGET (entry));

  /* This object is completely private. No external entity can gain a reference
   * to it; so we create it here and destroy it in finalize().
   */
  entry->im_context = gtk_im_multicontext_new ();
  
  g_signal_connect (entry->im_context, "commit",
		    G_CALLBACK (gtk_entry_commit_cb), entry);
  g_signal_connect (entry->im_context, "preedit_changed",
		    G_CALLBACK (gtk_entry_preedit_changed_cb), entry);
  g_signal_connect (entry->im_context, "retrieve_surrounding",
		    G_CALLBACK (gtk_entry_retrieve_surrounding_cb), entry);
  g_signal_connect (entry->im_context, "delete_surrounding",
		    G_CALLBACK (gtk_entry_delete_surrounding_cb), entry);
#endif
}

GType gtk_entry_get_type (void) {

  static GType entry_type = 0;

  DebOut("gtk_entry_get_type()\n");

  if (!entry_type) {
      static const GTypeInfo entry_info = {
        sizeof (GtkEntryClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
        (GClassInitFunc) gtk_entry_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_data */
        sizeof (GtkEntry),
        0,		/* n_preallocs */
        (GInstanceInitFunc) gtk_entry_init,
      };

      static const GInterfaceInfo editable_info = {
        (GInterfaceInitFunc) gtk_entry_editable_init,  /* interface_init */
        NULL,                              /* interface_finalize */
        NULL                               /* interface_data */
      };
      
#if 0
      static const GInterfaceInfo cell_editable_info = {
        (GInterfaceInitFunc) gtk_entry_cell_editable_init,    /* interface_init */
        NULL,                                                 /* interface_finalize */
        NULL                                                  /* interface_data */
      };
#endif

      entry_type = g_type_register_static (GTK_TYPE_WIDGET, "GtkEntry",
					   &entry_info, 0);

      g_type_add_interface_static (entry_type,
				   GTK_TYPE_EDITABLE,
				   &editable_info);
#if 0
      g_type_add_interface_static (entry_type,
				   GTK_TYPE_CELL_EDITABLE,
				   &cell_editable_info);
#endif
    }

  return entry_type;
}

/****************************************************************************************************/

GtkWidget *gtk_entry_new() {
  GtkWidget *ret = NULL;
  APTR entry;

  DebOut("gtk_entry_new()\n");

  ret=g_object_new (GTK_TYPE_ENTRY, NULL);

  entry=NewObject(CL_Entry->mcc_Class, NULL,MA_Widget,(ULONG) ret,MUIA_String_MaxLen,(ULONG) 256,TAG_DONE);

  DebOut("ret=%lx\n",ret);
  DebOut("entry=%lx\n",entry);

  if((!entry)||(!ret)) {
    ErrOut("gtk_entry_new: Unable to create new entry object\n");
  }

  set(entry, MUIA_CycleChain, 1);
  GtkSetObj(ret,entry);

  return ret;
}

/****************************************************************************************************/

const gchar *gtk_entry_get_text( GtkEntry *entry ) {
  const char *text;

  g_return_val_if_fail (GTK_IS_ENTRY (entry), NULL);

  DebOut("gtk_entry_get_text(%lx)\n",entry);

  DebOut("  MuiObject: %lx\n",GTK_MUI(entry)->MuiObject);

  get( GTK_MUI(entry)->MuiObject, MUIA_String_Contents, &text );

  DebOut("  mui Text is: %s\n",text);
  DebOut("  obj->Text is: %s\n",entry->text);

#warning fix entry-text!
//  return entry->text;
  return text;
}

/**
 * gtk_entry_set_max_length:
 * @entry: a #GtkEntry.
 * @max: the maximum length of the entry, or 0 for no maximum.
 *   (other than the maximum length of entries.) The value passed in will
 *   be clamped to the range 0-65536.
 * 
 * Sets the maximum allowed length of the contents of the widget. If
 * the current contents are longer than the given length, then they
 * will be truncated to fit.
 **/
void gtk_entry_set_max_length (GtkEntry *entry, gint max) {

  g_return_if_fail (GTK_IS_ENTRY (entry));

  DebOut("gtk_entry_set_max_length(entry %lx, max %ld)\n",entry,max);

  max = CLAMP (max, 0, MAX_SIZE);

  if (max > 0 && entry->text_length > max) {
    gtk_editable_delete_text (GTK_EDITABLE (entry), max, -1);
  }

  entry->text_max_length = max;

  SetAttrs(GTK_MUI(entry)->MuiObject,MUIA_String_MaxLen,(LONG) max, TAG_DONE);

  return;
}

void gtk_entry_set_text (GtkEntry *entry, const gchar *text) {
  gint tmp_pos;

  DebOut("gtk_entry_set_text(entry %lx, text: >%s<)\n",entry,text);

  /* anything changed? */
  if (strcmp (entry->text, text) == 0)
    return;

  tmp_pos = 0;
  gtk_editable_insert_text (GTK_EDITABLE (entry), text, strlen (text), &tmp_pos);

  /*
  nnset(GTK_MUI(entry)->MuiObject,MUIA_String_Contents,(LONG) text);
  GTK_ENTRY(entry)->text_length=strlen(text);
  */

  return;
}

gchar* gtk_entry_get_chars(GtkEditable *editable, gint start_pos, gint end_pos) {
  const gchar *gtext;
  gchar *ret;
  int size;

  DebOut("gtk_editable_get_chars(%lx,%ld,%ld=\n",editable,start_pos,end_pos);

  if (end_pos < 0)
    end_pos = GTK_ENTRY(editable)->text_length;

  start_pos = MIN (GTK_ENTRY(editable)->text_length, start_pos);
  end_pos = MIN (GTK_ENTRY(editable)->text_length, end_pos);

  gtext=gtk_entry_get_text(GTK_ENTRY(editable));

  if(start_pos>strlen(gtext)) {
    WarnOut("gtk_editable_get_chars: illegal start_pos\n");
    start_pos=0;
  }

  if(end_pos==-1) {
    end_pos=strlen(gtext);
  }

  size=end_pos-start_pos+1;

  ret=g_strndup(gtext+start_pos,end_pos-start_pos);

  DebOut(" return: >%s<\n",ret);
  return ret;
}

void gtk_entry_set_editable(GtkEntry *entry, gboolean editable) {

  DebOut("gtk_entry_set_editable(%lx,%d)\n",entry,editable);

  gtk_editable_set_editable((GtkEditable *)entry,editable);
}

/* this does not hide the entry gadget, but sets secret mode 
 * this seems not to work under AROS
 */
void gtk_entry_set_visibility(GtkEntry *editable, gboolean is_visible) {
  ULONG secret;

  DebOut("gtk_entry_set_visibility(%lx,%ld)\n",editable,is_visible);

  if(is_visible) {
    secret=(ULONG) FALSE;
  }
  else {
    secret=(ULONG) TRUE;
  }

  set(GtkObj(editable),MUIA_String_Secret,secret); 

  gtk_entry_set_text(editable,gtk_entry_get_text(editable));

  return;
}


/* GtkEditable method implementations
 */
/*gtk_eentry_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);*/

#if 0
/* old way..?*/
static void gtk_entry_insert_text (GtkEditable *editable,
		       const gchar *in_text,
		       gint         new_text_length,
		       gint        *position) {

  GtkEntry *entry = GTK_ENTRY (editable);
  gchar *new_text;
  const gchar *oldtext;
  int newlen;

  DebOut("gtk_entry_insert_text (GtkEditable %lx, in_text: %s, new_text_length %d, position: %d)\n",editable,in_text,new_text_length,*position);

 
  /* old version */
  g_object_ref (editable);

  oldtext=gtk_entry_get_text(GTK_ENTRY(editable));

  if (*position < 0 || *position > strlen(oldtext))
    *position = strlen(oldtext);
 
  DebOut("oldtext: %s\n",oldtext);
  DebOut("postition: %d\n",*position);

  if(new_text_length == -1) {
    new_text_length=strlen(in_text);
  }

  newlen=strlen(oldtext)+new_text_length;

  DebOut("new_text_length: %ld\n",newlen);
  DebOut("newlen:          %ld\n",newlen);

  new_text = mgtk_allocvec(newlen+1, MEMF_ANY);

  if (new_text) {

    /* Btw in MUI4 there is a method to insert text directly
    */

    strncpy(new_text, oldtext, *position);
    strncpy(new_text + *position, in_text, new_text_length);
    strcpy(new_text + *position + new_text_length, oldtext + *position);

    *position = *position+new_text_length;

    new_text[newlen]=(char) 0;
    DebOut("new text: %s\n",new_text);
    gtk_entry_set_text(entry,new_text);
    mgtk_freevec(new_text);
  }
#if 0
  g_signal_emit_by_name (editable, "insert_text", text, new_text_length, position);
#endif
  g_object_unref (editable);

  DebOut("gtk_entry_insert_text: exit\n");
}
#endif

/* GtkEditable method implementations
 */
static void
gtk_entry_insert_text (GtkEditable *editable,
		       const gchar *new_text,
		       gint         new_text_length,
		       gint        *position)
{
  GtkEntry *entry = GTK_ENTRY (editable);
  gchar buf[64];
  gchar *text;

  DebOut("gtk_entry_insert_text (GtkEditable %lx, in_text: %s, new_text_length %d, position: %d)\n",editable,new_text,new_text_length,*position);

  DebOut("a GTK_ENTRY(%lx)->text=%s\n",entry,entry->text);

  if (*position < 0 || *position > entry->text_length)
    *position = entry->text_length;
  
  g_object_ref (editable);
  
  if (new_text_length <= 63)
    text = buf;
  else
    text = g_new (gchar, new_text_length + 1);

  text[new_text_length] = '\0';
  strncpy (text, new_text, new_text_length);

  DebOut("b GTK_ENTRY(%lx)->text=%s\n",entry,entry->text);

  g_signal_emit_by_name (editable, "insert_text", text, new_text_length, position);

#if 0
  if (!entry->visible)
    trash_area (text, new_text_length);
#endif

  if (new_text_length > 63)
    g_free (text);

  DebOut("c GTK_ENTRY(%lx)->text=%s\n",entry,entry->text);

  g_object_unref (editable);


}


static void gtk_entry_set_selection_bounds (GtkEditable *editable,
				gint         start,
				gint         end)
{
  GtkEntry *entry = GTK_ENTRY (editable);

  DebOut("gtk_entry_set_selection_bounds(%lx,%d,%d)\n",editable,start,end);
  WarnOut("gtk_editable_select_region(): requires MUI4 (%d/%d)\n", start, end);

  if (start < 0)
    start = entry->text_length;
  if (end < 0)
    end = entry->text_length;
  
/* Requires MUI4
  **
  ** VaporWare donated TextInput.mcc source code to Stefan Stuntz and it was
  ** integrated to String.mui class.
  */

  set(GtkObj(editable), MUIA_Textinput_MarkStart, start);
  set(GtkObj(editable), MUIA_Textinput_MarkEnd, end < 0 ? end : start + end);

#if 0
  gtk_entry_reset_im_context (entry);

  gtk_entry_set_positions (entry,
			   MIN (end, entry->text_length),
			   MIN (start, entry->text_length));

  gtk_entry_update_primary_selection (entry);
#endif
}


/* Default signal handlers
 */
static void
gtk_entry_real_insert_text (GtkEditable *editable,
			    const gchar *new_text,
			    gint         new_text_length,
			    gint        *position)
{
  GtkEntry *entry = GTK_ENTRY (editable);
  gint index;
  gint n_chars;
  guint password_hint_timeout;

  DebOut("gtk_entry_real_insert_text(%lx,%s,%d,%d)\n",editable,new_text,new_text_length,position);

  DebOut("entry->n_bytes: %d\n",entry->n_bytes) ;
  if (new_text_length < 0)
    new_text_length = strlen (new_text);

  n_chars = g_utf8_strlen (new_text, new_text_length);
  if (entry->text_max_length > 0 && n_chars + entry->text_length > entry->text_max_length)
    {
#if 0
      gdk_display_beep (gtk_widget_get_display (GTK_WIDGET (entry)));
#endif
      WarnOut("too many characters in widget!?\n");
      n_chars = entry->text_max_length - entry->text_length;
      new_text_length = g_utf8_offset_to_pointer (new_text, n_chars) - new_text;
    }
    n_chars=new_text_length;

  if (new_text_length + entry->n_bytes + 1 > entry->text_size)
    {
      gsize prev_size = entry->text_size;

      while (new_text_length + entry->n_bytes + 1 > entry->text_size)
	{
	  if (entry->text_size == 0)
	    entry->text_size = MIN_SIZE;
	  else
	    {
	      if (2 * (guint)entry->text_size < MAX_SIZE &&
		  2 * (guint)entry->text_size > entry->text_size)
		entry->text_size *= 2;
	      else
		{
		  entry->text_size = MAX_SIZE;
		  if (new_text_length > (gint)entry->text_size - (gint)entry->n_bytes - 1)
		    {
		      new_text_length = (gint)entry->text_size - (gint)entry->n_bytes - 1;
		      new_text_length = g_utf8_find_prev_char (new_text, new_text + new_text_length + 1) - new_text;
		      n_chars = g_utf8_strlen (new_text, new_text_length);
		    }
		  break;
		}
	    }
	}
  DebOut("entry->text pre realloc: >%s<\n",entry->text);

      if (entry->visible) {
	entry->text = g_realloc (entry->text, entry->text_size);
	DebOut("entry->text post 1 realloc: >%s<\n",entry->text);
      }
      else
	{
	  /* Same thing, just slower and without leaving stuff in memory.  */
	  gchar *et_new = g_malloc (entry->text_size);
	  memcpy (et_new, entry->text, MIN (prev_size, entry->text_size));
#if 0
	  trash_area (entry->text, prev_size);
#endif
	  g_free (entry->text);
	  entry->text = et_new;
	  DebOut("entry->text post 2 realloc: >%s<\n",entry->text);
	}
    }

  index = g_utf8_offset_to_pointer (entry->text, *position) - entry->text;

  DebOut(" entry->text: >%s<\n",entry->text);
  DebOut(" new_text: >%s<\n",new_text);
  g_memmove (entry->text + index + new_text_length, entry->text + index, entry->n_bytes - index);
  memcpy (entry->text + index, new_text, new_text_length);
  DebOut(" new_text_length: >%d<\n",new_text_length);
  DebOut("-----------------\n");
  DebOut(" entry->text: >%s<\n",entry->text);

  entry->n_bytes += new_text_length;
  entry->text_length += n_chars;

  /* NUL terminate for safety and convenience */
  entry->text[entry->n_bytes] = '\0';

  /* NULL terminate for Zune..*/
  entry->text[new_text_length] = '\0';
  DebOut("entry->text NUL terminated: >%s<\n",entry->text);
  
  if (entry->current_pos > *position)
    entry->current_pos += n_chars;
  
  if (entry->selection_bound > *position)
    entry->selection_bound += n_chars;

#if 0
  g_object_get (gtk_widget_get_settings (GTK_WIDGET (entry)),
                "gtk-entry-password-hint-timeout", &password_hint_timeout,
                NULL);

  if (password_hint_timeout > 0 && n_chars == 1 && !entry->visible &&
      (new_text_length < PASSWORD_HINT_MAX))
    {
      GtkEntryPasswordHint *password_hint = g_object_get_qdata (G_OBJECT (entry),
                                                                quark_password_hint);

      if (! password_hint)
        {
          password_hint = g_new0 (GtkEntryPasswordHint, 1);
          g_object_set_qdata_full (G_OBJECT (entry), quark_password_hint,
                                   password_hint,
                                   (GDestroyNotify) gtk_entry_password_hint_free);
        }

      memset (&password_hint->password_hint, 0x0, PASSWORD_HINT_MAX);
      password_hint->password_hint_length = new_text_length;
      memcpy (&password_hint->password_hint, new_text, new_text_length);
      password_hint->password_hint_position = *position + n_chars;
    }
  else
    {
      g_object_set_qdata (G_OBJECT (entry), quark_password_hint, NULL);
    }
#endif

  *position += n_chars;

#if 0
  gtk_entry_recompute (entry);

  emit_changed (entry);
#endif
  g_object_notify (G_OBJECT (editable), "text");
}
