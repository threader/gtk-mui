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
 * $Id: gtk.h,v 1.20 2012/11/27 14:36:26 o1i Exp $
 *
 *****************************************************************************/

#ifndef __GTK_H__
#define __GTK_H__ 

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <stdarg.h>

#include <glib/gmacros.h>
#include <gtk/gtkenums.h>


/* TODO: remove me */
#define P_(String) (String)
/* end of remove me */

/* ???...*/
/* #define GTK_MAJOR_VERSION 2*/

/* Pens used for GTK styles */
#define MGTK_PEN_FG      TEXTPEN
#define MGTK_PEN_BG      BACKGROUNDPEN
#define MGTK_PEN_LIGHT   SHADOWPEN
#define MGTK_PEN_DARK    TEXTPEN
#define MGTK_PEN_MID     TEXTPEN
#define MGTK_PEN_TEXT    TEXTPEN
#define MGTK_PEN_BASE    BACKGROUNDPEN
#define MGTK_PEN_TEXT_AA TEXTPEN

#define MGTK_PEN_WHITE   SHINEPEN
#define MGTK_PEN_BLACK   TEXTPEN


#include <glib/gtypes.h>
#include <glib-object.h>
#include <gtk/gtktypeutils.h>


#define GDestroyNotify gint

/* GTK */



APTR mgtk_allocvec(ULONG size, ULONG req);
VOID mgtk_freevec (APTR ptr);
APTR mgtk_allocmem(ULONG size, ULONG req);
VOID mgtk_freemem (APTR ptr, ULONG size);

gulong gtk_signal_connect_full  (GtkObject *object,
                                 const gchar *name,
                                  GtkSignalFunc func,
                                  GtkCallbackMarshal unsupported,
                                  gpointer data,
                                  GtkDestroyNotify destroy_func,
                                  gint object_signal,
                                  gint after);


#define gtk_signal_lookup(name,object_type) \
  g_signal_lookup ((name), (object_type))
#define gtk_signal_name(signal_id) \
  g_signal_name (signal_id)
#define gtk_signal_emit_stop(object,signal_id) \
  g_signal_stop_emission ((object), (signal_id), 0)
#define gtk_signal_connect(object,name,func,func_data) \
  gtk_signal_connect_full ((object), (name), (func), 0, (func_data), 0, 0, 0)
#define gtk_signal_connect_after(object,name,func,func_data) \
  gtk_signal_connect_full ((object), (name), (func), 0, (func_data), 0, 0, 1)
#define gtk_signal_connect_object(object,name,func,slot_object) \
  gtk_signal_connect_full ((object), (name), (func), 0, (slot_object), 0, 1, 0)
#define gtk_signal_connect_object_after(object,name,func,slot_object) \
  gtk_signal_connect_full ((object), (name), (func), 0, (slot_object), 0, 1, 1)
#define gtk_signal_disconnect(object,handler_id) \
  g_signal_handler_disconnect ((object), (handler_id))


/* from gaim/internal.h*/
#define N_(String) (String)
#define _(x) (x)

typedef struct {
  char *key;
  gpointer value;
  struct internalGObject *next;
} internalGObject;

typedef struct {
  guint type;
  guint id;
  gchar *name;
  APTR next;
} mgtk_SignalType;

/*gdk */

#include <gdk/gdktypes.h>
#include <gdk/gdkvisual.h>
#include <gdk/gdkgc.h>
#include <gdk/gdkdevice.h>


#include <gtk/gtkstyle.h>

/***************************************************
 * GtkWidget
 *
 * Every GTK Object uses the following struct
 ***************************************************/
#include <gtk/gtkrequisition.h>

#ifndef GtkWidget
typedef struct _GtkWidget GtkWidget;
#endif

#include <gtk/gtkobject.h>
#include <gtk/gtkmui.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdkevents.h>
#include <gdk/gdkwindow.h>


typedef struct {
  GType type_id;
  gchar *type_name;
  guint16    instance_size;
  GtkWidget *(*newparentobject) (void);
  GtkWidget *(*newobject) (void);
  GtkWidgetClass *class;
  APTR next;
} mgtk_Fundamental;

