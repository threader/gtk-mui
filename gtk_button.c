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
 * $Id: gtk_button.c,v 1.17 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <stdio.h>
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

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

#define CHILD_SPACING     1

static const GtkBorder default_default_border = { 1, 1, 1, 1 };
static const GtkBorder default_default_outside_border = { 0, 0, 0, 0 };

/* Time out before giving up on getting a key release when animating
 * the close button.
 */
#define ACTIVATE_TIMEOUT 250

enum {
  PRESSED,
  RELEASED,
  CLICKED,
  ENTER,
  LEAVE,
  ACTIVATE,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_LABEL,
  PROP_IMAGE,
  PROP_RELIEF,
  PROP_USE_UNDERLINE,
  PROP_USE_STOCK,
  PROP_FOCUS_ON_CLICK,
  PROP_XALIGN,
  PROP_YALIGN,
};

#define GTK_BUTTON_GET_PRIVATE(o)       (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_BUTTON, GtkButtonPrivate))
typedef struct _GtkButtonPrivate GtkButtonPrivate;

struct _GtkButtonPrivate
{
  gfloat       xalign;
  gfloat       yalign;
  GtkWidget   *image;
  guint        align_set : 1;
  guint        image_is_stock : 1;
};

static void gtk_button_class_init     (GtkButtonClass   *klass);
static void gtk_button_init           (GtkButton        *button);
#if 0
static void gtk_button_destroy        (GtkObject        *object);
#endif
static void gtk_button_set_property   (GObject         *object,
                                       guint            prop_id,
                                       const GValue    *value,
                                       GParamSpec      *pspec);
static void gtk_button_get_property   (GObject         *object,
                                       guint            prop_id,
                                       GValue          *value,
                                       GParamSpec      *pspec);
#if 0
static void gtk_button_screen_changed (GtkWidget        *widget,
				       GdkScreen        *previous_screen);
static void gtk_button_realize        (GtkWidget        *widget);
static void gtk_button_unrealize      (GtkWidget        *widget);
static void gtk_button_map            (GtkWidget        *widget);
static void gtk_button_unmap          (GtkWidget        *widget);
static void gtk_button_size_request   (GtkWidget        *widget,
				       GtkRequisition   *requisition);
static void gtk_button_size_allocate  (GtkWidget        *widget,
				       GtkAllocation    *allocation);
static gint gtk_button_expose         (GtkWidget        *widget,
				       GdkEventExpose   *event);
static gint gtk_button_button_press   (GtkWidget        *widget,
				       GdkEventButton   *event);
static gint gtk_button_button_release (GtkWidget        *widget,
				       GdkEventButton   *event);
static gint gtk_button_key_release    (GtkWidget        *widget,
				       GdkEventKey      *event);
static gint gtk_button_enter_notify   (GtkWidget        *widget,
				       GdkEventCrossing *event);
static gint gtk_button_leave_notify   (GtkWidget        *widget,
				       GdkEventCrossing *event);
static void gtk_real_button_pressed   (GtkButton        *button);
static void gtk_real_button_released  (GtkButton        *button);
static void gtk_real_button_activate  (GtkButton         *button);
static void gtk_button_update_state   (GtkButton        *button);
static void gtk_button_add            (GtkContainer   *container,
			               GtkWidget      *widget);
static GType gtk_button_child_type    (GtkContainer     *container);
static void gtk_button_finish_activate (GtkButton *button,
					gboolean   do_it);

static GObject*	gtk_button_constructor (GType                  type,
					guint                  n_construct_properties,
					GObjectConstructParam *construct_params);
#endif
static void gtk_button_construct_child (GtkButton             *button);
#if 0
static void gtk_button_state_changed   (GtkWidget             *widget,
					GtkStateType           previous_state);
static void gtk_button_grab_notify     (GtkWidget             *widget,
					gboolean               was_grabbed);
#endif



static GtkBinClass *parent_class = NULL;
static guint button_signals[LAST_SIGNAL] = { 0 };

GType gtk_button_get_type (void) {
  static GType button_type = 0;

  DebOut("gtk_button_get_type()\n");

  if (!button_type) {
    static const GTypeInfo button_info = {
      sizeof (GtkButtonClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_button_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkButton),
      16,		/* n_preallocs */
      (GInstanceInitFunc) gtk_button_init,
    };

    button_type = g_type_register_static (GTK_TYPE_BIN, "GtkButton",
					    &button_info, 0);
  }

  DebOut("  button_type=%lx\n",button_type);

  return button_type;
}

