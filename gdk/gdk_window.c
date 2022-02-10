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
 * $Id: gdk_window.c,v 1.8 2013/03/25 10:22:49 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "../debug.h"
#include <gtk/gtk.h>
#include "../gtk_globals.h"

GdkWindow *gdk_window_new(GdkWindow *parent, GdkWindowAttr *attributes, gint attributes_mask) {
  GdkWindow *ret;
  GtkWidget *widget;

  DebOut("======================= gdk_window_new ====================\n");
  DebOut("gdk_window_new(%lx,%lx,%d)\n",parent,attributes,attributes_mask);

  ret=g_new0(GdkWindow,1);

  if(!attributes->visual) {
    ErrOut("gdk_window_new: attributes->visual is NULL\n");
    return ret;
  }
  if(!attributes->visual->mgtk_widget) {
    ErrOut("gdk_window_new: attributes->visual->mgtk_widget is NULL\n");
    return ret;
  }

  /* get back the dirty stored widget */
  widget=(GtkWidget *) attributes->visual->mgtk_widget;
  if(widget) {
    DebOut("  widget=%lx\n",widget);

    /* and store it dirty again ;) */
    ret->mgtk_widget=(GdkWindow *) widget;

    /* set size of MUI object */
    DebOut("  x=%d\n",attributes->x);
    DebOut("  y=%d\n",attributes->y);
    /* TODO */
  }
  else {
    DebOut("WARNING: could not find widget\n");
  }

#warning gdk_window_new: anything to do here ??

  return ret;
}

gboolean gdk_window_is_viewable(GdkWindow *window) {
  GtkWidget *widget;

  widget=window->mgtk_widget;

  return GTK_WIDGET_VISIBLE(widget);
}

void gdk_window_clear_area(GdkWindow *window, gint x, gint y, gint width, gint height) {
  GtkWidget *widget;
  Object *obj;

  DebOut("gdk_window_clear_area(%lx,%d,%d,%d,%d)\n",window,x,y,width,height);

  if(!widget) {
    ErrOut("gdk_window_clear_area(%lx,%d,%d,%d,%d) with window==NULL !?\n",window,x,y,width,height);
    return;
  }

  widget=(GtkWidget *) window->mgtk_widget;

  if(!widget) {
    DebOut("WARNING: could not find widget\n");
    return;
  }

  obj=GtkObj(widget);

  SetAPen(_rp(obj),_dri(obj)->dri_Pens[MGTK_PEN_BG]);
  RectFill(_rp(obj),_mleft(obj)+x,_mtop(obj)+y,_mleft(obj)+x+width,_mtop(obj)+y+height);
}

void gdk_window_set_user_data(GdkWindow *window, gpointer user_data) {

  DebOut("gdk_window_set_user_data(%lx,%lx)\n",window,user_data);
  DebOut("WARNING: Just a dummy\n");
#warning gdk_window_set_user_data: Just a dummy
}

void gdk_window_move_resize(GdkWindow *window, gint x, gint y, gint width, gint height) {

  DebOut("gdk_window_move_resize(%lx,%d,%d,%d,%d)\n",window,x,y,width,height);
  DebOut("WARNING: Just a dummy\n");
#warning gdk_window_move_resize: Just a dummy
}

GdkWindow *gdk_window_get_pointer(GdkWindow *window, gint *x, gint *y, GdkModifierType *mask) {

  DebOut("gdk_window_get_pointer(%lx,..)\n",window);
  DebOut("WARNING: Just a dummy\n");
#warning gdk_window_get_pointer: Just a dummy
  return window;
}