typedef struct {
  GtkObject *object;
  guint signal_id;
  guint type;
} mgtk_stored_signal;

/***************************************************
 * global data 
 ***************************************************/
struct mgtk_ObjApp {
  APTR MuiApp;
  APTR MuiRoot;      /* WindowObject */
  APTR MuiRootStrip; /* WindowObject Menu Strip*/
  struct DiskObject *icon;    /* iconify icon object */
  GSList *windows;   /* all GTK windows in a linked list */
  GtkWidget *grab;   /* the widget getting all keystrokes.. TODO */
  guint mgtk_signal_count;
  mgtk_SignalType *mgtk_signal;       /* All GTK (custom) signals  */
  GType mgtk_fundamental_count;
  mgtk_Fundamental *mgtk_fundamental; /* All widget types */
  GtkStyle *default_style;
  guint white_black_optained;
  LONG white_pen;
  LONG black_pen;
  guint other_pens_optained;
#if 0
  GList *delayed_signals;   /* signals which would have been called from a hook */
#endif

  struct Screen   *screen;
  struct DrawInfo *dri;
  APTR             visualinfo;
};

#include <gtk/gtkprogressbar.h>

typedef GtkWidget GdkPixbuf;
typedef GtkWidget GtkTextBuffer;
typedef GtkWidget GtkOptionMenu;
typedef GtkWidget GtkHandleBox;

#include <gtk/gtkmain.h>
#include <gtk/gtktypebuiltins.h>
#include <gtk/gtkmarshalers.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkrange.h>
#include <gtk/gtksettings.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtktoolbar.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkframe.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtktable.h>
#include <gtk/gtksignal.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmisc.h>
#include <gtk/gtklist.h>
#include <gtk/gtkscale.h>
#include <gtk/gtkrange.h>
#include <gtk/gtkvscale.h>
#include <gtk/gtkhscale.h>
#include <gtk/gtkcombo.h>
#include <gtk/gtkdialog.h>
#include <gtk/gtkfilesel.h>
#include <gtk/gtkclist.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkhbbox.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenushell.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkfixed.h>
#include <gtk/gtkaccelgroup.h>
#include <gtk/gtkaccellabel.h>
#include <gtk/gtkaccelmap.h>


/* internal functions */
ULONG      xget(Object *obj, ULONG attr);
GtkWidget *mgtk_widget_new(int type);
int        mgtk_add_child(GtkWidget *parent, GtkWidget *child);
void       mgtk_initchange(GtkWidget *w);
void       mgtk_exitchange(GtkWidget *w);
void      *mgtk_malloc(int size);
void       mgtk_radio_button_muify(GtkWidget *button);
Object    *mgtk_get_mui_action_object(GtkWidget *widget);
void       mgtk_call_destroy_hooks(GtkWidget *widget);
GType      mgtk_g_type_register_fundamental(GType type_id, const gchar *type_name, GtkWidget  *(*newobject) (void));
mgtk_Fundamental *mgtk_g_type_get_fundamental(GType type_id);

/* gtk functions */
void       gtk_init(int *argc, char ***argv);
gboolean    gtk_init_check                  (int *argc,
                                             char ***argv);
GtkWidget* gtk_button_new (void);
GtkWidget* gtk_button_new_with_label    (const char *label);
GtkWidget* gtk_button_new_from_stock       (const gchar *stock_id);
gpointer   gtk_type_class                  (GtkType type);
void       gtk_grab_add                    (GtkWidget *widget);
void       gtk_grab_remove                 (GtkWidget *widget);
GtkWidget* gtk_grab_get_current            (void);
void       gtk_main         (void);
void       gtk_main_quit    (void);
void       gtk_widget_destroy( GtkWidget *widget/*, GtkWidget *param*/ );
void       gtk_widget_unref(GtkWidget *widget);
GtkWidget *gtk_widget_ref(GtkWidget *widget);
void       gtk_widget_set_size_request    (GtkWidget           *widget,
                                           gint                 width,
                                            gint                 height);
