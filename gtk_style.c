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
 * $Id: gtk_style.c,v 1.12 2007/03/19 09:00:06 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"


/* --- functions --- */

/* WARNING: Styles are not "full real" Gtk Objects..*/

GType gtk_style_get_type (void) {

  static GType style_type = 0;

  DebOut("gtk_style_get_type(): most stuff not done yet\n");
  
  if (!style_type)
    {
      static const GTypeInfo style_info =
      {
        sizeof (GtkStyleClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        /*(GClassInitFunc) gtk_style_class_init,*/
        (GClassInitFunc) NULL,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GtkStyle),
        0,              /* n_preallocs */
        /*(GInstanceInitFunc) gtk_style_init,*/
        (GInstanceInitFunc) NULL,
      };
      
      style_type = g_type_register_static (G_TYPE_OBJECT, "GtkStyle", &style_info, 0);
    }
  
  return style_type;
}

/*************** OLD ***********************************************************/
/* TODO: move gdk functions to gdk/ */

static GdkGC *allocgc() {
  GdkGC *ret;

  ret=g_new0(GdkGC,1);

  ret->colormap=g_new0(GdkColormap,1);

  ret->colormap->colors=g_new0(GdkColor,1);

  ret->colormap->colors->havepen=0;

  return ret;
}

static GdkGC *newgc(gint defaultpen) {
  GdkGC *ret;

  ret=allocgc();

  ret->colormap->colors->pen=defaultpen;
  ret->colormap->colors->havepen=1;

  return ret;
}

GdkGC *gdk_gc_new(GdkWindow *window) {
  GdkGC *ret;

  ret=allocgc();
  ret->colormap->colors->havepen=0;

  return ret;
}

GtkStyle *gtk_style_new() {

  GtkStyle *ret;
  int i;

  DebOut("gtk_style_new()\n");

/*
  ret=g_new0(GtkStyle,1);
  */
  ret=g_object_new (GTK_TYPE_STYLE, NULL);
  DebOut("  ret=%lx\n",ret);
  i=0;
  while(i<6) {
    ret->fg_gc[i]=allocgc();
    ret->bg_gc[i]=allocgc();
    ret->light_gc[i]=allocgc();
    ret->dark_gc[i]=allocgc();
    ret->mid_gc[i]=allocgc();
    ret->text_gc[i]=allocgc();
    ret->base_gc[i]=allocgc();
    ret->text_aa_gc[i]=allocgc();
    ret->bg_pixmap[i]=g_new0(GdkPixmap,1);

    i++;
  }
  ret->white_gc=allocgc();
  ret->black_gc=allocgc();

  ret->xthickness=2;
  ret->ythickness=2;

  ret->colormap=g_new0(GdkColormap,1);
  ret->colormap->colors=g_new0(GdkColor,1);
  ret->colormap->colors->havepen=0;

  ret->attach_count=0;

  return ret;
}

void gtk_style_set_background(GtkStyle *style, GdkWindow *window, GtkStateType state_type) {

  DebOut("gtk_style_set_background(%lx,%lx,%d)\n",style,window,state_type);

  DebOut("  ... nothing to do, we care for styles in MUI?\n");

}

void gtk_style_apply_default_background (GtkStyle *style, GdkWindow *window, gboolean set_bg, GtkStateType state_type, GdkRectangle *area, gint x, gint y, gint width, gint height) {

  DebOut("gtk_style_apply_default_background(%lx,%lx,%d)\n",style,window,set_bg);

  DebOut("  ... nothing to do, we care for styles in MUI?\n");
}

/*
        DETAILPEN
            Pen compatible with V34. Used to render text in the screen's title bar.
        BLOCKPEN
          Pen compatible with V34. Used to fill the screen's title bar.
        TEXTPEN
          Pen for regular text on BACKGROUNDPEN.
        SHINEPEN
          Pen for the bright edge on 3D objects.
        SHADOWPEN
          Pen for the dark edge on 3D objects.
        FILLPEN
          Pen for filling the active window borders and selected gadgets.
        FILLTEXTPEN
          Pen for text rendered over FILLPEN.
        BACKGROUNDPEN
          Pen for the background color.  Currently must be zero.
        HIGHLIGHTTEXTPEN
          Pen for "special color" or highlighted text on BACKGROUNDPEN.k

        or MUI pens ??
          MPEN_SHINE      = 0,
          MPEN_HALFSHINE  = 1,
          MPEN_BACKGROUND = 2,
          MPEN_HALFSHADOW = 3,
          MPEN_SHADOW     = 4,
          MPEN_TEXT       = 5,
          MPEN_FILL       = 6,
          MPEN_MARK       = 7,
          MPEN_COUNT      = 8,

*/

#define INTUI_PENS 1

