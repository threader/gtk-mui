/* GDK - The GIMP Drawing Kit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: gdkevents.c,v 1.3 2007/10/25 15:27:28 o1i Exp $
 */

#include <config.h>
#include <string.h>		/* For memset() */

#include "gdk.h"
#include "gdkevents.h"
#if 0
#include "gdkinternals.h"
#include "gdkalias.h"
#endif

#include "debug.h"

/**
 * gdk_event_copy:
 * @event: a #GdkEvent
 * 
 * Copies a #GdkEvent, copying or incrementing the reference count of the
 * resources associated with it (e.g. #GdkWindow's and strings).
 * 
 * Return value: a copy of @event. The returned #GdkEvent should be freed with
 * gdk_event_free().
 **/
GdkEvent *gdk_event_copy (GdkEvent *event) {

#if 0
  GdkEventPrivate *new_private;
  GdkEvent *new_event;
#endif
  
  g_return_val_if_fail (event != NULL, NULL);

  ErrOut("gdk_event_copy(%lx) not done yet!\n",event);
  return event;
#if 0
  
  new_event = gdk_event_new (GDK_NOTHING);
  new_private = (GdkEventPrivate *)new_event;

  *new_event = *event;
  if (new_event->any.window)
    g_object_ref (new_event->any.window);

#if 0
  if (gdk_event_is_allocated (event))
    {
      GdkEventPrivate *private = (GdkEventPrivate *)event;

      new_private->screen = private->screen;
    }
#endif
  
  switch (event->any.type)
    {
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
      new_event->key.string = g_strdup (event->key.string);
      break;
      
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY:
      if (event->crossing.subwindow != NULL)
	g_object_ref (event->crossing.subwindow);
      break;
      
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
      g_object_ref (event->dnd.context);
      break;
      
#if 0
    case GDK_EXPOSE:
      if (event->expose.region)
	new_event->expose.region = gdk_region_copy (event->expose.region);
      break;
#endif
      
    case GDK_SETTING:
      new_event->setting.name = g_strdup (new_event->setting.name);
      break;

#if 0
    case GDK_BUTTON_PRESS:
    case GDK_BUTTON_RELEASE:
      if (event->button.axes) 
	new_event->button.axes = g_memdup (event->button.axes, 
					     sizeof (gdouble) * event->button.device->num_axes);
      break;

    case GDK_MOTION_NOTIFY:
      if (event->motion.axes) 
	new_event->motion.axes = g_memdup (event->motion.axes, 
					   sizeof (gdouble) * event->motion.device->num_axes);
      
      break;
#endif
      
    default:
      break;
    }
  
  return new_event;
#endif
}

/**
 * gdk_event_free:
 * @event:  a #GdkEvent.
 * 
 * Frees a #GdkEvent, freeing or decrementing any resources associated with it.
 * Note that this function should only be called with events returned from
 * functions such as gdk_event_peek(), gdk_event_get(),
 * gdk_event_get_graphics_expose() and gdk_event_copy().
 **/
void gdk_event_free (GdkEvent *event) {

  g_return_if_fail (event != NULL);

  ErrOut("gdk_event_free(%lx) needs to be debugged..\n",event);

#if 0
  g_assert (event_chunk != NULL); /* paranoid */
#endif
  
  if (event->any.window)
    g_object_unref (event->any.window);
  
  switch (event->any.type)
    {
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
      g_free (event->key.string);
      break;
      
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY:
      if (event->crossing.subwindow != NULL)
	g_object_unref (event->crossing.subwindow);
      break;
      
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
      g_object_unref (event->dnd.context);
      break;

    case GDK_BUTTON_PRESS:
    case GDK_BUTTON_RELEASE:
      if (event->button.axes)
	g_free (event->button.axes);
      break;
      
#if 0
    case GDK_EXPOSE:
      if (event->expose.region)
	gdk_region_destroy (event->expose.region);
      break;
#endif
      
    case GDK_MOTION_NOTIFY:
      if (event->motion.axes)
	g_free (event->motion.axes);
      break;
      
    case GDK_SETTING:
      g_free (event->setting.name);
      break;
      
    default:
      break;
    }

#if 0
  g_hash_table_remove (event_hash, event);
  g_mem_chunk_free (event_chunk, event);
#endif
}


GType gdk_event_get_type (void) {

  static GType our_type = 0;

  DebOut("gdk_event_get_type()\n");
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("GdkEvent",
					     (GBoxedCopyFunc)gdk_event_copy,
					     (GBoxedFreeFunc)gdk_event_free);
  return our_type;
}

#define __GDK_EVENTS_C__