void       gtk_widget_set_usize           (GtkWidget *widget,
                                            gint width,
                                            gint height);
void       gtk_widget_set_state            (GtkWidget *widget,
                                             GtkStateType state);
GtkWidget *gtk_hbox_new     (gint homogeneous, gint spacing );
GtkWidget *mgtk_hbox_new_noparams( void );

GtkWidget *gtk_option_menu_new             (void);
void       gtk_option_menu_set_menu(GtkOptionMenu *option_menu, GtkWidget *menu);


void gtk_editable_select_region( GtkEditable *entry,
                                              gint         start,
                                              gint         end );
void        gtk_editable_insert_text        (GtkEditable *editable,
                                              const gchar *new_text,
                                              gint new_text_length,
                                              gint *position);

void       gtk_editable_set_editable         (GtkEditable *editable,
                                            gboolean     is_editable);
gboolean    gtk_editable_get_editable         (GtkEditable *editable);
gchar*      gtk_editable_get_chars (GtkEditable *editable,
																		gint start_pos,
																		gint end_pos);

GtkWidget*  gtk_image_new                   (void);
GtkWidget*  gtk_image_new_from_file         (const gchar *filename);
GtkWidget  *gtk_arrow_new(GtkArrowType arrow_type, GtkShadowType shadow_type);
GtkWidget  *gtk_hseparator_new(void);
GtkWidget  *gtk_hscrollbar_new(GtkAdjustment *adjustment);
GtkWidget  *gtk_hscale_new( GtkAdjustment *adjustment );
GtkWidget  *gtk_vscale_new( GtkAdjustment *adjustment );
void        gtk_range_set_update_policy     (GtkRange *range, GtkUpdateType policy);
GtkTooltips* gtk_tooltips_new(void);
void        gtk_tooltips_set_tip(GtkTooltips *tooltips, 
																	GtkWidget *widget, 
																	const gchar *tip_text, 
																	const gchar *tip_private);
void        gtk_tooltips_disable(GtkTooltips *tooltips);
void        gtk_tooltips_enable(GtkTooltips *tooltips);
void        gtk_tooltips_force_window(GtkTooltips *tooltips);

GtkWidget*  gtk_label_new                   (const char *str);
void        gtk_label_set_text              (GtkLabel *label,
                                             const char *str);
GtkWidget*  gtk_progress_bar_new            (void);
const gchar* gtk_progress_bar_get_text      (GtkProgressBar *pbar);
void        gtk_progress_bar_set_text       (GtkProgressBar *pbar,
                                             const gchar *text);
gdouble     gtk_progress_bar_get_fraction   (GtkProgressBar *pbar);
GtkProgressBarOrientation gtk_progress_bar_get_orientation
                                            (GtkProgressBar *pbar);
void        gtk_progress_bar_pulse          (GtkProgressBar *pbar);
void        gtk_progress_bar_set_fraction   (GtkProgressBar *pbar,
                                             gdouble fraction);
void        gtk_progress_bar_set_orientation
                                            (GtkProgressBar *pbar,
                                             GtkProgressBarOrientation orientation);

guint       g_timeout_add                   (guint interval, GSourceFunc function, gpointer data);
guint       gtk_timeout_add                 (guint32 interval, GtkFunction function, gpointer data);
void        gtk_timeout_remove              (guint timeout_handler_id);
gboolean    g_source_remove                 (guint tag);
gpointer    gtk_type_class                  (GtkType type);

GtkStyle*   gtk_style_new                   (void);
GtkStyle*   gtk_style_attach                (GtkStyle *style,
                                             GdkWindow *window);
void        gtk_style_set_background        (GtkStyle *style,
                                             GdkWindow *window,
                                              GtkStateType state_type);
GtkStyle*   gtk_rc_get_style_by_paths       (GtkSettings *settings,
                                              const char *widget_path,
                                              const char *class_path,
                                              GType type);