GtkStyle *mgtk_get_default_style(Object *obj) {
  unsigned int i;
#ifndef INTUI_PENS
  struct MUI_RenderInfo *mri;
#endif
  GETGLOBAL

  DebOut("mgtk_get_default_style(%lx)\n");

  if(!mgtk->default_style) {
    DebOut("  first call, init mgtk->default_style\n");
    if(!obj) {
      if(mgtk->MuiRoot) {
        DebOut("  obj is NULL, using mgtk->MuiRoot..\n");
        obj=mgtk->MuiRoot;
      }
      else {
        DebOut("WARNING: mgtk->MuiRoot and obj == NULL, could not init style..\n");
        return NULL;
      }
    }

    mgtk->default_style=gtk_style_new();
    i=0;
    while(i<6) {
#if INTUI_PENS
      mgtk->default_style->fg_gc[i]=newgc(MGTK_PEN_FG);  
      mgtk->default_style->bg_gc[i]=newgc(MGTK_PEN_BG);
      mgtk->default_style->light_gc[i]=newgc(MGTK_PEN_LIGHT);
      mgtk->default_style->dark_gc[i]=newgc(MGTK_PEN_DARK);
      mgtk->default_style->mid_gc[i]=newgc(MGTK_PEN_MID); /* hmm..*/
      mgtk->default_style->text_gc[i]=newgc(MGTK_PEN_TEXT);
      mgtk->default_style->base_gc[i]=newgc(MGTK_PEN_BASE);
      mgtk->default_style->text_aa_gc[i]=newgc(MGTK_PEN_TEXT_AA); /* hmm..*/
#else
      mgtk->default_style->fg_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_TEXT]));  
      mgtk->default_style->bg_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
      mgtk->default_style->light_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_SHINE]));
      mgtk->default_style->mid_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_HALFSHADOW])); 
      mgtk->default_style->dark_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_SHADOW]));
      mgtk->default_style->text_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_TEXT]));
      mgtk->default_style->base_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
      mgtk->default_style->text_aa_gc[i]=newgc(MUIPEN(mri->mri_Pens[MPEN_TEXT])); 
#endif

      DebOut("mgtk->default_style->fg_gc[i]=%d\n",mgtk->default_style->fg_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->bg_gc[i]=%d\n",mgtk->default_style->bg_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->light_gc[i]=%d\n",mgtk->default_style->light_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->mid_gc[i]=%d\n",mgtk->default_style->mid_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->dark_gc[i]=%d\n",mgtk->default_style->dark_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->text_gc[i]=%d\n",mgtk->default_style->text_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->base_gc[i]=%d\n",mgtk->default_style->base_gc[i]->colormap->colors->pen);
      DebOut("mgtk->default_style->text_aa_gc[i]=%d\n",mgtk->default_style->text_aa_gc[i]->colormap->colors->pen);

      mgtk->default_style->bg_pixmap[i]=g_new0(GdkPixmap,1);

      i++;
    }
    mgtk->default_style->white_gc=newgc(mgtk->white_pen); 
    mgtk->default_style->black_gc=newgc(mgtk->black_pen); 
  }
  else {
    DebOut("  just return mgtk->default_style (%lx)\n",mgtk->default_style);
  }

  mgtk->default_style->attach_count++;
  DebOut("  mgtk_get_default_style successfull\n");
  return mgtk->default_style;
}

/* Attaches a style to a window; this process allocates 
 * the colors and creates the GC's for the style - it 
 * specializes it to a particular visual and colormap. 
 * The process may involve the creation of a new style if 
 * the style has already been attached to a window with a 
 * different style and colormap.
 */

GtkStyle *gtk_style_attach(GtkStyle *style, GdkWindow *window) {
  GtkWidget *widget;

  DebOut("gtk_style_attach(%lx,%lx)\n",style,window);

  if(!window) {
    DebOut("WARNING: window is NULL, so I cannot attach style..\n");
    return NULL;
  }

  if(style) {
    window->style=style;
  }
  else {
    widget=(GtkWidget *) window->mgtk_widget;
#warning Someone should check that GDK stuff.
    window->style=mgtk_get_default_style(GtkObj(widget));
  }

  /* o1i: after this function, bg_gc must be filled, according to bg ..*/
  /* this might be totally wrong...? */
  style->bg_gc[GTK_STATE_NORMAL]->colormap->colors->red=style->bg[GTK_STATE_NORMAL].red;
  style->bg_gc[GTK_STATE_NORMAL]->colormap->colors->blue=style->bg[GTK_STATE_NORMAL].blue;
  style->bg_gc[GTK_STATE_NORMAL]->colormap->colors->green=style->bg[GTK_STATE_NORMAL].green;

  return window->style;
}

void gdk_gc_set_rgb_fg_color(GdkGC *gc, const GdkColor *color) {

  DebOut("gdk_gc_set_rgb_fg_color(%lx,%lx)",gc,color);

  if(!gc->colormap) {
    ErrOut("gdk_gc_set_rgb_fg_color: gc has no colormap!\n");
    return;
  }

  gc->colormap->colors->red=color->red;
  gc->colormap->colors->green=color->green;
  gc->colormap->colors->blue=color->blue;
  gc->colormap->colors->havepen=0;

  /* TODO: ObtainPen..? */
}

/* A style created by matching with the supplied paths, or NULL 
 * if nothing matching was specified and the default style 
 * should be used.
 */
GtkStyle* gtk_rc_get_style_by_paths(GtkSettings *settings, const char *widget_path, const char *class_path, GType type) {
  DebOut("gtk_rc_get_style_by_paths(%lx,%s,%s,%d)\n",settings,widget_path,class_path,type);

  /* as we ignore styles more or less completely, a default style is fine here */

  return NULL;
}

GtkStyle *gtk_style_copy (GtkStyle *style) {

  DebOut("gtk_style_copy(%lx)\n",style);

  DebOut("  TODO: this just calls gtk_style_new..\n");

  return gtk_style_new();
}


