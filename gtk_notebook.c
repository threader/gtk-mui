/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005, 2006 Oliver Brunner
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
 * $Id: gtk_notebook.c,v 1.10 2007/10/25 15:22:19 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "gtk.h"
#include "gtk/gtknotebook.h"
#include "gtk/gtkmarshalers.h"

#include "debug.h"
#include "gtk_globals.h"
#include "classes/classes.h"

#define TAB_OVERLAP    2
#define TAB_CURVATURE  1
#define ARROW_SIZE     12
#define ARROW_SPACING  0
#define NOTEBOOK_INIT_SCROLL_DELAY (200)
#define NOTEBOOK_SCROLL_DELAY      (100)

enum {
  SWITCH_PAGE,
  FOCUS_TAB,
  SELECT_PAGE,
  CHANGE_CURRENT_PAGE,
  MOVE_FOCUS_OUT,
  LAST_SIGNAL
};

enum {
  STEP_PREV,
  STEP_NEXT
};

typedef enum
{
  ARROW_NONE,
  ARROW_LEFT_BEFORE,
  ARROW_RIGHT_BEFORE,
  ARROW_LEFT_AFTER,
  ARROW_RIGHT_AFTER
} GtkNotebookArrow;

#define ARROW_IS_LEFT(arrow)  ((arrow) == ARROW_LEFT_BEFORE || (arrow) == ARROW_LEFT_AFTER)
#define ARROW_IS_BEFORE(arrow) ((arrow) == ARROW_LEFT_BEFORE || (arrow) == ARROW_RIGHT_BEFORE)

enum {
  PROP_0,
  PROP_TAB_POS,
  PROP_SHOW_TABS,
  PROP_SHOW_BORDER,
  PROP_SCROLLABLE,
  PROP_TAB_BORDER,
  PROP_TAB_HBORDER,
  PROP_TAB_VBORDER,
  PROP_PAGE,
  PROP_ENABLE_POPUP,
  PROP_HOMOGENEOUS
};

enum {
  CHILD_PROP_0,
  CHILD_PROP_TAB_LABEL,
  CHILD_PROP_MENU_LABEL,
  CHILD_PROP_POSITION,
  CHILD_PROP_TAB_EXPAND,
  CHILD_PROP_TAB_FILL,
  CHILD_PROP_TAB_PACK
};

#define GTK_NOTEBOOK_PAGE(_glist_)         ((GtkNotebookPage *)((GList *)(_glist_))->data)

struct _GtkNotebookPage
{
  GtkWidget *child;
  GtkWidget *tab_label;
  GtkWidget *menu_label;
  GtkWidget *last_focus_child;	/* Last descendant of the page that had focus */

  guint default_menu : 1;	/* If true, we create the menu label ourself */
  guint default_tab  : 1;	/* If true, we create the tab label ourself */
  guint expand       : 1;
  guint fill         : 1;
  guint pack         : 1;

  GtkRequisition requisition;
  GtkAllocation allocation;

  guint mnemonic_activate_signal;
};

/*** GtkNotebook Methods ***/
static void gtk_notebook_class_init          (GtkNotebookClass *klass);
static void gtk_notebook_init                (GtkNotebook      *notebook);

#if 0
static gboolean gtk_notebook_select_page         (GtkNotebook      *notebook,
						  gboolean          move_focus);
static gboolean gtk_notebook_focus_tab           (GtkNotebook      *notebook,
						  GtkNotebookTab    type);
static void     gtk_notebook_change_current_page (GtkNotebook      *notebook,
						  gint              offset);
static void     gtk_notebook_move_focus_out      (GtkNotebook      *notebook,
						  GtkDirectionType  direction_type);
#endif

/*** GtkObject Methods ***/
#if 0
static void gtk_notebook_destroy             (GtkObject        *object);
#endif
static void gtk_notebook_set_property	     (GObject         *object,
					      guint            prop_id,
					      const GValue    *value,
					      GParamSpec      *pspec);
static void gtk_notebook_get_property	     (GObject         *object,
					      guint            prop_id,
					      GValue          *value,
					      GParamSpec      *pspec);

static void gtk_notebook_add                 (GtkContainer     *container,
					      GtkWidget        *widget);

static void gtk_notebook_forall              (GtkContainer     *container,
					      gboolean		include_internals,
					      GtkCallback       callback,
					      gpointer          callback_data);

static GtkContainerClass *parent_class = NULL;
static guint notebook_signals[LAST_SIGNAL] = { 0 };