GtkStyle*   mgtk_get_default_style          (Object *obj);
void        gtk_paint_polygon               (GtkStyle *style,
                                              GdkWindow *window,
                                              GtkStateType state_type,
                                              GtkShadowType shadow_type,
                                              GdkRectangle *area,
                                              GtkWidget *widget,
                                              const gchar *detail,
                                              GdkPoint *points,
                                              gint npoints,
                                              gboolean fill);
void        gtk_paint_box                   (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_arrow                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             GtkArrowType arrow_type,
                                             gboolean fill,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_shadow                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_check                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_extension             (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkPositionType gap_side);
void        gtk_paint_flat_box              (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_focus                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_option                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_slider                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkOrientation orientation);
void        gtk_paint_vline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint y1_,
                                             gint y2_,
                                             gint x);
void        gtk_paint_hline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x1,
                                             gint x2,
                                             gint y);
GtkWidget*  gtk_handle_box_new              (void);
GtkWidget *mgtk_custom_new(void);

#define GTK_HANDLE_BOX(n)       ((GtkHandleBox *) n)
#define GTK_SIGNAL_FUNC(f)      ((GtkSignalFunc) (f))

GtkOptionMenu     *GTK_OPTION_MENU(GtkWidget *widget);


/*#define GTK_ENTRY(a) a*/

/*
void GLADE_HOOKUP_OBJECT(GtkWidget *a,GtkWidget *b,gchar *foo);
void GLADE_HOOKUP_OBJECT_NO_REF(GtkWidget *a,GtkWidget *b,gchar *foo);
*/

#define GTK_CHECK_VERSION(major,minor,micro) 1
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)  ((gint)   (p))

/*
typedef struct _GdkPixbuf GdkPixbuf;
struct GdkPixbuf
{
  int  priv;
};
*/


/*
#include <gdk/gdkevents.h>
*/

#include <gtk/gtkstock.h>
#include <glib/gslist.h>
#include <glib/gstrfuncs.h>

GdkPixbuf*  gdk_pixbuf_new_from_file(const char *filename, GError **error);

/* gnome */
#ifndef PACKAGE
#define PACKAGE "GTK-MUI"
#define VERSION "0.0666"
#define LIBGNOMEUI_MODULE NULL
#define GNOME_PARAM_APP_DATADIR "PROGDIR:"
#define GNOME_FILE_DOMAIN_APP_PIXMAP "PROGDIR:"
#define PACKAGE_DATA_DIR "PROGDIR"
#endif

typedef struct {
  const char *name;
  const char *version;
  const char *description;
/*  GnomeModuleRequirement *requirements;

  GnomeModuleHook instance_init;
  GnomeModuleHook pre_args_parse, post_args_parse;

  struct poptOption *options;
  
  GnomeModuleInitHook init_pass;
  
  GnomeModuleClassInitHook class_init;*/
  
  const char *opt_prefix;
  gpointer    expansion1;
} GnomeModuleInfo;

/*
void gnome_program_init            (const char *app_id,
                                             const char *app_version,
					      const GnomeModuleInfo *module_info,
					      int argc,
					      char **argv,
					      const char *first_property_name,
					      ...);
*/

/* maybe wrong parameters..*/
#define gnome_program_init(a,b,c,argc,argv,f,g,h) gtk_init(&argc,&argv)

/*ATK*/
typedef struct _AtkAction AtkAction;
typedef struct {
  int badmagic;
} _AtkAction;


/* mui */
#ifndef __AROS__
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif



/* TODO: remove me */
Object *mgtk_get_mui_object(GtkWidget *w);
void    GtkSetObj(GtkWidget *widget, APTR obj);
#ifdef __AROS__
#define GtkObj(widget)      mgtk_get_mui_object((GtkWidget *)widget)
#else
#define GtkObj(widget)      GTK_MUI(widget)->MuiObject
#endif

#define GtkWinObj(widget)   GTK_MUI(widget)->MuiWin
#define GtkMenuObj(widget)  GTK_MUI(widget)->MuiWinMenustrip

/* get the gtk widget class name of a mui object */
#define GtkWidName(obj)   g_type_name(G_TYPE_FROM_INSTANCE ((GtkWidget *)xget(obj,MA_Widget)))

#endif /* __GTK_H__  */
