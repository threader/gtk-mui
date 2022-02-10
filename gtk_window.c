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
 * $Id: gtk_window.c,v 1.28 2012/07/05 12:44:36 o1i Exp $
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


#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>

//#define MGTK_DEBUG 1
#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

#ifndef _gtk_marshal_VOID__VOID
#define _gtk_marshal_VOID__VOID g_cclosure_marshal_VOID__VOID
#endif

enum {
  SET_FOCUS,
  FRAME_EVENT,
  ACTIVATE_FOCUS,
  ACTIVATE_DEFAULT,
  MOVE_FOCUS,
  KEYS_CHANGED,
  LAST_SIGNAL
};

enum {
  PROP_0,

  /* Construct */
  PROP_TYPE,

  /* Normal Props */
  PROP_TITLE,
  PROP_ROLE,
  PROP_ALLOW_SHRINK,
  PROP_ALLOW_GROW,
  PROP_RESIZABLE,
  PROP_MODAL,
  PROP_WIN_POS,
  PROP_DEFAULT_WIDTH,
  PROP_DEFAULT_HEIGHT,
  PROP_DESTROY_WITH_PARENT,
  PROP_ICON,
  PROP_ICON_NAME,
  PROP_SCREEN,
  PROP_TYPE_HINT,
  PROP_SKIP_TASKBAR_HINT,
  PROP_SKIP_PAGER_HINT,
  PROP_ACCEPT_FOCUS,
  PROP_FOCUS_ON_MAP,
  PROP_DECORATED,
  PROP_GRAVITY,
  
  /* Readonly properties */
  PROP_IS_ACTIVE,
  PROP_HAS_TOPLEVEL_FOCUS,
  
  LAST_ARG
};
static void  gtk_window_class_init (GtkWindowClass  *klass);
static void  gtk_window_show       (GtkWidget       *widget);
static void  gtk_window_hide       (GtkWidget       *widget);

static guint window_signals[LAST_SIGNAL] = { 0 };
static GtkBinClass *parent_class = NULL;


static void gtk_window_set_property (GObject         *object,
				     guint            prop_id,
				     const GValue    *value,
				     GParamSpec      *pspec);
static void gtk_window_get_property (GObject         *object,
				     guint            prop_id,
				     GValue          *value,
				     GParamSpec      *pspec);