static void gtk_notebook_class_init (GtkNotebookClass *class) {

  GObjectClass   *gobject_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
#if 0
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (class);
#endif
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (class);
#if 0
  GtkBindingSet *binding_set;
#endif

  DebOut("gtk_notebook_class_init(%lx)\n",class);
  
  parent_class = g_type_class_peek_parent (class);

  gobject_class->set_property = gtk_notebook_set_property;
  gobject_class->get_property = gtk_notebook_get_property;
#if 0
  object_class->destroy = gtk_notebook_destroy;
#endif

#if 0
  widget_class->map = gtk_notebook_map;
  widget_class->unmap = gtk_notebook_unmap;
  widget_class->realize = gtk_notebook_realize;
  widget_class->unrealize = gtk_notebook_unrealize;
  widget_class->size_request = gtk_notebook_size_request;
  widget_class->size_allocate = gtk_notebook_size_allocate;
  widget_class->expose_event = gtk_notebook_expose;
  widget_class->scroll_event = gtk_notebook_scroll;
  widget_class->button_press_event = gtk_notebook_button_press;
  widget_class->button_release_event = gtk_notebook_button_release;
  widget_class->enter_notify_event = gtk_notebook_enter_notify;
  widget_class->leave_notify_event = gtk_notebook_leave_notify;
  widget_class->motion_notify_event = gtk_notebook_motion_notify;
  widget_class->grab_notify = gtk_notebook_grab_notify;
  widget_class->state_changed = gtk_notebook_state_changed;
  widget_class->focus_in_event = gtk_notebook_focus_in;
  widget_class->focus_out_event = gtk_notebook_focus_out;
  widget_class->focus = gtk_notebook_focus;
  widget_class->style_set = gtk_notebook_style_set;
  
#endif
  container_class->add = gtk_notebook_add;
#if 0
  container_class->remove = gtk_notebook_remove;
#endif
  container_class->forall = gtk_notebook_forall;
#if 0
  container_class->set_focus_child = gtk_notebook_set_focus_child;
  container_class->get_child_property = gtk_notebook_get_child_property;
  container_class->set_child_property = gtk_notebook_set_child_property;
  container_class->child_type = gtk_notebook_child_type;

  class->switch_page = gtk_notebook_real_switch_page;

  class->focus_tab = gtk_notebook_focus_tab;
  class->select_page = gtk_notebook_select_page;
  class->change_current_page = gtk_notebook_change_current_page;
  class->move_focus_out = gtk_notebook_move_focus_out;
#endif
  
  g_object_class_install_property (gobject_class,
				   PROP_PAGE,
				   g_param_spec_int ("page",
 						     P_("Page"),
 						     P_("The index of the current page"),
 						     0,
 						     G_MAXINT,
 						     0,
 						     G_PARAM_READWRITE));
#if 0
  g_object_class_install_property (gobject_class,
				   PROP_TAB_POS,
				   g_param_spec_enum ("tab_pos",
 						      P_("Tab Position"),
 						      P_("Which side of the notebook holds the tabs"),
 						      GTK_TYPE_POSITION_TYPE,
 						      GTK_POS_TOP,
 						      G_PARAM_READWRITE));
#endif
  g_object_class_install_property (gobject_class,
				   PROP_TAB_BORDER,
				   g_param_spec_uint ("tab_border",
 						      P_("Tab Border"),
 						      P_("Width of the border around the tab labels"),
 						      0,
 						      G_MAXUINT,
 						      2,
 						      G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
				   PROP_TAB_HBORDER,
				   g_param_spec_uint ("tab_hborder",
 						      P_("Horizontal Tab Border"),
 						      P_("Width of the horizontal border of tab labels"),
 						      0,
 						      G_MAXUINT,
 						      2,
 						      G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_TAB_VBORDER,
				   g_param_spec_uint ("tab_vborder",
 						      P_("Vertical Tab Border"),
 						      P_("Width of the vertical border of tab labels"),
 						      0,
 						      G_MAXUINT,
 						      2,
 						      G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_SHOW_TABS,
				   g_param_spec_boolean ("show_tabs",
 							 P_("Show Tabs"),
 							 P_("Whether tabs should be shown or not"),
 							 TRUE,
 							 G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_SHOW_BORDER,
				   g_param_spec_boolean ("show_border",
 							 P_("Show Border"),
 							 P_("Whether the border should be shown or not"),
 							 TRUE,
 							 G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_SCROLLABLE,
				   g_param_spec_boolean ("scrollable",
 							 P_("Scrollable"),
 							 P_("If TRUE, scroll arrows are added if there are too many tabs to fit"),
 							 FALSE,
 							 G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_ENABLE_POPUP,
				   g_param_spec_boolean ("enable_popup",
 							 P_("Enable Popup"),
 							 P_("If TRUE, pressing the right mouse button on the notebook pops up a menu that you can use to go to a page"),
 							 FALSE,
 							 G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
				   PROP_HOMOGENEOUS,
				   g_param_spec_boolean ("homogeneous",
 							 P_("Homogeneous"),
 							 P_("Whether tabs should have homogeneous sizes"),
 							 FALSE,
							 G_PARAM_READWRITE));

#if 0
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_TAB_LABEL,
					      g_param_spec_string ("tab_label", 
								   P_("Tab label"),
								   P_("The string displayed on the child's tab label"),
								   NULL,
								   G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_MENU_LABEL,
					      g_param_spec_string ("menu_label", 
								   P_("Menu label"), 
								   P_("The string displayed in the child's menu entry"),
								   NULL,
								   G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_POSITION,
					      g_param_spec_int ("position", 
								P_("Position"), 
								P_("The index of the child in the parent"),
								-1, G_MAXINT, 0,
								G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_TAB_EXPAND,
					      g_param_spec_boolean ("tab_expand", 
								    P_("Tab expand"), 
								    P_("Whether to expand the child's tab or not"),
								    TRUE,
								    G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_TAB_FILL,
					      g_param_spec_boolean ("tab_fill", 
								    P_("Tab fill"), 
								    P_("Whether the child's tab should fill the allocated area or not"),
								    TRUE,
								    G_PARAM_READWRITE));
#endif
#if 0
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_TAB_PACK,
					      g_param_spec_enum ("tab_pack", 
								 P_("Tab pack type"),
								 P_("A GtkPackType indicating whether the child is packed with reference to the start or end of the parent"),
								 GTK_TYPE_PACK_TYPE, GTK_PACK_START,
								 G_PARAM_READWRITE));
#endif

/**
 * GtkNotebook:has-secondary-backward-stepper:
 *
 * The "has-secondary-backward-stepper" property determines whether 
 * a second backward arrow button is displayed on the opposite end 
 * of the tab area.
 *
 * Since: 2.4
 */  
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boolean ("has_secondary_backward_stepper",
								 P_("Secondary backward stepper"),
								 P_("Display a second backward arrow button on the opposite end of the tab area"),
								 FALSE,
								 
								 G_PARAM_READABLE));

/**
 * GtkNotebook:has-secondary-forward-stepper:
 *
 * The "has-secondary-forward-stepper" property determines whether 
 * a second forward arrow button is displayed on the opposite end 
 * of the tab area.
 *
 * Since: 2.4
 */  
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boolean ("has_secondary_forward_stepper",
								 P_("Secondary forward stepper"),
								 P_("Display a second forward arrow button on the opposite end of the tab area"),
								 FALSE,
								 
								 G_PARAM_READABLE));

/**
 * GtkNotebook:has-backward-stepper:
 *
 * The "has-backward-stepper" property determines whether 
 * the standard backward arrow button is displayed.
 *
 * Since: 2.4
 */  
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boolean ("has_backward_stepper",
								 P_("Backward stepper"),
								 P_("Display the standard backward arrow button"),
								 TRUE,
								 
                                                                   G_PARAM_READABLE));

/**
 * GtkNotebook:has-forward-stepper:
 *
 * The "has-forward-stepper" property determines whether 
 * the standard forward arrow button is displayed.
 *
 * Since: 2.4
 */  
  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_boolean ("has_forward_stepper",
								 P_("Forward stepper"),
								 P_("Display the standard forward arrow button"),
								 TRUE,
                 G_PARAM_READABLE));

  notebook_signals[SWITCH_PAGE] =
    g_signal_new ("switch_page",
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtkNotebookClass, switch_page),
		  NULL, NULL,
		  _gtk_marshal_VOID__POINTER_UINT,
		  G_TYPE_NONE, 2,
		  G_TYPE_POINTER,
		  G_TYPE_UINT);
#if 0
  notebook_signals[FOCUS_TAB] = 
    g_signal_new ("focus_tab",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkNotebookClass, focus_tab),
                  NULL, NULL,
                  _gtk_marshal_BOOLEAN__ENUM,
                  G_TYPE_BOOLEAN, 1,
                  GTK_TYPE_NOTEBOOK_TAB);
#endif
  notebook_signals[SELECT_PAGE] = 
    g_signal_new ("select_page",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkNotebookClass, select_page),
                  NULL, NULL,
                  _gtk_marshal_BOOLEAN__BOOLEAN,
                  G_TYPE_BOOLEAN, 1,
                  G_TYPE_BOOLEAN);
  notebook_signals[CHANGE_CURRENT_PAGE] = 
    g_signal_new ("change_current_page",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkNotebookClass, change_current_page),
                  NULL, NULL,
                  _gtk_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);
#if 0
  notebook_signals[MOVE_FOCUS_OUT] =
    g_signal_new ("move_focus_out",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkNotebookClass, move_focus_out),
                  NULL, NULL,
                  _gtk_marshal_VOID__ENUM,
                  G_TYPE_NONE, 1,
                  GTK_TYPE_DIRECTION_TYPE);
#endif
  
  
#if 0
  binding_set = gtk_binding_set_by_class (class);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_space, 0,
                                "select_page", 1, 
                                G_TYPE_BOOLEAN, FALSE);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_KP_Space, 0,
                                "select_page", 1, 
                                G_TYPE_BOOLEAN, FALSE);
  
  gtk_binding_entry_add_signal (binding_set,
                                GDK_Home, 0,
                                "focus_tab", 1, 
                                GTK_TYPE_NOTEBOOK_TAB, GTK_NOTEBOOK_TAB_FIRST);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_KP_Home, 0,
                                "focus_tab", 1, 
                                GTK_TYPE_NOTEBOOK_TAB, GTK_NOTEBOOK_TAB_FIRST);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_End, 0,
                                "focus_tab", 1, 
                                GTK_TYPE_NOTEBOOK_TAB, GTK_NOTEBOOK_TAB_LAST);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_KP_End, 0,
                                "focus_tab", 1, 
                                GTK_TYPE_NOTEBOOK_TAB, GTK_NOTEBOOK_TAB_LAST);

  gtk_binding_entry_add_signal (binding_set,
                                GDK_Page_Up, GDK_CONTROL_MASK,
                                "change_current_page", 1,
                                G_TYPE_INT, -1);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_Page_Down, GDK_CONTROL_MASK,
                                "change_current_page", 1,
                                G_TYPE_INT, 1);

  gtk_binding_entry_add_signal (binding_set,
                                GDK_Page_Up, GDK_CONTROL_MASK | GDK_MOD1_MASK,
                                "change_current_page", 1,
                                G_TYPE_INT, -1);
  gtk_binding_entry_add_signal (binding_set,
                                GDK_Page_Down, GDK_CONTROL_MASK | GDK_MOD1_MASK,
                                "change_current_page", 1,
                                G_TYPE_INT, 1);

  add_arrow_bindings (binding_set, GDK_Up, GTK_DIR_UP);
  add_arrow_bindings (binding_set, GDK_Down, GTK_DIR_DOWN);
  add_arrow_bindings (binding_set, GDK_Left, GTK_DIR_LEFT);
  add_arrow_bindings (binding_set, GDK_Right, GTK_DIR_RIGHT);

  add_tab_bindings (binding_set, GDK_CONTROL_MASK, GTK_DIR_TAB_FORWARD);
  add_tab_bindings (binding_set, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_DIR_TAB_BACKWARD);
