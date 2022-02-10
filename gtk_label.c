/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005-2006 Oliver Brunner
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
 * $Id: gtk_label.c,v 1.20 2013/03/22 10:01:44 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk/gtk.h"
#include "gtk/gtklabel.h"
#include "classes/classes.h"
#include "gtk_globals.h"

#include <gdk/gdkkeysyms.h>

#define GTK_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_LABEL, GtkLabelPrivate))

typedef struct {
  gint width_chars;
  gint max_width_chars;
  guint single_line_mode : 1;
  guint have_transform : 1;
  gdouble angle;
} GtkLabelPrivate;

struct _GtkLabelSelectionInfo {
  GdkWindow *window;
  gint selection_anchor;
  gint selection_end;
  GtkWidget *popup_menu;
  
  gint drag_start_x;
  gint drag_start_y;

  guint in_drag : 1;
};

enum {
  MOVE_CURSOR,
  COPY_CLIPBOARD,
  POPULATE_POPUP,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_LABEL,
  PROP_ATTRIBUTES,
  PROP_USE_MARKUP,
  PROP_USE_UNDERLINE,
  PROP_JUSTIFY,
  PROP_PATTERN,
  PROP_WRAP,
  PROP_SELECTABLE,
  PROP_MNEMONIC_KEYVAL,
  PROP_MNEMONIC_WIDGET,
  PROP_CURSOR_POSITION,
  PROP_SELECTION_BOUND,
  PROP_ELLIPSIZE,
  PROP_WIDTH_CHARS,
  PROP_SINGLE_LINE_MODE,
  PROP_ANGLE,
  PROP_MAX_WIDTH_CHARS
};

static guint signals[LAST_SIGNAL] = { 0 };

static void gtk_label_class_init        (GtkLabelClass    *klass);
static void gtk_label_init              (GtkLabel         *label);
static void gtk_label_set_property      (GObject          *object,
					 guint             prop_id,
					 const GValue     *value,
					 GParamSpec       *pspec);
static void gtk_label_get_property      (GObject          *object,
					 guint             prop_id,
					 GValue           *value,
					 GParamSpec       *pspec);
#if 0
static void gtk_label_destroy           (GtkObject        *object);
static void gtk_label_finalize          (GObject          *object);
static void gtk_label_size_request      (GtkWidget        *widget,
					 GtkRequisition   *requisition);
static void gtk_label_size_allocate     (GtkWidget        *widget,
                                         GtkAllocation    *allocation);
static void gtk_label_state_changed     (GtkWidget        *widget,
                                         GtkStateType      state);
static void gtk_label_style_set         (GtkWidget        *widget,
					 GtkStyle         *previous_style);
static void gtk_label_direction_changed (GtkWidget        *widget,
					 GtkTextDirection  previous_dir);
static gint gtk_label_expose            (GtkWidget        *widget,
					 GdkEventExpose   *event);

static void gtk_label_realize           (GtkWidget        *widget);
static void gtk_label_unrealize         (GtkWidget        *widget);
static void gtk_label_map               (GtkWidget        *widget);
static void gtk_label_unmap             (GtkWidget        *widget);

static gboolean gtk_label_button_press      (GtkWidget        *widget,
					     GdkEventButton   *event);
static gboolean gtk_label_button_release    (GtkWidget        *widget,
					     GdkEventButton   *event);
static gboolean gtk_label_motion            (GtkWidget        *widget,
					     GdkEventMotion   *event);


static void gtk_label_set_text_internal          (GtkLabel      *label,
						  gchar         *str);
static void gtk_label_set_label_internal         (GtkLabel      *label,
						  gchar         *str);
static void gtk_label_set_use_markup_internal    (GtkLabel      *label,
						  gboolean       val);
static void gtk_label_set_use_underline_internal (GtkLabel      *label,
						  gboolean       val);
static void gtk_label_set_attributes_internal    (GtkLabel      *label,
						  PangoAttrList *attrs);
static void gtk_label_set_uline_text_internal    (GtkLabel      *label,
						  const gchar   *str);
static void gtk_label_set_pattern_internal       (GtkLabel      *label,
				                  const gchar   *pattern);
static void set_markup                           (GtkLabel      *label,
						  const gchar   *str,
						  gboolean       with_uline);
static void gtk_label_recalculate                (GtkLabel      *label);
static void gtk_label_hierarchy_changed          (GtkWidget     *widget,
						  GtkWidget     *old_toplevel);
