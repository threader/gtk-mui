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
 * $Id: gtkmui.h,v 1.11 2012/11/27 14:36:26 o1i Exp $
 *
 *****************************************************************************/

/*****************************************************************************
 * This class hides all GTK-MUI internal stuff from the GTK applications.
 * From a GTK application, all parts of GTK_MUI are *private*.
 *****************************************************************************/

#ifndef __GTK_MUI_H__
#define __GTK_MUI_H__

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>

G_BEGIN_DECLS

#define GTK_TYPE_MUI                  (gtk_mui_get_type ())
#define GTK_MUI(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_MUI, GtkMui))
#define GTK_MUI_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_MUI, GtkMuiClass))
#define GTK_IS_MUI(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_MUI))
#define GTK_IS_MUI_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_MUI))
#define GTK_MUI_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_MUI, GtkMuiClass))


typedef struct _GtkMui       GtkMui;
typedef struct _GtkMuiClass  GtkMuiClass;
typedef struct _mgtk_signal_connect_data mgtk_signal_connect_data;

typedef enum {
	CL_UNDEFINED,
	CL_FAMILY,
	/* CL_APPLICATION, */ /* not possible */
	CL_WINDOW,
	CL_AREA,
	CL_OTHER, /* really undefined */
} mgtkMUIClass;

struct _mgtk_signal_connect_data{
  gchar *detailed_signal;
  guint id;
  GCallback          c_handler; /* callback*/
  gpointer           data;
  GClosureNotify     destroy_data;
  GConnectFlags      connect_flags;
  mgtk_signal_connect_data *next;
};

struct _GtkMui {
  GObject object;             /* parent object */
  Object *MuiObject;
  mgtkMUIClass mainclass;     /* main class tree (area, window, application), 
                               * defines what kind of tags and methods are available */
  APTR MuiWin;                /* if !IS_WINDOW, this points eventually to the MUI root Window (TODO) */
  APTR MuiMenu;
  APTR MuiGroup; 
  mgtk_signal_connect_data *mgtk_signal_connect_data; /* if you want to add a signal later */
  APTR nextObject;
  GSList *remember;
};

struct _GtkMuiClass {
  GObjectClass parent_class;
};

/* 
 * print widget tree for debugging
 */
void      gtk_mui_list(GtkMui *widget);
void      gtk_mui_list_child_tree(GtkMui *w);

/* commodity control */
void      gtk_mui_application_iconify(gboolean state);
gboolean  gtk_mui_application_is_iconified(void);
void      gtk_mui_application_gui_hotkey(char *cx_popkey);
gint      gtk_mui_application_icon(char *iconname);
GType     gtk_mui_get_type  (void) G_GNUC_CONST;

void mgtk_add(GtkMui *parent, GtkMui *son);

G_END_DECLS

#endif /* __GTK_MUI_H__ */