static void gtk_button_class_init (GtkButtonClass *klass) {

  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  DebOut("gtk_button_class_init(%lx)\n",klass);

  gobject_class = G_OBJECT_CLASS (klass);
  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;
  container_class = (GtkContainerClass*) klass;
  
  parent_class = g_type_class_peek_parent (klass);

#if 0
  gobject_class->constructor = gtk_button_constructor;
#endif
  gobject_class->set_property = gtk_button_set_property;
  gobject_class->get_property = gtk_button_get_property;

#if 0
  object_class->destroy = gtk_button_destroy;

  widget_class->screen_changed = gtk_button_screen_changed;
  widget_class->realize = gtk_button_realize;
  widget_class->unrealize = gtk_button_unrealize;
  widget_class->map = gtk_button_map;
  widget_class->unmap = gtk_button_unmap;
  widget_class->size_request = gtk_button_size_request;
  widget_class->size_allocate = gtk_button_size_allocate;
  widget_class->expose_event = gtk_button_expose;
  widget_class->button_press_event = gtk_button_button_press;
  widget_class->button_release_event = gtk_button_button_release;
  widget_class->key_release_event = gtk_button_key_release;
  widget_class->enter_notify_event = gtk_button_enter_notify;
  widget_class->leave_notify_event = gtk_button_leave_notify;
  widget_class->state_changed = gtk_button_state_changed;
  widget_class->grab_notify = gtk_button_grab_notify;

  container_class->child_type = gtk_button_child_type;
  container_class->add = gtk_button_add;

  klass->pressed = gtk_real_button_pressed;
  klass->released = gtk_real_button_released;
  klass->clicked = NULL;
  klass->enter = gtk_button_update_state;
  klass->leave = gtk_button_update_state;
  klass->activate = gtk_real_button_activate;

#endif
  g_object_class_install_property (gobject_class,
                                   PROP_LABEL,
                                   g_param_spec_string ("label",
                                                        P_("Label"),
                                                        P_("Text of the label widget inside the button, if the button contains a label widget"),
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
#if 0
  g_object_class_install_property (gobject_class,
                                   PROP_USE_UNDERLINE,
                                   g_param_spec_boolean ("use_underline",
							 P_("Use underline"),
							 P_("If set, an underline in the text indicates the next character should be used for the mnemonic accelerator key"),
                                                        FALSE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property (gobject_class,
                                   PROP_USE_STOCK,
                                   g_param_spec_boolean ("use_stock",
							 P_("Use stock"),
							 P_("If set, the label is used to pick a stock item instead of being displayed"),
                                                        FALSE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property (gobject_class,
                                   PROP_FOCUS_ON_CLICK,
                                   g_param_spec_boolean ("focus_on_click",
							 P_("Focus on click"),
							 P_("Whether the button grabs focus when it is clicked with the mouse"),
							 TRUE,
							 G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_RELIEF,
                                   g_param_spec_enum ("relief",
                                                      P_("Border relief"),
                                                      P_("The border relief style"),
                                                      GTK_TYPE_RELIEF_STYLE,
                                                      GTK_RELIEF_NORMAL,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));
  
  /**
   * GtkButton:xalign:
   *
   * If the child of the button is a #GtkMisc or #GtkAlignment, this property 
   * can be used to control it's horizontal alignment. 0.0 is left aligned, 
   * 1.0 is right aligned.
   * 
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_XALIGN,
                                   g_param_spec_float("xalign",
                                                      P_("Horizontal alignment for child"),
                                                      P_("Horizontal position of child in available space. 0.0 is left aligned, 1.0 is right aligned"),
                                                      0.0,
                                                      1.0,
                                                      0.5,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));

  /**
   * GtkButton:yalign:
   *
   * If the child of the button is a #GtkMisc or #GtkAlignment, this property 
   * can be used to control it's vertical alignment. 0.0 is top aligned, 
   * 1.0 is bottom aligned.
   * 
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_YALIGN,
                                   g_param_spec_float("yalign",
                                                      P_("Vertical alignment for child"),
                                                      P_("Vertical position of child in available space. 0.0 is top aligned, 1.0 is bottom aligned"),
                                                      0.0,
                                                      1.0,
                                                      0.5,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));

  /**
   * GtkButton::image:
   * 
   * The child widget to appear next to the button text.
   * 
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_IMAGE,
                                   g_param_spec_object ("image",
                                                        P_("Image widget"),
                                                        P_("Child widget to appear next to the button text"),
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));
#endif

  button_signals[PRESSED] =
    g_signal_new ("pressed",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkButtonClass, pressed),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  button_signals[RELEASED] =
    g_signal_new ("released",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkButtonClass, released),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  button_signals[CLICKED] =
    g_signal_new ("clicked",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkButtonClass, clicked),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  button_signals[ENTER] =
    g_signal_new ("enter",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkButtonClass, enter),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  button_signals[LEAVE] =
    g_signal_new ("leave",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkButtonClass, leave),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * GtkButton::activate:
   * @widget: the object which received the signal.
   *
   * The "activate" signal on GtkButton is an action signal and
   * emitting it causes the button to animate press then release. 
   * Applications should never connect to this signal, but use the
   * "clicked" signal.
   */
  button_signals[ACTIVATE] =
    g_signal_new ("activate",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkButtonClass, activate),
		  NULL, NULL,
		  _gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  widget_class->activate_signal = button_signals[ACTIVATE];

#if 0
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boxed ("default_border",
							       P_("Default Spacing"),
							       P_("Extra space to add for CAN_DEFAULT buttons"),
							       GTK_TYPE_BORDER,
							       G_PARAM_READABLE));

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boxed ("default_outside_border",
							       P_("Default Outside Spacing"),
							       P_("Extra space to add for CAN_DEFAULT buttons that is always drawn outside the border"),
							       GTK_TYPE_BORDER,
							       G_PARAM_READABLE));
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_int ("child_displacement_x",
							     P_("Child X Displacement"),
							     P_("How far in the x direction to move the child when the button is depressed"),
							     G_MININT,
							     G_MAXINT,
							     0,
							     G_PARAM_READABLE));
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_int ("child_displacement_y",
							     P_("Child Y Displacement"),
							     P_("How far in the y direction to move the child when the button is depressed"),
							     G_MININT,
							     G_MAXINT,
							     0,
							     G_PARAM_READABLE));

  /**
   * GtkButton:displace-focus:
   *
   * Whether the child_displacement_x/child_displacement_y properties should also 
   * affect the focus rectangle.
   *
   * Since: 2.6
   */
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boolean ("displace-focus",
								 P_("Displace focus"),
								 P_("Whether the child_displacement_x/_y properties should also affect the focus rectangle"),
						       FALSE,
						       G_PARAM_READABLE));

  gtk_settings_install_property (g_param_spec_boolean ("gtk-button-images",
						       P_("Show button images"),
						       P_("Whether stock icons should be shown in buttons"),
						       TRUE,
						       G_PARAM_READWRITE));
  
#endif
  g_type_class_add_private (gobject_class, sizeof (GtkButtonPrivate));  
}


static void gtk_button_construct_child (GtkButton *button) {

  GtkButtonPrivate *priv = GTK_BUTTON_GET_PRIVATE (button);
#if 0
  GtkStockItem item;
#endif
  GtkWidget *label;
#if 0
  GtkWidget *hbox;
  GtkWidget *align;
#endif
  GtkWidget *image = NULL;
  gchar *label_text = NULL;
  
  if (!button->constructed)
    return;
  
  if (button->label_text == NULL)
    return;

  if (GTK_BIN (button)->child) {
    if (priv->image && !priv->image_is_stock) {
      image = g_object_ref (priv->image);
      if (image->parent)
        gtk_container_remove (GTK_CONTAINER (image->parent), image);
    }

    gtk_container_remove (GTK_CONTAINER (button),
  			    GTK_BIN (button)->child);
  
    priv->image = NULL;
  }
  
#if 0
  if (button->use_stock &&
      gtk_stock_lookup (button->label_text, &item))
    {
      if (!image)
	image = g_object_ref (gtk_image_new_from_stock (button->label_text, GTK_ICON_SIZE_BUTTON));

      label_text = item.label;
    }
  else
#endif
  label_text = button->label_text;

  if (image) {
    WarnOut("gtk_button_construct_child: image not done yet!\n");
#if 0
      label = gtk_label_new_with_mnemonic (label_text);
      gtk_label_set_mnemonic_widget (GTK_LABEL (label), GTK_WIDGET (button));
      
      priv->image = image;

      g_object_set (priv->image, 
		    "visible", show_image (button),
		    "no_show_all", TRUE,
		    NULL);
      hbox = gtk_hbox_new (FALSE, 2);

      if (priv->align_set)
	align = gtk_alignment_new (priv->xalign, priv->yalign, 0.0, 0.0);
      else
	align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
	
      gtk_box_pack_start (GTK_BOX (hbox), priv->image, FALSE, FALSE, 0);
      gtk_box_pack_end (GTK_BOX (hbox), label, FALSE, FALSE, 0);
      
      gtk_container_add (GTK_CONTAINER (button), align);
      gtk_container_add (GTK_CONTAINER (align), hbox);
      gtk_widget_show_all (align);

      g_object_unref (image);

#endif
      return;
    }
  
#if 0
 if (button->use_underline)
    {
      label = gtk_label_new_with_mnemonic (button->label_text);
      gtk_label_set_mnemonic_widget (GTK_LABEL (label), GTK_WIDGET (button));
    }
  else
#endif
    label = gtk_label_new (button->label_text);
  
#if 0
  if (priv->align_set)
    gtk_misc_set_alignment (GTK_MISC (label), priv->xalign, priv->yalign);
#endif

  gtk_widget_show (label);
  gtk_container_add (GTK_CONTAINER (button), label);
}
/**
 * gtk_button_set_label:
 * @button: a #GtkButton
 * @label: a string
 *
 * Sets the text of the label of the button to @str. This text is
 * also used to select the stock item if gtk_button_set_use_stock()
 * is used.
 *
 * This will also clear any previously set labels.
 **/
void gtk_button_set_label (GtkButton *button, const gchar *label) {
  gchar *new_label;

  DebOut("gtk_button_set_label(%lx,%s)\n",button,label);
  
  g_return_if_fail (GTK_IS_BUTTON (button));

  new_label = g_strdup (label);
  g_free (button->label_text);
  button->label_text = new_label;
  
  gtk_button_construct_child (button);
  
  g_object_notify (G_OBJECT (button), "label");
}

static void gtk_button_set_property (GObject         *object,
                         guint            prop_id,
                         const GValue    *value,
                         GParamSpec      *pspec) {

  GtkButton *button = GTK_BUTTON (object);

  DebOut("gtk_button_set_property(%lx,%d,..)\n",object,prop_id);

  switch (prop_id) {
    case PROP_LABEL:
      gtk_button_set_label (button, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void gtk_button_get_property (GObject         *object,
                         guint            prop_id,
                         GValue          *value,
                         GParamSpec      *pspec) {

  GtkButton *button = GTK_BUTTON (object);

  DebOut("gtk_button_get_property(%lx,%d,..)\n",object,prop_id);

  switch (prop_id) {
    case PROP_LABEL:
      g_value_set_string (value, button->label_text);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void gtk_button_init (GtkButton *button) {

  GtkButtonPrivate *priv = GTK_BUTTON_GET_PRIVATE (button);

#if 0
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_FOCUS | GTK_RECEIVES_DEFAULT);
  GTK_WIDGET_SET_FLAGS (button, GTK_NO_WINDOW);
#endif

  button->label_text = NULL;
  
  button->constructed = FALSE;
  button->in_button = FALSE;
  button->button_down = FALSE;
  button->relief = GTK_RELIEF_NORMAL;
  button->use_stock = FALSE;
  button->use_underline = FALSE;
  button->depressed = FALSE;
  button->depress_on_activate = TRUE;
  button->focus_on_click = TRUE;

  priv->xalign = 0.5;
  priv->yalign = 0.5;
  priv->align_set = 0;
  priv->image_is_stock = TRUE;

  GTK_MUI(button)->mainclass=CL_AREA;
}

GtkWidget* gtk_button_new_with_label(const char *label) {
  Object *button;
  GtkWidget *ret;

  DebOut("gtk_button_new_with_label(%s)\n",label);

  button=NULL;

  if(label) {
    ret=g_object_new (GTK_TYPE_BUTTON, "label", label, NULL);
    if(ret) {
      DebOut("  label: %s\n",label);
      /* Note: MUI_MakeObject() uses underscored char for keyboard shortcuts
      **
      ** If this is not desired then we must use taglist way
      */
      button=NewObject(CL_Button->mcc_Class, NULL,MA_Widget,(ULONG) ret,MA_Button_Label,(ULONG) label,TAG_DONE);
    }
  }
  else { /* no label */
    ret=g_object_new (GTK_TYPE_BUTTON, NULL);
    if(ret) {
      button=NewObject(CL_Button->mcc_Class, NULL,MA_Widget,(ULONG) ret,TAG_DONE);
    }
  }

  if((!ret)||(!button)) {
    ErrOut("Unable to create gtk_button!\n");
  }

  DebOut("  gtk_button_new: %lx\n",button);
  GtkSetObj(ret,button);

  return ret;
}

GtkWidget* gtk_button_new (void) {

  DebOut("gtk_button_new()\n");

  return gtk_button_new_with_label(NULL);
}

GtkWidget*  gtk_button_new_from_stock       (const gchar *stock_id){

  DebOut("gtk_button_new_from_stock(%s)\n",stock_id);

  if(!strcmp(stock_id,GTK_STOCK_CLOSE)) {
    return gtk_button_new_with_label("\033cClose");
  }

  ErrOut("Unknown stock object: %s\n",stock_id);
  return NULL;
}

/* Creates a new GtkButton containing a label. 
 * If characters in label are preceded by an underscore, 
 * they are underlined. 
 * If you need a literal underscore character in a label, use '__' (two underscores). 
 * The first underlined character represents a keyboard accelerator called a 
 * mnemonic. Pressing Alt and that key activates the button.
 *
 * o1i: hmm, should be pretty MUI compatible..
 */
GtkWidget *gtk_button_new_with_mnemonic(const gchar *label) {
  DebOut("gtk_button_new_with_mnemonic(%s)\n",label);

  return gtk_button_new_with_label(label);
}