static void gtk_label_screen_changed             (GtkWidget     *widget,
						  GdkScreen     *old_screen);

static void gtk_label_create_window       (GtkLabel *label);
static void gtk_label_destroy_window      (GtkLabel *label);
static void gtk_label_clear_layout        (GtkLabel *label);
static void gtk_label_ensure_layout       (GtkLabel *label);
static void gtk_label_select_region_index (GtkLabel *label,
                                           gint      anchor_index,
                                           gint      end_index);

static gboolean gtk_label_mnemonic_activate (GtkWidget         *widget,
					     gboolean           group_cycling);
static void     gtk_label_setup_mnemonic    (GtkLabel          *label,
					     guint              last_key);
static void     gtk_label_drag_data_get     (GtkWidget         *widget,
					     GdkDragContext    *context,
					     GtkSelectionData  *selection_data,
					     guint              info,
					     guint              time);


/* For selectable lables: */
static void gtk_label_move_cursor        (GtkLabel        *label,
					  GtkMovementStep  step,
					  gint             count,
					  gboolean         extend_selection);
static void gtk_label_copy_clipboard     (GtkLabel        *label);
static void gtk_label_select_all         (GtkLabel        *label);
static void gtk_label_do_popup           (GtkLabel        *label,
					  GdkEventButton  *event);

static gint gtk_label_move_forward_word  (GtkLabel        *label,
					  gint             start);
static gint gtk_label_move_backward_word (GtkLabel        *label,
					  gint             start);
#endif

static GtkMiscClass *parent_class = NULL;


GType gtk_label_get_type (void) {

  static GType label_type = 0;

  DebOut("gtk_label_get_type()\n");
  
  if (!label_type) {
    static const GTypeInfo label_info = {
      sizeof (GtkLabelClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) gtk_label_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof (GtkLabel),
      32,             /* n_preallocs */
      (GInstanceInitFunc) gtk_label_init,
    };

    label_type = g_type_register_static (GTK_TYPE_MISC, "GtkLabel",
            &label_info, 0);
  }
  
  return label_type;
}

static void gtk_label_class_init (GtkLabelClass *class) {

  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
#if 0
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
  GtkBindingSet *binding_set;
#endif

  DebOut("gtk_label_class_init(%lx)\n",class);

  parent_class = g_type_class_peek_parent (class);
  
  gobject_class->set_property = gtk_label_set_property;
  gobject_class->get_property = gtk_label_get_property;
#if 0
  gobject_class->finalize = gtk_label_finalize;

  object_class->destroy = gtk_label_destroy;
  
  widget_class->size_request = gtk_label_size_request;
  widget_class->size_allocate = gtk_label_size_allocate;
  widget_class->state_changed = gtk_label_state_changed;
  widget_class->style_set = gtk_label_style_set;
  widget_class->direction_changed = gtk_label_direction_changed;
  widget_class->expose_event = gtk_label_expose;
  widget_class->realize = gtk_label_realize;
  widget_class->unrealize = gtk_label_unrealize;
  widget_class->map = gtk_label_map;
  widget_class->unmap = gtk_label_unmap;
  widget_class->button_press_event = gtk_label_button_press;
  widget_class->button_release_event = gtk_label_button_release;
  widget_class->motion_notify_event = gtk_label_motion;
  widget_class->hierarchy_changed = gtk_label_hierarchy_changed;
  widget_class->screen_changed = gtk_label_screen_changed;
  widget_class->mnemonic_activate = gtk_label_mnemonic_activate;
  widget_class->drag_data_get = gtk_label_drag_data_get;

  class->move_cursor = gtk_label_move_cursor;
  class->copy_clipboard = gtk_label_copy_clipboard;

  signals[MOVE_CURSOR] = 
    g_signal_new ("move_cursor",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkLabelClass, move_cursor),
		  NULL, NULL,
		  _gtk_marshal_VOID__ENUM_INT_BOOLEAN,
		  G_TYPE_NONE, 3,
		  GTK_TYPE_MOVEMENT_STEP,
		  G_TYPE_INT,
		  G_TYPE_BOOLEAN);
#endif
  
  signals[COPY_CLIPBOARD] =
    g_signal_new ("copy_clipboard",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkLabelClass, copy_clipboard),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  