static void gtk_window_class_init (GtkWindowClass *klass) {

  DebOut("gtk_window_class_init(%lx)\n",klass);

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;
#if 0
  GtkBindingSet *binding_set;
#endif
  
  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;
  container_class = (GtkContainerClass*) klass;
  
  parent_class = g_type_class_peek_parent (klass);

#if 0
  gobject_class->dispose = gtk_window_dispose;
  gobject_class->finalize = gtk_window_finalize;
#endif

  gobject_class->set_property = gtk_window_set_property;
  gobject_class->get_property = gtk_window_get_property;
  
#if 0
  object_class->destroy = gtk_window_destroy;

#endif
  widget_class->show = gtk_window_show;
  widget_class->hide = gtk_window_hide;
#if 0
  widget_class->map = gtk_window_map;
  widget_class->unmap = gtk_window_unmap;
  widget_class->realize = gtk_window_realize;
  widget_class->unrealize = gtk_window_unrealize;
  widget_class->size_request = gtk_window_size_request;
  widget_class->size_allocate = gtk_window_size_allocate;
  widget_class->configure_event = gtk_window_configure_event;
  widget_class->key_press_event = gtk_window_key_press_event;
  widget_class->key_release_event = gtk_window_key_release_event;
  widget_class->enter_notify_event = gtk_window_enter_notify_event;
  widget_class->leave_notify_event = gtk_window_leave_notify_event;
  widget_class->focus_in_event = gtk_window_focus_in_event;
  widget_class->focus_out_event = gtk_window_focus_out_event;
  widget_class->client_event = gtk_window_client_event;
  widget_class->focus = gtk_window_focus;
  
  widget_class->expose_event = gtk_window_expose;
   
  container_class->check_resize = gtk_window_check_resize;

  klass->set_focus = gtk_window_real_set_focus;
  klass->frame_event = gtk_window_frame_event;

  klass->activate_default = gtk_window_real_activate_default;
  klass->activate_focus = gtk_window_real_activate_focus;
  klass->move_focus = gtk_window_move_focus;
  klass->keys_changed = gtk_window_keys_changed;
  
  g_type_class_add_private (gobject_class, sizeof (GtkWindowPrivate));
  
#endif
  /* Construct */
  g_object_class_install_property (gobject_class,
                                   PROP_TYPE,
                                   g_param_spec_enum ("type",
						      P_("Window Type"),
						      P_("The type of the window"),
						      GTK_TYPE_WINDOW_TYPE,
						      GTK_WINDOW_TOPLEVEL,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
#if 0
  /* Regular Props */
  g_object_class_install_property (gobject_class,
                                   PROP_TITLE,
                                   g_param_spec_string ("title",
                                                        P_("Window Title"),
                                                        P_("The title of the window"),
                                                        NULL,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_ROLE,
                                   g_param_spec_string ("role",
							P_("Window Role"),
							P_("Unique identifier for the window to be used when restoring a session"),
							NULL,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_ALLOW_SHRINK,
                                   g_param_spec_boolean ("allow_shrink",
							 P_("Allow Shrink"),
							 /* xgettext:no-c-format */
							 P_("If TRUE, the window has no mimimum size. Setting this to TRUE is 99% of the time a bad idea"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_ALLOW_GROW,
                                   g_param_spec_boolean ("allow_grow",
							 P_("Allow Grow"),
							 P_("If TRUE, users can expand the window beyond its minimum size"),
							 TRUE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_RESIZABLE,
                                   g_param_spec_boolean ("resizable",
							 P_("Resizable"),
							 P_("If TRUE, users can resize the window"),
							 TRUE,
							 G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_MODAL,
                                   g_param_spec_boolean ("modal",
							 P_("Modal"),
							 P_("If TRUE, the window is modal (other windows are not usable while this one is up)"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_WIN_POS,
                                   g_param_spec_enum ("window_position",
						      P_("Window Position"),
						      P_("The initial position of the window"),
						      GTK_TYPE_WINDOW_POSITION,
						      GTK_WIN_POS_NONE,
						      G_PARAM_READWRITE));
 
  g_object_class_install_property (gobject_class,
                                   PROP_DEFAULT_WIDTH,
                                   g_param_spec_int ("default_width",
						     P_("Default Width"),
						     P_("The default width of the window, used when initially showing the window"),
						     -1,
						     G_MAXINT,
						     -1,
						     G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_DEFAULT_HEIGHT,
                                   g_param_spec_int ("default_height",
						     P_("Default Height"),
						     P_("The default height of the window, used when initially showing the window"),
						     -1,
						     G_MAXINT,
						     -1,
						     G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_DESTROY_WITH_PARENT,
                                   g_param_spec_boolean ("destroy_with_parent",
							 P_("Destroy with Parent"),
							 P_("If this window should be destroyed when the parent is destroyed"),
                                                         FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_ICON,
                                   g_param_spec_object ("icon",
                                                        P_("Icon"),
                                                        P_("Icon for this window"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_READWRITE));
  
  /**
   * GtkWindow:icon-name:
   *
   * The :icon-name property specifies the name of the themed icon to
   * use as the window icon. See #GtkIconTheme for more details.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ICON_NAME,
                                   g_param_spec_string ("icon_name",
                                                        P_("Icon Name"),
                                                        P_("Name of the themed icon for this window"),
							NULL,
                                                        G_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
				   PROP_SCREEN,
				   g_param_spec_object ("screen",
 							P_("Screen"),
 							P_("The screen where this window will be displayed"),
							GDK_TYPE_SCREEN,
 							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_IS_ACTIVE,
                                   g_param_spec_boolean ("is_active",
							 P_("Is Active"),
							 P_("Whether the toplevel is the current active window"),
							 FALSE,
							 G_PARAM_READABLE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_HAS_TOPLEVEL_FOCUS,
                                   g_param_spec_boolean ("has_toplevel_focus",
							 P_("Focus in Toplevel"),
							 P_("Whether the input focus is within this GtkWindow"),
							 FALSE,
							 G_PARAM_READABLE));

  g_object_class_install_property (gobject_class,
				   PROP_TYPE_HINT,
				   g_param_spec_enum ("type_hint",
                                                      P_("Type hint"),
                                                      P_("Hint to help the desktop environment understand what kind of window this is and how to treat it."),
                                                      GDK_TYPE_WINDOW_TYPE_HINT,
                                                      GDK_WINDOW_TYPE_HINT_NORMAL,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_SKIP_TASKBAR_HINT,
				   g_param_spec_boolean ("skip_taskbar_hint",
                                                         P_("Skip taskbar"),
                                                         P_("TRUE if the window should not be in the task bar."),
                                                         FALSE,
                                                         G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
				   PROP_SKIP_PAGER_HINT,
				   g_param_spec_boolean ("skip_pager_hint",
                                                         P_("Skip pager"),
                                                         P_("TRUE if the window should not be in the pager."),
                                                         FALSE,
                                                         G_PARAM_READWRITE));  

  /**
   * GtkWindow:accept-focus-hint:
   *
   * Whether the window should receive the input focus.
   *
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
				   PROP_ACCEPT_FOCUS,
				   g_param_spec_boolean ("accept_focus",
                                                         P_("Accept focus"),
                                                         P_("TRUE if the window should receive the input focus."),
                                                         TRUE,
                                                         G_PARAM_READWRITE));  

  /**
   * GtkWindow:focus-on-map-hint:
   *
   * Whether the window should receive the input focus when mapped.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_FOCUS_ON_MAP,
				   g_param_spec_boolean ("focus_on_map",
                                                         P_("Focus on map"),
                                                         P_("TRUE if the window should receive the input focus when mapped."),
                                                         TRUE,
                                                         G_PARAM_READWRITE));  

  /**
   * GtkWindow:decorated:
   *
   * Whether the window should be decorated by the window manager.
   *
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_DECORATED,
                                   g_param_spec_boolean ("decorated",
							 P_("Decorated"),
							 P_("Whether the window should be decorated by the window manager"),
							 TRUE,
							 G_PARAM_READWRITE));

  /**
   * GtkWindow:gravity:
   *
   * The window gravity of the window. See gtk_window_move() and #GdkGravity for
   * more details about window gravity.
   *
   * Since: 2.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_GRAVITY,
                                   g_param_spec_enum ("gravity",
						      P_("Gravity"),
						      P_("The window gravity of the window"),
						      GDK_TYPE_GRAVITY,
						      GDK_GRAVITY_NORTH_WEST,
						      G_PARAM_READWRITE));


  window_signals[SET_FOCUS] =
    g_signal_new ("set_focus",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtkWindowClass, set_focus),
                  NULL, NULL,
                  _gtk_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  GTK_TYPE_WIDGET);
  
  window_signals[FRAME_EVENT] =
    g_signal_new ("frame_event",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET(GtkWindowClass, frame_event),
                  _gtk_boolean_handled_accumulator, NULL,
                  _gtk_marshal_BOOLEAN__BOXED,
                  G_TYPE_BOOLEAN, 1,
                  GDK_TYPE_EVENT);

  window_signals[ACTIVATE_FOCUS] =
    g_signal_new ("activate_focus",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkWindowClass, activate_focus),
                  NULL, NULL,
                  _gtk_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  window_signals[ACTIVATE_DEFAULT] =
    g_signal_new ("activate_default",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkWindowClass, activate_default),
                  NULL, NULL,
                  _gtk_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  window_signals[MOVE_FOCUS] =
    g_signal_new ("move_focus",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkWindowClass, move_focus),
                  NULL, NULL,
                  _gtk_marshal_VOID__ENUM,
                  G_TYPE_NONE,
                  1,
                  GTK_TYPE_DIRECTION_TYPE);

  window_signals[KEYS_CHANGED] =
    g_signal_new ("keys_changed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GtkWindowClass, keys_changed),
                  NULL, NULL,
                  _gtk_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  /*
   * Key bindings
   */

  binding_set = gtk_binding_set_by_class (klass);

  gtk_binding_entry_add_signal (binding_set, GDK_space, 0,
                                "activate_focus", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Space, 0,
                                "activate_focus", 0);
  
  gtk_binding_entry_add_signal (binding_set, GDK_Return, 0,
                                "activate_default", 0);

  gtk_binding_entry_add_signal (binding_set, GDK_KP_Enter, 0,
                                "activate_default", 0);

  add_arrow_bindings (binding_set, GDK_Up, GTK_DIR_UP);
  add_arrow_bindings (binding_set, GDK_Down, GTK_DIR_DOWN);
  add_arrow_bindings (binding_set, GDK_Left, GTK_DIR_LEFT);
  add_arrow_bindings (binding_set, GDK_Right, GTK_DIR_RIGHT);

  add_tab_bindings (binding_set, 0, GTK_DIR_TAB_FORWARD);
  add_tab_bindings (binding_set, GDK_CONTROL_MASK, GTK_DIR_TAB_FORWARD);
  add_tab_bindings (binding_set, GDK_SHIFT_MASK, GTK_DIR_TAB_BACKWARD);
  add_tab_bindings (binding_set, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_DIR_TAB_BACKWARD);
#endif

  /* this is not done it the original sources..*/
    g_signal_new ("destroy",
                  G_TYPE_OBJECT,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  _gtk_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);
    g_signal_new ("delete_event",
                  G_TYPE_OBJECT,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  _gtk_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);
}

GType gtk_window_get_type (void) {

  static GType window_type = 0;

  DebOut("gtk_window_get_type()\n");

  if (!window_type) {
      static const GTypeInfo window_info = {
        sizeof (GtkWindowClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
        (GClassInitFunc) gtk_window_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_data */
        sizeof (GtkWindow),
        0,		/* n_preallocs */
        /*(GInstanceInitFunc) gtk_window_init,*/
        (GInstanceInitFunc) NULL,
      };

      window_type = g_type_register_static (GTK_TYPE_BIN, "GtkWindow", &window_info, 0);
      /*window_type = g_type_register_static (G_TYPE_OBJECT, "GtkWindow", &window_info, 0);*/
    }

  return window_type;
}

/*********************************
 * gtk_window_new:
 * @type: type of window
 * 
 * Creates a new #GtkWindow, which is a toplevel window that can
 * contain other widgets. Nearly always, the type of the window should
 * be #GTK_WINDOW_TOPLEVEL. If you're implementing something like a
 * popup menu from scratch (which is a bad idea, just use #GtkMenu),
 * you might use #GTK_WINDOW_POPUP. #GTK_WINDOW_POPUP is not for
 * dialogs, though in some other toolkits dialogs are called "popups".
 * In GTK+, #GTK_WINDOW_POPUP means a pop-up menu or pop-up tooltip.
 * On X11, popup windows are not controlled by the <link
 * linkend="gtk-X11-arch">window manager</link>.
 *
 * If you simply want an undecorated window (no window borders), use
 * gtk_window_set_decorated(), don't use #GTK_WINDOW_POPUP.
 * 
 * Return value: a new #GtkWindow.
 *********************************/
GtkWidget* gtk_window_new   (GtkWindowType   type) {

  GtkWindow *win;
  APTR obj,root,menu;

  DebOut("gtk_window_new(%d)\n",type);

  switch(type) {

    case GTK_WINDOW_TOPLEVEL:

      DebOut("  create GTK_WINDOW_TOPLEVEL\n");

      obj = NewObject(CL_Window->mcc_Class, NULL,
                      MUIA_Window_Menustrip, 
                      menu = (APTR)MenustripObject, 
                        MUIA_Menustrip_Enabled, (LONG) FALSE, 
                      End,
                      WindowContents, 
                        root = (APTR)NewObject(CL_RootGroup->mcc_Class, NULL, TAG_DONE)
                      );

      if(obj) {
        win=g_object_new (GTK_TYPE_WINDOW, NULL);
        if(win) {
          DebOut("  new gtk window %lx, mui %lx\n",win,obj);
          DoMethod(global_mgtk->MuiApp, OM_ADDMEMBER, (ULONG) obj);
          set(obj, MA_Widget, win);

          #warning fixme
	  /* fixed !? */

          DoMethod(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
                  MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

          GtkSetObj(GTK_WIDGET(win),      root);
					//GTK_MUI(win)->MuiObject       = NULL; /* added to avoid an error-requester under MorphOS, Kronos*/
          GTK_MUI(win)->MuiWin          = obj;
          GTK_MUI(win)->MuiMenu         = menu;
          GTK_MUI(win)->mainclass       = CL_WINDOW;

          DebOut("GTK_MUI(win)->MuiWin          = %lx\n",obj);
          DebOut("GTK_MUI(win)->MuiObject       = %lx\n",root);
          DebOut("GTK_MUI(win)->MuiMenu         = %lx\n",menu);

          if(!g_slist_length(global_mgtk->windows)) {
            DebOut("  this is the first window\n");
            global_mgtk->MuiRoot=obj;
            /* init the default style here */
            global_mgtk->default_style=mgtk_get_default_style(global_mgtk->MuiRoot);
          }

          global_mgtk->windows=g_slist_append(global_mgtk->windows,win);

          return GTK_WIDGET(win);
        }
        MUI_DisposeObject(obj);
      }
      return NULL;

  case GTK_WINDOW_POPUP:

    DebOut("  GTK_WINDOW_POPUP\n");
    win=g_object_new (GTK_TYPE_WINDOW, NULL);
    DebOut("  popup win=%lx\n",win);

    /* create empty pop up window,
     * at the moment, this is not used at all
     * (and most likely not tested ;))
     */
    GtkSetObj(GTK_WIDGET(win), PopobjectObject,
                              MUIA_Popstring_String, HGroup, End,
                              MUIA_Popstring_Button, PopButton(MUII_PopUp),
                            End);

    GTK_MUI(win)->mainclass=CL_AREA;
    if(GtkObj(win)) {
      return GTK_WIDGET(win);
    }
    ErrOut("could not create MUI PopobjectObject\n");
    break;

  default:
    ErrOut("gtk_window_new: unknown window type %d\n",type);
    break;
  }

  return gtk_frame_new("window dummy (created because of an error)");
}

void gtk_window_set_title   (GtkWindow *window, const gchar *title) {
  DebOut("gtk_window_set_title(win %lx,%s)\n",window,title);

  DebOut("  win->MuiWin=%lx\n",GTK_MUI(window)->MuiWin);

  SetAttrs(GTK_MUI(window)->MuiWin,MUIA_Window_Title,(ULONG) title, TAG_DONE); /* ouch, casting it to ULONG.. */
}

void gtk_window_set_resizable(GtkWindow *window, gboolean resizable) {
#if 0
  ULONG is_open, was_resizable;
#endif

  DebOut("gtk_window_set_resizable(%lx,%d)\n",window,resizable);

#if 0
  was_resizable = xget(GTK_MUI(window)->MuiWin, MUIA_Window_SizeGadget);
  is_open = xget(GTK_MUI(window)->MuiWin, MUIA_Window_Open);

  SetAttrs(GTK_MUI(window)->MuiWin,MUIA_Window_SizeGadget,(ULONG) resizable, TAG_DONE);

  if (is_open && !(was_resizable & resizable))
  {
    /* We must reopen window to toggle sizing gadget
    **
    ** This causes unavoidable flickery. It could be avoided by rootgroup subclass
    ** (by making group fixed size) but the sizing gadget remains.
    **
    ** Hmm...
    */

    SetAttrs(GTK_MUI(window)->MuiWin, MUIA_Window_Open, FALSE, TAG_DONE);
    SetAttrs(GTK_MUI(window)->MuiWin, MUIA_Window_Open, TRUE, TAG_DONE);
  }
#else
	/* Drawback in this method: sizing gadget is still there
	**
	** OTOH is there any good reason to disable resizing? Because GTK coders are lame?
  **
  ** Disable this
	*/

  set(GTK_MUI(window)->MuiObject, MA_RootGroup_Resizable, resizable);
#endif

  return;
}

gboolean gtk_window_get_resizable(GtkWindow *window)
{
	return xget(GTK_MUI(window)->MuiWin, MUIA_Window_SizeGadget);
}

void gtk_window_set_modal(GtkWindow *window, gboolean modal) {

  DebOut("gtk_window_set_modal(%lx,%d)\n",window,modal);

  window->modal=modal;
  set(GTK_MUI(window)->MuiWin, MA_GtkWindow_Modal, modal);

}

/* Dialog windows should be set transient for the main application window 
 * they were spawned from. This allows window managers to e.g. keep the 
 * dialog on top of the main window, or center the dialog over the main window. 
 * gtk_dialog_new_with_buttons() and other convenience functions in 
 * GTK+ will sometimes call gtk_window_set_transient_for() on your behalf.
 *
 * On Windows, this function will and put the child window on top of 
 * the parent, much as the window manager would have done on X.
 * In MUI we make the parent window the RefWindow of the child.
 * The child won't stay at top, but it should be sufficient.
 */
void gtk_window_set_transient_for(GtkWindow *window, GtkWindow *parent) {

  DebOut("gtk_window_set_transient_for(%lx,%lx)\n");

  if((!window) || (!parent) || (!GTK_MUI(window)->MuiObject) || (!GTK_MUI(parent)->MuiObject)) {
    WarnOut("  unable to gtk_window_set_transient_for, something is NULL!\n");
    return;
  }

  set(GTK_MUI(window)->MuiObject, MUIA_Window_RefWindow,GTK_MUI(parent)->MuiObject);
  set(GTK_MUI(window)->MuiObject, MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered);
  set(GTK_MUI(window)->MuiObject, MUIA_Window_TopEdge,  MUIV_Window_TopEdge_Centered);
}

void gtk_window_present(GtkWindow *window) {
  DebOut("gtk_window_present(%lx)\n",window);

  gtk_widget_show(GTK_WIDGET(window));

  if(GtkObj(window)) {
    set(GtkObj(window),MUIA_Window_Activate,(LONG) TRUE);
  }
}

void gtk_window_set_position(GtkWindow *window, GtkWindowPosition position) {

  DebOut("gtk_window_set_position(%lx,%lx)\n",window,position);

  DebOut("  gtk_window_set_position is TODO!\n");
}


static void gtk_window_set_property (GObject      *object,
			 guint         prop_id,
			 const GValue *value,
			 GParamSpec   *pspec) {
  GtkWindow  *window;
  
  window = GTK_WINDOW (object);

  switch (prop_id)
    {
#if 0
    case PROP_TYPE:
      window->type = g_value_get_enum (value);
      break;
#endif
    case PROP_TITLE:
      gtk_window_set_title (window, g_value_get_string (value));
      break;
#if 0
    case PROP_ROLE:
      gtk_window_set_role (window, g_value_get_string (value));
      break;
    case PROP_ALLOW_SHRINK:
      window->allow_shrink = g_value_get_boolean (value);
      gtk_widget_queue_resize (GTK_WIDGET (window));
      break;
    case PROP_ALLOW_GROW:
      window->allow_grow = g_value_get_boolean (value);
      gtk_widget_queue_resize (GTK_WIDGET (window));
      g_object_notify (G_OBJECT (window), "resizable");
      break;
    case PROP_RESIZABLE:
      window->allow_grow = g_value_get_boolean (value);
      gtk_widget_queue_resize (GTK_WIDGET (window));
      g_object_notify (G_OBJECT (window), "allow-grow");
      break;
    case PROP_MODAL:
      gtk_window_set_modal (window, g_value_get_boolean (value));
      break;
    case PROP_WIN_POS:
      gtk_window_set_position (window, g_value_get_enum (value));
      break;
    case PROP_DEFAULT_WIDTH:
      gtk_window_set_default_size_internal (window,
                                            TRUE, g_value_get_int (value),
                                            FALSE, -1, FALSE);
      break;
    case PROP_DEFAULT_HEIGHT:
      gtk_window_set_default_size_internal (window,
                                            FALSE, -1,
                                            TRUE, g_value_get_int (value), FALSE);
      break;
    case PROP_DESTROY_WITH_PARENT:
      gtk_window_set_destroy_with_parent (window, g_value_get_boolean (value));
      break;
    case PROP_ICON:
      gtk_window_set_icon (window,
                           g_value_get_object (value));
      break;
    case PROP_ICON_NAME:
      gtk_window_set_icon_name (window, g_value_get_string (value));
      break;
    case PROP_SCREEN:
      gtk_window_set_screen (window, g_value_get_object (value));
      break;
    case PROP_TYPE_HINT:
      gtk_window_set_type_hint (window,
                                g_value_get_enum (value));
      break;
    case PROP_SKIP_TASKBAR_HINT:
      gtk_window_set_skip_taskbar_hint (window,
                                        g_value_get_boolean (value));
      break;
    case PROP_SKIP_PAGER_HINT:
      gtk_window_set_skip_pager_hint (window,
                                      g_value_get_boolean (value));
      break;
    case PROP_ACCEPT_FOCUS:
      gtk_window_set_accept_focus (window,
				   g_value_get_boolean (value));
      break;
    case PROP_FOCUS_ON_MAP:
      gtk_window_set_focus_on_map (window,
				   g_value_get_boolean (value));
      break;
    case PROP_DECORATED:
      gtk_window_set_decorated (window, g_value_get_boolean (value));
      break;
    case PROP_GRAVITY:
      gtk_window_set_gravity (window, g_value_get_enum (value));
      break;
#endif
    default:
      break;
    }
}

static void
gtk_window_get_property (GObject      *object,
			 guint         prop_id,
			 GValue       *value,
			 GParamSpec   *pspec) {
  GtkWindow  *window;

  window = GTK_WINDOW (object);

  switch (prop_id)
    {
      GtkWindowGeometryInfo *info;
    case PROP_TYPE:
      g_value_set_enum (value, window->type);
      break;
    case PROP_ROLE:
      g_value_set_string (value, window->wm_role);
      break;
    case PROP_TITLE:
      g_value_set_string (value, window->title);
      break;
#if 0
    case PROP_ALLOW_SHRINK:
      g_value_set_boolean (value, window->allow_shrink);
      break;
    case PROP_ALLOW_GROW:
      g_value_set_boolean (value, window->allow_grow);
      break;
    case PROP_RESIZABLE:
      g_value_set_boolean (value, window->allow_grow);
      break;
    case PROP_MODAL:
      g_value_set_boolean (value, window->modal);
      break;
    case PROP_WIN_POS:
      g_value_set_enum (value, window->position);
      break;
    case PROP_DEFAULT_WIDTH:
      info = gtk_window_get_geometry_info (window, FALSE);
      if (!info)
	g_value_set_int (value, -1);
      else
	g_value_set_int (value, info->default_width);
      break;
    case PROP_DEFAULT_HEIGHT:
      info = gtk_window_get_geometry_info (window, FALSE);
      if (!info)
	g_value_set_int (value, -1);
      else
	g_value_set_int (value, info->default_height);
      break;
    case PROP_DESTROY_WITH_PARENT:
      g_value_set_boolean (value, window->destroy_with_parent);
      break;
    case PROP_ICON:
      g_value_set_object (value, gtk_window_get_icon (window));
      break;
    case PROP_ICON_NAME:
      g_value_set_string (value, gtk_window_get_icon_name (window));
      break;
    case PROP_SCREEN:
      g_value_set_object (value, window->screen);
      break;
    case PROP_IS_ACTIVE:
      g_value_set_boolean (value, window->is_active);
      break;
    case PROP_HAS_TOPLEVEL_FOCUS:
      g_value_set_boolean (value, window->has_toplevel_focus);
      break;
    case PROP_TYPE_HINT:
      g_value_set_enum (value,
                        window->type_hint);
      break;
    case PROP_SKIP_TASKBAR_HINT:
      g_value_set_boolean (value,
                           gtk_window_get_skip_taskbar_hint (window));
      break;
    case PROP_SKIP_PAGER_HINT:
      g_value_set_boolean (value,
                           gtk_window_get_skip_pager_hint (window));
      break;
    case PROP_ACCEPT_FOCUS:
      g_value_set_boolean (value,
                           gtk_window_get_accept_focus (window));
      break;
    case PROP_FOCUS_ON_MAP:
      g_value_set_boolean (value,
                           gtk_window_get_focus_on_map (window));
      break;
    case PROP_DECORATED:
      g_value_set_boolean (value, gtk_window_get_decorated (window));
      break;
    case PROP_GRAVITY:
      g_value_set_enum (value, gtk_window_get_gravity (window));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_window_show (GtkWidget *widget) {

  APTR obj;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WINDOW (widget));

#if 0
  GtkWindow *window = GTK_WINDOW (widget);
  GtkContainer *container = GTK_CONTAINER (window);
  gboolean need_resize;
#endif

  obj=GTK_MUI(widget)->MuiWin;

  if(!obj) {
    WarnOut("GTK_MUI(%lx)->MuiWin is NULL!\n", widget);
    return;
  }

  GTK_WIDGET_SET_FLAGS (widget, GTK_VISIBLE);

  set(obj, MUIA_Window_Open, FALSE);

  set(GtkObj(widget), MUIA_Window_Open, TRUE);

#if 0
  need_resize = container->need_resize || !GTK_WIDGET_REALIZED (widget);
  container->need_resize = FALSE;

  if (need_resize)
    {
      GtkWindowGeometryInfo *info = gtk_window_get_geometry_info (window, TRUE);
      GtkAllocation allocation = { 0, 0 };
      GdkGeometry new_geometry;
      guint width, height, new_flags;

      /* determine default size to initially show the window with */
      gtk_widget_size_request (widget, NULL);
      gtk_window_compute_default_size (window, &width, &height);

      /* save away the last default size for later comparisions */
      info->last.width = width;
      info->last.height = height;

      /* constrain size to geometry */
      gtk_window_compute_hints (window, &new_geometry, &new_flags);
      gtk_window_constrain_size (window,
         &new_geometry, new_flags,
         width, height,
         &width, &height);

      /* and allocate the window */
      allocation.width  = width;
      allocation.height = height;
      gtk_widget_size_allocate (widget, &allocation);
      
      if (GTK_WIDGET_REALIZED (widget))
        gdk_window_resize (widget->window, width, height);
      else
        gtk_widget_realize (widget);
    }
  
  gtk_container_check_resize (container);

  gtk_widget_map (widget);

  if (window->modal)
    gtk_grab_add (widget);
#endif
}

static void gtk_window_hide (GtkWidget *widget) {
  
  APTR       obj;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WINDOW (widget));

  obj=GTK_MUI(widget)->MuiWin;

  if(!obj) {
    WarnOut("GTK_MUI(%lx)->MuiWin is NULL!\n", widget);
    return;
  }

  set(obj, MUIA_Window_Open, FALSE);

  GTK_WIDGET_UNSET_FLAGS (widget, GTK_VISIBLE);

#if 0
  gtk_widget_unmap (widget);

  if (window->modal)
    gtk_grab_remove (widget);
#endif
}
