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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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
 * $Id: gdkvisual-fb.c,v 1.3 2011/07/04 12:16:21 o1i Exp $
 */

#include <config.h>
#include "../gdkvisual.h"
#if 0
#include "gdkprivate-fb.h"
#include "gdkinternals.h"
#endif
#include <sys/ioctl.h>
#include "debug.h"

static GdkVisual *system_visual = NULL;

static void
gdk_visual_finalize (GObject *object)
{
  g_error ("A GdkVisual object was finalized. This should not happen");
}

static void
gdk_visual_class_init (GObjectClass *class)
{
  class->finalize = gdk_visual_finalize;
}


GType
gdk_visual_get_type (void)
{
  static GType object_type = 0;

  if (!object_type)
    {
      static const GTypeInfo object_info =
      {
        sizeof (GdkVisualClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gdk_visual_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GdkVisual),
        0,              /* n_preallocs */
        (GInstanceInitFunc) NULL,
      };
      
      object_type = g_type_register_static (G_TYPE_OBJECT,
                                            "GdkVisual",
                                            &object_info, 0);
    }
  
  return object_type;
}

void _gdk_visual_init (void) {

  DebOut("_gdk_visual_init()\n");

  system_visual = g_object_new (GDK_TYPE_VISUAL, NULL);

#if 0
  system_visual->depth = system_visual->bits_per_rgb = gdk_display->modeinfo.bits_per_pixel;
#endif
  system_visual->depth = system_visual->bits_per_rgb = 8;
  system_visual->byte_order = GDK_LSB_FIRST;
  system_visual->colormap_size = 0;

#if 0
  switch (gdk_display->sinfo.visual)
    {
    case FB_VISUAL_PSEUDOCOLOR:
      system_visual->colormap_size = 1 << gdk_display->modeinfo.bits_per_pixel;
      system_visual->type = GDK_VISUAL_PSEUDO_COLOR;
      break;
    case FB_VISUAL_DIRECTCOLOR:
    case FB_VISUAL_TRUECOLOR:
#endif
      system_visual->type = GDK_VISUAL_TRUE_COLOR;

#if 0
      system_visual->red_prec = gdk_display->modeinfo.red.length;
      system_visual->red_shift = gdk_display->modeinfo.red.offset;
      system_visual->red_mask = ((1 << (system_visual->red_prec)) - 1) << system_visual->red_shift;

      system_visual->green_prec = gdk_display->modeinfo.green.length;
      system_visual->green_shift = gdk_display->modeinfo.green.offset;
      system_visual->green_mask = ((1 << (system_visual->green_prec)) - 1) << system_visual->green_shift;

      system_visual->blue_prec = gdk_display->modeinfo.blue.length;
      system_visual->blue_shift = gdk_display->modeinfo.blue.offset;
      system_visual->blue_mask = ((1 << (system_visual->blue_prec)) - 1) << system_visual->blue_shift;
#else
      /* Significant only for GDK_VISUAL_PSEUDOCOLOR and GDK_VISUAL_DIRECTCOLOR. (hopfully ;) */
      system_visual->red_prec = 0;
      system_visual->red_shift = 0;
      system_visual->red_mask = 0;

      system_visual->green_prec = 0;
      system_visual->green_shift = 0;
      system_visual->green_mask = 0;

      system_visual->blue_prec = 0;
      system_visual->blue_shift = 0;
      system_visual->blue_mask = 0;

#endif

#if 0
      if (gdk_display->sinfo.visual == FB_VISUAL_DIRECTCOLOR) 
	{
	  guint16 red[256], green[256], blue[256];
	  struct fb_cmap fbc = {0,0};
	  int size, i;
	  /* Load the colormap to ramps here, as they might be initialized to
	     some other garbage */
	  
	  g_warning ("Directcolor visual, not very well tested\n");
	  fbc.red = red;
	  fbc.green = green;
	  fbc.blue = blue;
	  
	  size = 1 << system_visual->red_prec;
	  for (i = 0; i < size; i++)
	    red[i] = i * 65535 / (size - 1);
	  
	  size = 1 << system_visual->green_prec;
	  fbc.len = size;
	  for (i = 0; i < size; i++)
	    green[i] = i * 65535 / (size - 1);
	  
	  size = 1 << system_visual->blue_prec;
	  for (i = 0; i < size; i++)
	    blue[i] = i * 65535 / (size - 1);
	  
	  ioctl (gdk_display->fb_fd, FBIOPUTCMAP, &fbc);
	}
      break;
    case FB_VISUAL_STATIC_PSEUDOCOLOR:
      system_visual->type = GDK_VISUAL_STATIC_COLOR;
      system_visual->colormap_size = 1 << gdk_display->modeinfo.bits_per_pixel;
      break;
    default:
      g_assert_not_reached ();
      break;
    }
#endif
}

gint
gdk_visual_get_best_depth (void)
{
  return system_visual->depth;
}

GdkVisualType
gdk_visual_get_best_type (void)
{
  return system_visual->type;
}

GdkVisual*
gdk_screen_get_system_visual (GdkScreen *screen)
{
  return system_visual;
}

GdkVisual*
gdk_visual_get_best (void)
{
  return system_visual;
}

GdkVisual*
gdk_visual_get_best_with_depth (gint depth)
{
  if (system_visual->depth != depth)
    return NULL;

  return system_visual;
}

GdkVisual*
gdk_visual_get_best_with_type (GdkVisualType visual_type)
{
  if (system_visual->type != visual_type)
    return NULL;

  return system_visual;
}

GdkVisual*
gdk_visual_get_best_with_both (gint          depth,
			       GdkVisualType visual_type)
{
  if (system_visual->depth != depth)
    return NULL;

  if (system_visual->type != visual_type)
    return NULL;

  return system_visual;
}

void
gdk_query_depths  (gint **depths,
		   gint  *count)
{
  *count = 1;
  *depths = &system_visual->depth;
}

void
gdk_query_visual_types (GdkVisualType **visual_types,
			gint           *count)
{
  *count = 1;
  *visual_types = &system_visual->type;
}

GList*
gdk_screen_list_visuals (GdkScreen *screen)
{
  return g_list_append (NULL, gdk_visual_get_system ());
}

GdkScreen *
gdk_visual_get_screen (GdkVisual *visual)
{
  g_return_val_if_fail (GDK_IS_VISUAL (visual), NULL);

#if 0
  return gdk_screen_get_default ();
#endif
  return NULL;
}