#if 0
  signals[POPULATE_POPUP] =
    g_signal_new ("populate_popup",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtkLabelClass, populate_popup),
		  NULL, NULL,
		  _gtk_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
		  GTK_TYPE_MENU);
#endif

  g_object_class_install_property (gobject_class,
                                   PROP_LABEL,
                                   g_param_spec_string ("label",
                                                        P_("Label"),
                                                        P_("The text of the label"),
                                                        NULL,
                                                        G_PARAM_READWRITE));
#if 0
  g_object_class_install_property (gobject_class,
				   PROP_ATTRIBUTES,
				   g_param_spec_boxed ("attributes",
						       P_("Attributes"),
						       P_("A list of style attributes to apply to the text of the label"),
						       PANGO_TYPE_ATTR_LIST,
						       G_PARAM_READWRITE));
#endif
  g_object_class_install_property (gobject_class,
                                   PROP_USE_MARKUP,
                                   g_param_spec_boolean ("use_markup",
							 P_("Use markup"),
							 P_("The text of the label includes XML markup. See pango_parse_markup()"),
                                                        FALSE,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_USE_UNDERLINE,
                                   g_param_spec_boolean ("use_underline",
							 P_("Use underline"),
							 P_("If set, an underline in the text indicates the next character should be used for the mnemonic accelerator key"),
                                                        FALSE,
                                                        G_PARAM_READWRITE));

#if 0
  g_object_class_install_property (gobject_class,
				   PROP_JUSTIFY,
                                   g_param_spec_enum ("justify",
                                                      P_("Justification"),
                                                      P_("The alignment of the lines in the text of the label relative to each other. This does NOT affect the alignment of the label within its allocation. See GtkMisc::xalign for that"),
						      GTK_TYPE_JUSTIFICATION,
						      GTK_JUSTIFY_LEFT,
                                                      G_PARAM_READWRITE));
#endif

  g_object_class_install_property (gobject_class,
                                   PROP_PATTERN,
                                   g_param_spec_string ("pattern",
                                                        P_("Pattern"),
                                                        P_("A string with _ characters in positions correspond to characters in the text to underline"),
                                                        NULL,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class,
                                   PROP_WRAP,
                                   g_param_spec_boolean ("wrap",
                                                        P_("Line wrap"),
                                                        P_("If set, wrap lines if the text becomes too wide"),
                                                        FALSE,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTABLE,
                                   g_param_spec_boolean ("selectable",
                                                        P_("Selectable"),
                                                        P_("Whether the label text can be selected with the mouse"),
                                                        FALSE,
                                                        G_PARAM_READWRITE));
#if 0
  g_object_class_install_property (gobject_class,
                                   PROP_MNEMONIC_KEYVAL,
                                   g_param_spec_uint ("mnemonic_keyval",
						      P_("Mnemonic key"),
						      P_("The mnemonic accelerator key for this label"),
						      0,
						      G_MAXUINT,
						      GDK_VoidSymbol,
						      G_PARAM_READABLE));
#endif
  g_object_class_install_property (gobject_class,
                                   PROP_MNEMONIC_WIDGET,
                                   g_param_spec_object ("mnemonic_widget",
							P_("Mnemonic widget"),
							P_("The widget to be activated when the label's mnemonic "
							  "key is pressed"),
							GTK_TYPE_WIDGET,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_CURSOR_POSITION,
                                   g_param_spec_int ("cursor_position",
                                                     P_("Cursor Position"),
                                                     P_("The current position of the insertion cursor in chars"),
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READABLE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTION_BOUND,
                                   g_param_spec_int ("selection_bound",
                                                     P_("Selection Bound"),
                                                     P_("The position of the opposite end of the selection from the cursor in chars"),
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READABLE));
  
  /**
   * GtkLabel:ellipsize:
   *
   * The preferred place to ellipsize the string, if the label does not have 
   * enough room to display the entire string, specified as a #PangoEllisizeMode. 
   *
   * Note that setting this property to a value other than %PANGO_ELLIPSIZE_NONE 
   * has the side-effect that the label requests only enough space to display the
   * ellipsis "...". In particular, this means that ellipsizing labels don't
   * work well in notebook tabs, unless the tab's ::tab-expand property is set
   * to %TRUE. Other means to set a label's width are
   * gtk_widget_set_size_request() and gtk_label_set_width_chars().
   *
   * Since: 2.6
   */
#if 0
  g_object_class_install_property (gobject_class,
				   PROP_ELLIPSIZE,
                                   g_param_spec_enum ("ellipsize",
                                                      P_("Ellipsize"),
                                                      P_("The preferred place to ellipsize the string, if the label does not have enough room to display the entire string, if at all"),
						      PANGO_TYPE_ELLIPSIZE_MODE,
						      PANGO_ELLIPSIZE_NONE,
                                                      G_PARAM_READWRITE));
#endif

  /**
   * GtkLabel:width-chars:
   * 
   * The desired width of the label, in characters. If this property is set to
   * -1, the width will be calculated automatically, otherwise the label will
   * request either 3 characters or the property value, whichever is greater.
   * If the width-chars property is set to a positive value, then the 
   * max-width-chars property is ignored. 
   *
   * Since: 2.6
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_WIDTH_CHARS,
                                   g_param_spec_int ("width_chars",
                                                     P_("Width In Characters"),
                                                     P_("The desired width of the label, in characters"),
                                                     -1,
                                                     G_MAXINT,
                                                     -1,
                                                     G_PARAM_READWRITE));
  
  /**
   * GtkLabel:single-line-mode:
   * 
   * Whether the label is in single line mode. In single line mode,
   * the height of the label does not depend on the actual text, it
   * is always set to ascent + descent of the font. This can be an
   * advantage in situations where resizing the label because of text 
   * changes would be distracting, e.g. in a statusbar.
   *
   * Since: 2.6
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_SINGLE_LINE_MODE,
                                   g_param_spec_boolean ("single-line-mode",
                                                        P_("Single Line Mode"),
                                                        P_("Whether the label is in single line mode"),
                                                        FALSE,
                                                        G_PARAM_READWRITE));

  /**
   * GtkLabel:angle:
   * 
   * The angle that the baseline of the label makes with the horizontal,
   * in degrees, measured counterclockwise. An angle of 90 reads from
   * from bottom to top, an angle of 270, from top to bottom. Ignored
   * if the label is selectable, wrapped, or ellipsized.
   *
   * Since: 2.6
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ANGLE,
                                   g_param_spec_double ("angle",
							P_("Angle"),
							P_("Angle at which the label is rotated"),
							0.0,
							360.0,
							0.0, 
							G_PARAM_READWRITE));
  
  /**
   * GtkLabel:max-width-chars:
   * 
   * The desired maximum width of the label, in characters. If this property 
   * is set to -1, the width will be calculated automatically, otherwise the 
   * label will request space for no more than the requested number of 
   * characters. If the width-chars property is set to a positive value,
   * then the max-width-chars property is ignored.
   * 
   * Since: 2.6
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_MAX_WIDTH_CHARS,
                                   g_param_spec_int ("max_width_chars",
                                                     P_("Maximum Width In Characters"),
                                                     P_("The desired maximum width of the label, in characters"),
                                                     -1,
                                                     G_MAXINT,
                                                     -1,
                                                     G_PARAM_READWRITE));
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
  
  add_move_binding (binding_set, GDK_f, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_LOGICAL_POSITIONS, 1);
  
  add_move_binding (binding_set, GDK_b, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_LOGICAL_POSITIONS, -1);
  
  add_move_binding (binding_set, GDK_Right, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, 1);

  add_move_binding (binding_set, GDK_Left, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, -1);

  add_move_binding (binding_set, GDK_KP_Right, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, 1);

  add_move_binding (binding_set, GDK_KP_Left, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_WORDS, -1);
  
  add_move_binding (binding_set, GDK_a, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_PARAGRAPH_ENDS, -1);

  add_move_binding (binding_set, GDK_e, GDK_CONTROL_MASK,
		    GTK_MOVEMENT_PARAGRAPH_ENDS, 1);

  add_move_binding (binding_set, GDK_f, GDK_MOD1_MASK,
		    GTK_MOVEMENT_WORDS, 1);

  add_move_binding (binding_set, GDK_b, GDK_MOD1_MASK,
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

  /* copy */
  gtk_binding_entry_add_signal (binding_set, GDK_c, GDK_CONTROL_MASK,
				"copy_clipboard", 0);
#endif
				
  g_type_class_add_private (class, sizeof (GtkLabelPrivate));
}

#if 0
# TODO
GtkWidget *gtk_label_new_with_mnemonic(const char *str)
{
	GtkWidget *widget = mgtk_label_new(CL_Label, str);

	if (widget)
	{
		SetAttrs(GtkObj(widget), MUIA_Text_ControlChar, '_', MUIA_Text_HiChar, '_', TAG_DONE);
	}

	return widget;
}

/**********************************************************************
	Supported functions
**********************************************************************/

void gtk_label_set_justify(GtkLabel *label, GtkJustification jtype)
{
	const char *j;

	// fixme: GTK_JUSTIFY_FILL not supported

	label->jtype = jtype;

	switch (jtype)
	{
		case GTK_JUSTIFY_LEFT  : j = MUIX_L; break;
		case GTK_JUSTIFY_RIGHT : j = MUIX_R; break;

		default:
		case GTK_JUSTIFY_CENTER:
		case GTK_JUSTIFY_FILL  : j = MUIX_C; break;
	}

	SetAttrs(GtkObj(label), MUIA_Text_PreParse, j, TAG_DONE);
	MGTK_UPDATE_LABEL
}

GtkJustification gtk_label_get_justify(GtkLabel *label)
{
	return label->jtype;
}

PangoEllipsizeMode gtk_label_get_ellipsize(GtkLabel *label)
{
	return label->ellipsize;
}
#endif

const gchar *gtk_label_get_text(GtkLabel *label)
{
	// fixme: should return without pango markup or mnemonics
	//return (const gchar *)xget(GtkObj(label), MUIA_Text_Contents);
  return label->text;
}

#if 0
const gchar *gtk_label_get_label(GtkLabel *label)
{
	return (const gchar *)xget(GtkObj(label), MUIA_Text_Contents);
}

void gtk_label_set_text_with_mnemonic(GtkLabel *label, const gchar *str)
{
	SetAttrs(GtkObj(label), MUIA_Text_ControlChar, '_', MUIA_Text_HiChar, '_', MUIA_Text_Contents, str, TAG_DONE);
	MGTK_UPDATE_LABEL
}

gboolean gtk_label_get_use_underline(GtkLabel *label)
{
	return label->use_underline;
}

void gtk_label_set_label(GtkLabel *label, const gchar *str)
{
	SetAttrs(GtkObj(label),
		MUIA_Text_ControlChar, label->use_underline ? '_' : 0,
		MUIA_Text_HiChar, label->use_underline ? '_' : 0,
		MUIA_Text_Contents, str,
	TAG_DONE);
	MGTK_UPDATE_LABEL
}

void gtk_label_set_use_markup(GtkLabel *label, gboolean setting)
{
	label->use_markup = setting;
}

void gtk_label_set_use_underline(GtkLabel *label, gboolean setting)
{
	label->use_underline = setting;
}

guint gtk_label_get_mnemonic_keyval(GtkLabel *label)
{
	return label->mnemonic_keyval;
}

GtkWidget *gtk_label_get_mnemonic_widget(GtkLabel *label)
{
	return label->mnemonic_widget;
}

void gtk_label_set_mnemonic_widget(GtkLabel *label, GtkWidget *widget)
{
	label->mnemonic_widget = widget;

	if (label->mnemonic_keyval > 0 && label->mnemonic_keyval <= 0x7f)
	{
		set(GtkObj(widget), MUIA_ControlChar, label->mnemonic_keyval);
	}
}
#endif /* itix version */

static void 
gtk_label_set_property (GObject      *object,
			guint         prop_id,
			const GValue *value,
			GParamSpec   *pspec)
{
  GtkLabel *label;

  label = GTK_LABEL (object);
  
  switch (prop_id)
    {
#if 0
    case PROP_LABEL:
      gtk_label_set_label (label, g_value_get_string (value));
      break;
    case PROP_ATTRIBUTES:
      gtk_label_set_attributes (label, g_value_get_boxed (value));
      break;
    case PROP_USE_MARKUP:
      gtk_label_set_use_markup (label, g_value_get_boolean (value));
      break;
    case PROP_USE_UNDERLINE:
      gtk_label_set_use_underline (label, g_value_get_boolean (value));
      break;
    case PROP_JUSTIFY:
      gtk_label_set_justify (label, g_value_get_enum (value));
      break;
    case PROP_PATTERN:
      gtk_label_set_pattern (label, g_value_get_string (value));
      break;
    case PROP_WRAP:
      gtk_label_set_line_wrap (label, g_value_get_boolean (value));
      break;	  
    case PROP_SELECTABLE:
      gtk_label_set_selectable (label, g_value_get_boolean (value));
      break;	  
    case PROP_MNEMONIC_WIDGET:
      gtk_label_set_mnemonic_widget (label, (GtkWidget*) g_value_get_object (value));
      break;
    case PROP_ELLIPSIZE:
      gtk_label_set_ellipsize (label, g_value_get_enum (value));
      break;
    case PROP_WIDTH_CHARS:
      gtk_label_set_width_chars (label, g_value_get_int (value));
      break;
    case PROP_SINGLE_LINE_MODE:
      gtk_label_set_single_line_mode (label, g_value_get_boolean (value));
      break;	  
    case PROP_ANGLE:
      gtk_label_set_angle (label, g_value_get_double (value));
      break;
    case PROP_MAX_WIDTH_CHARS:
      gtk_label_set_max_width_chars (label, g_value_get_int (value));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_label_get_property (GObject     *object,
			guint        prop_id,
			GValue      *value,
			GParamSpec  *pspec)
{
  GtkLabel *label;
  
  label = GTK_LABEL (object);
  
  switch (prop_id)
    {
    case PROP_LABEL:
      g_value_set_string (value, label->label);
      break;
#if 0
    case PROP_ATTRIBUTES:
      g_value_set_boxed (value, label->attrs);
      break;
#endif
    case PROP_USE_MARKUP:
      g_value_set_boolean (value, label->use_markup);
      break;
    case PROP_USE_UNDERLINE:
      g_value_set_boolean (value, label->use_underline);
      break;
    case PROP_JUSTIFY:
      g_value_set_enum (value, label->jtype);
      break;
    case PROP_WRAP:
      g_value_set_boolean (value, label->wrap);
      break;
#if 0
    case PROP_SELECTABLE:
      g_value_set_boolean (value, gtk_label_get_selectable (label));
      break;
#endif
    case PROP_MNEMONIC_KEYVAL:
      g_value_set_uint (value, label->mnemonic_keyval);
      break;
    case PROP_MNEMONIC_WIDGET:
      g_value_set_object (value, (GObject*) label->mnemonic_widget);
      break;
    case PROP_CURSOR_POSITION:
      if (label->select_info)
	{
	  gint offset = g_utf8_pointer_to_offset (label->text,
						  label->text + label->select_info->selection_end);
	  g_value_set_int (value, offset);
	}
      else
	g_value_set_int (value, 0);
      break;
    case PROP_SELECTION_BOUND:
      if (label->select_info)
	{
	  gint offset = g_utf8_pointer_to_offset (label->text,
						  label->text + label->select_info->selection_anchor);
	  g_value_set_int (value, offset);
	}
      else
	g_value_set_int (value, 0);
      break;
    case PROP_ELLIPSIZE:
      g_value_set_enum (value, label->ellipsize);
      break;
#if 0
    case PROP_WIDTH_CHARS:
      g_value_set_int (value, gtk_label_get_width_chars (label));
      break;
    case PROP_SINGLE_LINE_MODE:
      g_value_set_boolean (value, gtk_label_get_single_line_mode (label));
      break;
    case PROP_ANGLE:
      g_value_set_double (value, gtk_label_get_angle (label));
      break;
    case PROP_MAX_WIDTH_CHARS:
      g_value_set_int (value, gtk_label_get_max_width_chars (label));
      break;
#endif

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_label_init (GtkLabel *label) {
  GtkLabelPrivate *priv;

  DebOut("gtk_label_init()\n");

  GTK_WIDGET_SET_FLAGS (label, GTK_NO_WINDOW);

  priv = GTK_LABEL_GET_PRIVATE (label);
  priv->width_chars = -1;
  priv->angle = 0.0;
  priv->max_width_chars = -1;
  label->label = NULL;

  label->jtype = GTK_JUSTIFY_LEFT;
  label->wrap = FALSE;
#if 0
  label->ellipsize = PANGO_ELLIPSIZE_NONE;
#endif

  label->use_underline = FALSE;
  label->use_markup = FALSE;
  
#if 0
  label->mnemonic_keyval = GDK_VoidSymbol;
  label->layout = NULL;
  label->attrs = NULL;
#endif
  label->text = NULL;

  label->mnemonic_widget = NULL;
  label->mnemonic_window = NULL;
  
  GtkSetObj(GTK_WIDGET(label), NewObject(CL_Label->mcc_Class,NULL,TAG_DONE));
  set(GtkObj(label),MA_Widget,(ULONG) label);

  DebOut("  new object: %lx\n",GtkObj(label));

  GTK_MUI(label)->mainclass = CL_AREA;

#if 1
  gtk_label_set_text (label, "");
#else
  gtk_label_set_text (label, "empty label2");
#endif
}

/**
 * gtk_label_new:
 * @str: The text of the label
 *
 * Creates a new label with the given text inside it. You can
 * pass %NULL to get an empty label widget.
 *
 * Return value: the new #GtkLabel
 **/
GtkWidget *gtk_label_new (const gchar *str) {
  GtkLabel *label;
  
  DebOut("gtk_label_new(%s)\n",str);

  label = g_object_new (GTK_TYPE_LABEL, NULL);

  if (str && *str) {
    gtk_label_set_text (label, str);
  }
  
  return GTK_WIDGET (label);
}

/*******************
 * as far as I see, changing the text of a text label is
 * not possible in MUI!?
 * ok, the we delete the label and create a new one ;)
 *******************/

void gtk_label_set_text(GtkLabel *label, const char *str) {

  DebOut("gtk_label_set_text(%x,%s)\n",(int) label,str);

  if(str && label->text && !strcmp(str,label->text) && !strcmp(str,(char *) xget(GtkObj(label),MUIA_Text_Contents))) {
    return;
  }

  DebOut("1..\n");

  /* delete old string.. */
  if(label->text) {
    g_free(label->text);
    label->text=(char *) NULL;
  }

  DebOut("1..\n");
  if(str) {
    label->text=g_strdup(str);
  }
  else {
#if 0
    label->text=g_strdup("");
#else
    label->text=g_strdup("empty label1");
#endif
  }

  DebOut("label->text: %s\n", label->text);
  set(GtkObj(label),MUIA_Text_Contents,label->text);

  DebOut("gtk_label_set_text: %s\n",GTK_LABEL(label)->text);
}

#if 0
void gtk_label_set_ellipsize(GtkLabel *label, PangoEllipsizeMode mode) {

	label->ellipsize = mode;
}
#endif

/*
 * gtk_label_get is deprecated and should not be used in newly-written code.
 *
 */
void gtk_label_get(GtkLabel *label, char **str) {

  DebOut("gtk_label_get(%lx,..)\n",label);

  g_return_if_fail (GTK_IS_LABEL (label));
  g_return_if_fail (str != NULL);

  *str = label->text;
}

void gtk_label_set_width_chars(GtkLabel *label, gint n_chars)
{
}

void gtk_label_set_line_wrap(GtkLabel *label, gboolean wrap)
{
}

void gtk_label_get_layout_offsets(GtkLabel *label, gint *x, gint *y)
{
	// for complimentary only
	*x = 0;
	*y = 0;
}

gboolean gtk_label_get_selectable(GtkLabel *label)
{
	return FALSE;
}

void gtk_label_select_region(GtkLabel *label, gint start_offset, gint end_offset)
{
}

void gtk_label_set_selectable(GtkLabel *label, gboolean setting)
{
}

#if 0
/* TODO */
PangoAttrList *gtk_label_get_attributes(GtkLabel *label)
{
	return NULL;
}

PangoLayout *gtk_label_get_layout(GtkLabel *label)
{
	return label->layout;
}
#endif

gint gtk_label_get_width_chars(GtkLabel *label)
{
	return 80;
}

gboolean gtk_label_get_selection_bounds(GtkLabel *label, gint *start, gint *end)
{
	*start = 0;
	*end = 0;
	return FALSE;
}

gboolean gtk_label_get_single_line_mode(GtkLabel *label)
{
	return FALSE;
}

gdouble gtk_label_get_angle(GtkLabel *label)
{
	return 0.0f;
}

void gtk_label_set_single_line_mode(GtkLabel *label, gboolean single_line_mode)
{
}

void gtk_label_set_angle(GtkLabel *label, gdouble angle)
{
}

/**********************************************************************
	Deprecated functions, wont be implemented
**********************************************************************/

#if 0
void gtk_label_get(GtkLabel *label, char **str)
{
}

guint gtk_label_parse_uline(GtkLabel *label, const gchar *string)
{
}
#endif