#endif
}

static void gtk_notebook_init (GtkNotebook *notebook) {

  DebOut("gtk_notebook_init(%lx)\n",notebook);

  GTK_WIDGET_SET_FLAGS (notebook, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS (notebook, GTK_NO_WINDOW);

  notebook->cur_page = NULL;
  notebook->children = NULL;
  notebook->first_tab = NULL;
  notebook->focus_tab = NULL;
  notebook->event_window = NULL;
  notebook->menu = NULL;

  notebook->tab_hborder = 2;
  notebook->tab_vborder = 2;

  notebook->show_tabs = TRUE;
  notebook->show_border = TRUE;
  notebook->tab_pos = GTK_POS_TOP;
  notebook->scrollable = FALSE;
  notebook->in_child = 0;
  notebook->click_child = 0;
  notebook->button = 0;
  notebook->need_timer = 0;
  notebook->child_has_focus = FALSE;
  notebook->have_visible_child = FALSE;
  notebook->focus_out = FALSE;

  notebook->has_before_previous = 1;
  notebook->has_before_next     = 0;
  notebook->has_after_previous  = 0;
  notebook->has_after_next      = 1;

  GTK_MUI(notebook)->mainclass=CL_AREA;
}

GType gtk_notebook_get_type (void) {

  static GType notebook_type = 0;

  if (!notebook_type) { 
    static const GTypeInfo notebook_info = {
      sizeof (GtkNotebookClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_notebook_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkNotebook),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_notebook_init,
    };

    notebook_type = g_type_register_static (GTK_TYPE_CONTAINER, "GtkNotebook",
					      &notebook_info, 0);
  }
  return notebook_type;
}

static void gtk_notebook_set_property (GObject         *object,
			   guint            prop_id,
			   const GValue    *value,
			   GParamSpec      *pspec) {

  GtkNotebook *notebook;

  DebOut("gtk_notebook_set_property(%lx,%d,..)\n",object,prop_id);

  notebook = GTK_NOTEBOOK (object);

  switch (prop_id) {
#if 0
    case PROP_SHOW_TABS:
      gtk_notebook_set_show_tabs (notebook, g_value_get_boolean (value));
      break;
    case PROP_SHOW_BORDER:
      gtk_notebook_set_show_border (notebook, g_value_get_boolean (value));
      break;
    case PROP_SCROLLABLE:
      gtk_notebook_set_scrollable (notebook, g_value_get_boolean (value));
      break;
    case PROP_ENABLE_POPUP:
      if (g_value_get_boolean (value))
        gtk_notebook_popup_enable (notebook);
      else
        gtk_notebook_popup_disable (notebook);
      break;
    case PROP_HOMOGENEOUS:
      gtk_notebook_set_homogeneous_tabs_internal (notebook, g_value_get_boolean (value));
      break;  
    case PROP_PAGE:
      gtk_notebook_set_current_page (notebook, g_value_get_int (value));
      break;
    case PROP_TAB_POS:
      gtk_notebook_set_tab_pos (notebook, g_value_get_enum (value));
      break;
    case PROP_TAB_BORDER:
      gtk_notebook_set_tab_border_internal (notebook, g_value_get_uint (value));
      break;
    case PROP_TAB_HBORDER:
      gtk_notebook_set_tab_hborder_internal (notebook, g_value_get_uint (value));
      break;
    case PROP_TAB_VBORDER:
      gtk_notebook_set_tab_vborder_internal (notebook, g_value_get_uint (value));
      break;
#endif
    default:
      break;
  }
}

static void gtk_notebook_get_property (GObject         *object,
			   guint            prop_id,
			   GValue          *value,
			   GParamSpec      *pspec) {

  GtkNotebook *notebook;

  DebOut("gtk_notebook_get_property(%lx,%d,..)\n",object,prop_id);

  notebook = GTK_NOTEBOOK (object);

  switch (prop_id) {
#if 0
    case PROP_SHOW_TABS:
      g_value_set_boolean (value, notebook->show_tabs);
      break;
    case PROP_SHOW_BORDER:
      g_value_set_boolean (value, notebook->show_border);
      break;
    case PROP_SCROLLABLE:
      g_value_set_boolean (value, notebook->scrollable);
      break;
    case PROP_ENABLE_POPUP:
      g_value_set_boolean (value, notebook->menu != NULL);
      break;
    case PROP_HOMOGENEOUS:
      g_value_set_boolean (value, notebook->homogeneous);
      break;
    case PROP_PAGE:
      g_value_set_int (value, gtk_notebook_get_current_page (notebook));
      break;
    case PROP_TAB_POS:
      g_value_set_enum (value, notebook->tab_pos);
      break;
    case PROP_TAB_HBORDER:
      g_value_set_uint (value, notebook->tab_hborder);
      break;
    case PROP_TAB_VBORDER:
      g_value_set_uint (value, notebook->tab_vborder);
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

GtkWidget* gtk_notebook_new(void) {

  GtkWidget *ret;
  Object *note;

  DebOut("gtk_notebook_new()\n");

  ret=g_object_new (GTK_TYPE_NOTEBOOK, NULL);

  note = (APTR) NewObject(CL_NoteBook->mcc_Class, NULL,MA_Widget,(ULONG) ret,TAG_DONE);

  if((!ret)||(!note)) {
    ErrOut("gtk_notebook_new: unable to create new notebook\n");
    return ret;
  }

  DebOut(" NewObject=%lx\n",note);

  GtkSetObj(ret, note);

  return ret;
}

gint gtk_notebook_append_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label) {

  gint nchildren;
  g_return_val_if_fail (GTK_IS_NOTEBOOK (notebook), -1);
  g_return_val_if_fail (GTK_IS_WIDGET (child), -1);

  DebOut("gtk_notebook_append_page(%lx,%lx,%lx)\n",notebook,child,tab_label);
  if(tab_label) {
    DebOut("  label=%s\n",GTK_LABEL(tab_label)->text);
  }

  mgtk_add_child(GTK_WIDGET(notebook),child);

#warning we should have GtkNotebookPage, here!

  nchildren = g_list_length (notebook->children);
  notebook->children = g_list_insert (notebook->children, child, nchildren);

  return DoMethod(GtkObj(notebook), MM_NoteBook_AppendChild, (ULONG) child, (ULONG) tab_label);
}

gint gtk_notebook_prepend_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label) {

  DebOut("gtk_notebook_prepend_page(%lx,%lx,%lx)\n",notebook,child,tab_label);
  if(tab_label) {
    DebOut("  label=%s\n",GTK_LABEL(tab_label)->text);
  }

  mgtk_add_child(GTK_WIDGET(notebook),child);

  notebook->children = g_list_insert (notebook->children, child, 0);

  return DoMethod(GtkObj(notebook), MM_NoteBook_PrependChild, (ULONG) child, (ULONG) tab_label);
}

void gtk_notebook_next_page(GtkNotebook *notebook) {
  DebOut("gtk_notebook_next_page(%lx)\n",notebook);

  set(GtkObj(notebook),MUIA_Group_ActivePage,MUIV_Group_ActivePage_Next);
}

void gtk_notebook_prev_page(GtkNotebook *notebook) {
  DebOut("gtk_notebook_prev_page(%lx)\n",notebook);

  set(GtkObj(notebook),MUIA_Group_ActivePage,MUIV_Group_ActivePage_Prev);
}

/* page_num:   
 *  index of the page to switch to, starting from 0. 
 *  If negative, the last page will be used. 
 *  If greater than the number of pages in the notebook, nothing will be done.
 */
void gtk_notebook_set_current_page(GtkNotebook *notebook, gint page_num) {
  int nr;

  DebOut("gtk_notebook_set_current_page(%lx,%d)\n",notebook,page_num);

  nr=xget(GtkObj(notebook),MA_Note_NrChilds);

  if(page_num<0) {
    page_num=nr;
  }

  if(page_num > nr) {
    return;
  }

  set(GtkObj(notebook),MUIA_Group_ActivePage,page_num);
}


/**
 * gtk_notebook_set_show_tabs:
 * @notebook: a #GtkNotebook
 * @show_tabs: %TRUE if the tabs should be shown.
 * 
 * Sets whether to show the tabs for the notebook or not.
 **/
void gtk_notebook_set_show_tabs (GtkNotebook *notebook, gboolean     show_tabs) {

  DebOut("gtk_notebook_set_show_tabs(%lx,%d)\n",notebook,show_tabs);

  g_return_if_fail (GTK_IS_NOTEBOOK (notebook));

  show_tabs = show_tabs != FALSE;

  if (notebook->show_tabs == show_tabs)
    return;

  notebook->show_tabs = show_tabs;

  set(GtkObj(notebook),MA_Show_Label,show_tabs);
}


static void
gtk_notebook_forall (GtkContainer *container,
		     gboolean      include_internals,
		     GtkCallback   callback,
		     gpointer      callback_data)
{
  GtkNotebook *notebook;
  GList *children;

  g_return_if_fail (GTK_IS_NOTEBOOK (container));
  g_return_if_fail (callback != NULL);

  notebook = GTK_NOTEBOOK (container);

  children = notebook->children;
  while (children)
    {
      (* callback) (children->data, callback_data);
      children = children->next;
#if 0
      GtkNotebookPage *page;
      
      page = children->data;
      children = children->next;
      (* callback) (page->child, callback_data);

      if (include_internals)
	{
	  if (page->tab_label)
	    (* callback) (page->tab_label, callback_data);
	}
#endif
    }
}

static void
gtk_notebook_add (GtkContainer *container,
		  GtkWidget    *widget)
{
  g_return_if_fail (GTK_IS_NOTEBOOK (container));

#if 0
  gtk_notebook_insert_page_menu (GTK_NOTEBOOK (container), widget, 
				 NULL, NULL, -1);
#endif

  DebOut("gtk_notebook_add(%lx,%lx): do nothing\n",container,widget);
}

