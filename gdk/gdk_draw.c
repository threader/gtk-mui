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
 * $Id: gdk_draw.c,v 1.9 2013/07/17 10:37:00 mmartinka Exp $
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

/* pens (gc) is still TODO! */

// ---- point/points
void gdk_draw_point (GdkDrawable *drawable, GdkGC *gc, gint x, gint y)
{
  GtkWidget *widget;
  Object *obj;
  LONG pen;
  ULONG red,green,blue;
  struct Screen *screen;

  DebOut("gdk_draw_point(%lx,%lx,%d,%d)\n",drawable,gc,x,y);

  if(!global_mgtk || !xget(global_mgtk->MuiRoot,MUIA_Window_Open)) {
    /* WarnOut("Window not open? (mgtk_ObjApp %lx)\n",global_mgtk);*/
    return;
  }
  
  if(!drawable) {
    DebOut("ERROR: drawable is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:drawable is NULL!\n");
    return;
  }

  if(!gc) {
    DebOut("ERROR: gc is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:gc is NULL!\n");
    return;
  }

  widget=(GtkWidget *) drawable->mgtk_widget;

  if(!widget) {
    DebOut("ERROR: drawable->mgtk_widget is NULL!\n");
    return;
  }
  
  obj=GtkObj(widget);

  if(!obj) {
    DebOut("ERROR: obj is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c: obj is NULL!\n");
    return;
  }
  
  screen=_screen(obj);
  if(!screen) {
    ErrOut("gdk/gdk_draw.c: gdk_draw_point: could not find actual screen struct!\n");
    return;
  }
  
  if(gc->colormap->colors->havepen==1) {
    pen=(LONG) gc->colormap->colors->pen;
    DebOut("  found pen: %d\n",(int) pen);
  } else {
    red  =  gc->colormap->colors->red   << 16;
    blue =  gc->colormap->colors->blue  << 16;
    green=  gc->colormap->colors->green << 16;

    pen = ObtainBestPenA( screen->ViewPort.ColorMap, red, green, blue, NULL ); 
  }
  
/* we crash here, if the window is not visible..*/
  if(!_rp(obj)) {
    ErrOut("RP is NULL!?\n");
  }
  
  SetAPen(_rp(obj),pen);
  Move(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
  WritePixel(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
  
  if(!gc->colormap->colors->havepen) {
    ReleasePen(screen->ViewPort.ColorMap,pen);
  }
}

void gdk_draw_points (GdkDrawable *drawable, GdkGC *gc, GdkPoint *points, gint npoints)
{
  gint i;

  DebOut("gdk_draw_points(%lx,%lx,%lx,%d)\n",drawable,gc,points,npoints);

  if(npoints<2) {
    return;
  }

  for (i=0; i<npoints;) {
    gdk_draw_point(drawable,gc,points[i].x,points[i].y);
    i++;
  }
}


// ---- line/lines
void gdk_draw_line(GdkDrawable *drawable, GdkGC *gc, gint x1, gint y1, gint x2, gint y2) {
  GtkWidget *widget;
  Object *obj;
  LONG pen;

  DebOut("gdk_draw_line(%lx,%lx,%d,%d,%d,%d)\n",drawable,gc,x1,y1,x2,y2);

  if(!drawable) {
    DebOut("ERROR: drawable is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:drawable is NULL!\n");
    return;
  }

  if(!gc) {
    DebOut("ERROR: gc is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:gc is NULL!\n");
    return;
  }

  if(gc->colormap->colors->havepen) {
    pen=gc->colormap->colors->pen;
    DebOut("  found pen: %d\n",pen);
  }
  else {
    DebOut("WARNING: assuming gc pointer is a pen (%d)..\n",gc); /* TODO */
    pen=(LONG) gc;
  }

  widget=(GtkWidget *) drawable->mgtk_widget;

  if(!widget) {
    DebOut("ERROR: drawable->mgtk_widget is NULL!\n");
    return;
  }
  obj=GtkObj(widget);

  if(!obj) {
    DebOut("ERROR: obj is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c: obj is NULL!\n");
    return;
  }

  SetAPen(_rp(obj),_dri(obj)->dri_Pens[pen]);
  Move(_rp(obj),_mleft(obj)+x1,_mtop(obj)+y1);
  Draw(_rp(obj),_mleft(obj)+x2,_mtop(obj)+y2);

}

void gdk_draw_lines(GdkDrawable *drawable, GdkGC *gc, GdkPoint *points, gint npoints) {

  gint i;

  DebOut("gdk_draw_lines(%lx,%lx,%lx,%d)\n",drawable,gc,points,npoints);

  if(npoints<2) {
    return;
  }

  for (i=1; i<npoints; i++) {
    gdk_draw_line(drawable,gc,points[i-1].x,points[i-1].y,points[i].x,points[i].y);
  }

}

// ---- rectangle
void gdk_draw_rectangle(GdkDrawable *drawable, GdkGC *gc, gint filled, gint x, gint y, gint width, gint height) {
  GtkWidget *widget;
  Object *obj;
  LONG pen;
  struct Screen *screen;
  int i;
  ULONG red,green,blue;

  DebOut("gdk_draw_rectangle(%lx,%lx,%d,%d,%d,%d,%d)\n",drawable,gc,filled,x,y,width,height);

  /* a bad hack to ensure, that our app is not hidden 
   * (with exchange)
   */
  if(!global_mgtk || !xget(global_mgtk->MuiRoot,MUIA_Window_Open)) {
    /* WarnOut("Window not open? (mgtk_ObjApp %lx)\n",global_mgtk);*/
    return;
  }

  /* try to check everything ..*/
  if(!drawable) {
    ErrOut("gdk/gdk_draw.c: drawable is NULL!\n");
    return;
  }

  if(!gc) {
    ErrOut("gdk/gdk_draw.c:gc is NULL!\n");
    return;
  }

  widget=(GtkWidget *) drawable->mgtk_widget;
  DebOut("  widget=%lx\n",widget);

  if(!widget) {
    ErrOut("gdk/gdk_draw.c: gdk_draw_rectangle: drawable->mgtk_widget is NULL!\n");
    return;
  }

  obj=GtkObj(widget);

  if(!obj) {
    ErrOut("GDK: no obj!\n");
    return;
  }

  screen=_screen(obj);
  if(!screen) {
    ErrOut("gdk/gdk_draw.c: gdk_draw_rectangle: could not find actual screen struct!\n");
    return;
  }
  DebOut(" screen: %lx\n",_screen(obj));

  /* do we need to obtain a pen? */
  if(gc->colormap->colors->havepen==1) {
    pen=(LONG) gc->colormap->colors->pen;
    DebOut("  found pen: %d\n",(int) pen);
  }
  else {
    DebOut(" we have no pen up to now, so we should get one ..\n",gc);
    DebOut("   gc->colormap->colors->red: %lx\n",gc->colormap->colors->red);
    DebOut("   gc->colormap->colors->blue: %lx\n",gc->colormap->colors->blue);
    DebOut("   gc->colormap->colors->green: %lx\n",gc->colormap->colors->green);
    DebOut("   gc->colormap->colors->pixel: %d\n",gc->colormap->colors->pixel);
    red  =  gc->colormap->colors->red   << 16;
    blue =  gc->colormap->colors->blue  << 16;
    green=  gc->colormap->colors->green << 16;

    DebOut("   red  : %lx\n",red);
    DebOut("   blue : %lx\n",blue);
    DebOut("   green: %lx\n",green);

    pen = ObtainBestPenA( screen->ViewPort.ColorMap, red, green, blue, NULL ); 
    DebOut(" new pen=%d\n",pen);
    /* who would release this ..? 
    gc->colormap->colors->pen=pen;
    gc->colormap->colors->havepen=1;
    */
  }

/* we crash here, if the window is not visible..*/
  if(!_rp(obj)) {
    ErrOut("RP is NULL!?\n");
  }

  SetAPen(_rp(obj),pen);

  if(!filled) {
    Move(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
    Draw(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y);

    Move(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
    Draw(_rp(obj),_mleft(obj)+x,_mtop(obj)+y+height);
    
    Move(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y+height);
    Draw(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y);
    Move(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y+height);
    Draw(_rp(obj),_mleft(obj)+x,_mtop(obj)+y+height);
  }
  else {
   /* for(i=_mleft(obj)+x;i<_mleft(obj)+x+width;i++) {
      Move(_rp(obj),i,_mtop(obj)+y);
      Draw(_rp(obj),i,_mtop(obj)+y+height);
    } */
	RectFill(_rp(obj),_mleft(obj)+x, _mtop(obj)+y,_mleft(obj)+x+width, _mtop(obj)+y+height);
  }

  /* if we did not store the pen, we have to release it */
  if(!gc->colormap->colors->havepen) {
    ReleasePen(screen->ViewPort.ColorMap,pen);
  }

  DebOut("  exit gdk_draw_rectangle\n");
}
