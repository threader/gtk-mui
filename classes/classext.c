/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
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
 * E-Mail: gtk-mui "at" oliver-brunner.de
 *
 * $Id: classext.c,v 1.16 2011/07/04 12:16:21 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

#ifdef __MORPHOS__
#include <emul/emulregs.h>
#endif

/**********************************************************************
  mgtk_askminmax

  This is like standard mAskMinMax() but supports user defined
  default width/height

  This is just a trial and error AROS working code. Sorry, if I
  messed it up ;) o1i
**********************************************************************/

ULONG mgtk_askminmax(struct IClass *cl, APTR obj, struct MUIP_AskMinMax *msg, LONG defwidth, LONG defheight)
{
  unsigned int warn = 0;
  struct MUI_MinMax *mm; 
  GtkWidget *w;
  DebOut("calling super askminmax(..,%lx,..,%d,%d)\n",obj,defwidth,defheight);
  DoSuperMethodA(cl, obj, msg);

  mm = msg->MinMaxInfo;
  w=(GtkWidget *) xget(obj,MA_Widget);
  DebOut("mgtk_askminmax: vv widget %lx (%s) vv \n",w,GtkWidName(obj));
  DebOut("pre super askminmax(..,%lx,..,%d,%d)\n",obj,defwidth,defheight);
  DebOut("  mm->MinWidth: %d\n",mm->MinWidth);
  DebOut("  mm->MinHeight: %d\n",mm->MinHeight);
  DebOut("  mm->DefWidth: %d\n",mm->DefWidth);
  DebOut("  mm->DefHeight: %d\n",mm->DefHeight);
  DebOut("  mm->MaxWidth: %d\n",mm->MaxWidth);
  DebOut("  mm->MaxHeight: %d\n",mm->MaxHeight);

 

  mm = msg->MinMaxInfo;

  DebOut("mgtk_askminmax(..,%lx,..,%d,%d)\n",obj,defwidth,defheight);

  if(!w || !GTK_IS_MUI(w)) {
    ErrOut("mgtk_askminmax: widget %x is NULL/not GTK_MUI !?\n",(ULONG) w);
    return 0;
  }

//  DebOut("mgtk_askminmax: vv widget %lx (%s) vv \n",w,GtkWidName(obj));
  DebOut("post super askminmax(..,%lx,..,%d,%d)\n",obj,defwidth,defheight);
  DebOut("  mm->MinWidth: %d\n",mm->MinWidth);
  DebOut("  mm->MinHeight: %d\n",mm->MinHeight);
  DebOut("  mm->DefWidth: %d\n",mm->DefWidth);
  DebOut("  mm->DefHeight: %d\n",mm->DefHeight);
  DebOut("  mm->MaxWidth: %d\n",mm->MaxWidth);
  DebOut("  mm->MaxHeight: %d\n",mm->MaxHeight);

  /* at least under AROS, those values are sometimes.. strange (-3) ;) */ 
  if(mm->MinHeight < 0) {
    DebOut("setting mm->MinHeight to 0, was %d\n",mm->MinHeight);
    mm->MinHeight = 0;
    warn++;
  }
  if(mm->DefHeight < 0) {
    DebOut("setting mm->DefHeight to 1, was %d\n",mm->DefHeight);
    mm->DefHeight = 1;
    warn++;
  }
  if(mm->MaxHeight < 0) {
    DebOut("setting mm->MaxHeight to MAXMAX, was %d\n",mm->MaxHeight);
    mm->MaxHeight = MUI_MAXMAX;
    warn++;
  }
  if(mm->MinWidth < 0) {
    DebOut("setting mm->MinWidth to 0, was %d\n",mm->MinHeight);
    mm->MinWidth = 0;
    warn++;
  }
  if(mm->DefWidth < 0) {
    DebOut("setting mm->DefWidth to 1, was %d\n",mm->DefWidth);
    mm->DefWidth = 1;
    warn++;
  }
  if(mm->MaxWidth < 0) {
    DebOut("setting mm->MaxWidth to MAXMAX, was %d\n",mm->MaxWidth);
    mm->MaxWidth = MUI_MAXMAX;
    warn++;
  }

  /* hmm, according to MUI examples, we have to add our values here ..*/
  if (defwidth > AskMin_Limited ) {
    mm->DefWidth += defwidth;
    mm->MinWidth += defwidth;
  }
  if (defheight > AskMin_Limited ) {
    mm->DefHeight += defheight;
    mm->MinHeight += defheight;
  }

  /* do another test, if the values are reasonable..*/
  if(mm->MinWidth > mm->DefWidth ) {
    DebOut("aehm .. mm->MinWidth > mm->DefWidth !?\n");
    mm->MinWidth=mm->DefWidth;
    warn++;
  }

  if(mm->MaxWidth < mm->DefWidth) {
    DebOut("aehm .. mm->MaxWidth < mm->DefWidth !?\n");
    mm->MaxWidth = mm->DefWidth;
    warn++;
  }

  if(mm->MinHeight > mm->DefHeight) {
    DebOut("aehm .. (mm->MinHeight > mm->DefHeight)  !?\n");
    DebOut("         mm->MinHeight: %d\n",mm->MinHeight);
    DebOut("         mm->DefHeight: %d\n",mm->DefHeight);
    warn++;
    mm->MinHeight=mm->DefHeight;
  }

  if(mm->MaxHeight < mm->DefHeight) {
    DebOut("aehm .. mm->MaxHeight < mm->DefHeight !?\n");
    warn++;
    mm->MaxHeight = mm->DefHeight;
  }

  /* let the group take up all the space, it gets offered */
  if(defheight == AskMin_Unlimited) {
    mm->MaxHeight = MUI_MAXMAX;
  }
  if(defwidth == AskMin_Unlimited) {
    mm->MaxWidth = MUI_MAXMAX;
  }

  /* just take the default space, leave the rest untouched */
  if(defheight == AskMin_Limited) {
    mm->MaxHeight = mm->DefHeight;
  }
  if(defwidth == AskMin_Limited) {
    mm->MaxWidth = mm->DefWidth;
  }

  w=(GtkWidget *) xget(obj,MA_Widget);
  if(w && warn) {
    DebOut("=========================================================\n");
    WarnOut("%d Warnings for object %lx: %s (%lx)\n",warn,obj,g_type_name(G_OBJECT_TYPE (w)),w);
    gtk_mui_list(GTK_MUI(w));
    DebOut("=========================================================\n");
  }


  DebOut("  mm->MinWidth,MinHeight: %d,%d\n",mm->MinWidth,mm->MinHeight);
  DebOut("  mm->DefWidth,DefHeight: %d,%d\n",mm->DefWidth,mm->DefHeight);
  DebOut("  mm->MaxWidth,Maxheight: %d,%d\n",mm->MaxWidth,mm->MaxHeight);

  DebOut("mgtk_askminmax: ^^ widget %lx (%s) ^^ \n",w,GtkWidName(obj));
 
  return 0;
}

/****************************
 * mgtk_get_widget_from_obj
 *
 * get the GtkWidget of a 
 * MUI object
 ****************************/
GtkWidget *mgtk_get_widget_from_obj(Object *obj) {
  Object *o;

  o=obj;
  while(o && !xget(o,MA_Widget)) {
    DebOut("  o=%x\n",o);
    o=_parent(o);
  }

  if(!o) {
    ErrOut("Could not find GTK widget for MUI Object %lx\n",obj);
    return NULL;
  }

  return (GtkWidget *) xget(o,MA_Widget);
}


#ifdef __AROS__
APTR DoSuperNew(struct IClass *cl, APTR obj, ULONG tag1, ...) {

  return ((APTR)DoSuperMethod(cl, obj, OM_NEW, (ULONG) &tag1, (ULONG) NULL));
}
#endif

