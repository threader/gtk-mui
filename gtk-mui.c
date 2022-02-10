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
 * $Id: gtk-mui.c,v 1.70 2013/04/02 09:05:11 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/commodities.h>

#ifdef __AROS__
#include <dos/dos.h>
#endif
#include <mui.h>
/* cross platform hooks */
#ifdef __AROS__
#include "include/SDI_hook.h"
#else
#include <SDI_hook.h>
#endif

#include "classes.h"
#include "debug.h"
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkmenubar.h>
#include "gtk_globals.h"
#include <gtk/gtkrange.h>
#include <gtk/gtkfixed.h>
#include "mui.h"

/* stolen from YAM */
ULONG xget(Object *obj, ULONG attr) { 
  ULONG b = 0;
  GetAttr(attr, obj, &b);
  return b;
}

/**********************************************************************
 * GTK-MUI class
 **********************************************************************/

enum {
  COMMODITY_SHOW,
  COMMODITY_HIDE,
  LAST_SIGNAL
};

static guint mui_signals[LAST_SIGNAL] = { 0 };

static GtkMuiClass *parent_class = NULL;

static void gtk_mui_init (GtkMui *mui) {

  DebOut("gtk_mui_init(%lx)\n",mui);

  mui->MuiObject = NULL;
  mui->MuiWin = NULL;
  mui->MuiMenu = NULL;
  mui->MuiGroup = NULL;
  mui->mgtk_signal_connect_data = NULL;
  GTK_MUI(mui)->mainclass = CL_UNDEFINED;
  mui->nextObject = (GtkWidget *) NULL;
}

static void gtk_mui_class_init (GtkMuiClass *class);

GType gtk_mui_get_type (void) {

#if 0
  DebOut("gtk_mui_get_type()\n");
#endif

  static GType mui_type = 0;

  if (!mui_type) {
      static const GTypeInfo mui_info = {
        sizeof (GtkMuiClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
        (GClassInitFunc) gtk_mui_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_data */
        sizeof (GtkMui),
        0,		/* n_preallocs */
        (GInstanceInitFunc) gtk_mui_init,
      };
      mui_type = g_type_register_static (G_TYPE_OBJECT, "GtkMui", &mui_info, 0);
    }

  return mui_type;
}

static void gtk_mui_class_init (GtkMuiClass *class) {
  GtkWidgetClass *widget_class;

  DebOut("gtk_mui_class_init(%lx)\n",class);

  widget_class = (GtkWidgetClass*) class;
  parent_class = g_type_class_peek_parent (class);


  mui_signals[COMMODITY_SHOW] =
    gtk_signal_new ("commodity-show",
		    GTK_RUN_FIRST,
		    gtk_mui_get_type(),
		    NULL,
		    _gtk_marshal_VOID__VOID,
		    GTK_TYPE_NONE, 0);

  mui_signals[COMMODITY_HIDE] =
    gtk_signal_new ("commodity-hide",
		    GTK_RUN_FIRST,
		    gtk_mui_get_type(),
		    NULL,
		    _gtk_marshal_VOID__VOID,
		    GTK_TYPE_NONE, 0);
}


/**********************************************************************
 * tmp helper
 **********************************************************************/
Object *mgtk_get_mui_object(GtkWidget *w) {

#if 0
  DebOut("mgtk_get_mui_object(%lx)\n",w);
  DebOut("  GTK_MUI(w)->MuiObject: %lx\n",GTK_MUI(w)->MuiObject);
  DebOut("  w->MuiObject:          %lx\n",w->MuiObject);
#endif

  if(w) {
    if(GTK_MUI(w)->MuiObject ) {  /* remove all those helpers as soon as possible ..*/
      return GTK_MUI(w)->MuiObject;
    }
  }

  if(!GTK_IS_MENU_BAR(w)) {
    WarnOut("mgtk_get_mui_object: could not find MuiObject for %lx (%s)!\n",(ULONG) w,g_type_name(G_TYPE_FROM_INSTANCE(w)));
  }
  else {
    DebOut("mgtk_get_mui_object: could not find MuiObject for %lx (%s)!\n",(ULONG) w,g_type_name(G_TYPE_FROM_INSTANCE(w)));
  }
  return NULL;
  
}
/**********************************************************************
  mgtk_allocvec, mgtk_freevec, mgtk_allocmem, mgtk_freemem
**********************************************************************/

APTR mgtk_allocvec(ULONG size, ULONG req)
{
  APTR ptr;
#ifdef __MORPHOS__
  ptr = AllocVecTaskPooled(size);

  if (ptr && (req & MEMF_CLEAR))
  {
    memset(ptr, 0, size);
  }
#else
  ptr = AllocVec(size, req);
#endif

	return ptr;
}

VOID mgtk_freevec(APTR ptr)
{
#ifdef __MORPHOS__
  FreeVecTaskPooled(ptr);
#else
  FreeVec(ptr);
#endif
}

APTR mgtk_allocmem(ULONG size, ULONG req)
{
  APTR ptr;
#ifdef __MORPHOS__
  ptr = AllocTaskPooled(size);

  if (ptr && (req & MEMF_CLEAR))
  {
    memset(ptr, 0, size);
  }
#else
  ptr = AllocMem(size, req);
#endif

	return ptr;
}

VOID mgtk_freemem(APTR ptr, ULONG size)
{
#ifdef __MORPHOS__
  FreeTaskPooled(ptr, size);
#else
  FreeMem(ptr, size);
#endif
}

/*********************************
 * private functions
 *********************************/

APTR mgtk_malloc(int size) {
  APTR ret;

  DebOut("mgtk_malloc(%d)\n",size);

  if (!(ret = AllocRemember(&rememberKey, size, MEMF_CLEAR ))) {
    printf("mgtk_malloc: Unable to allocate %d bytes of memory\n",size);
    return(NULL);
  }

  return(ret);
}

int mgtk_add_child(GtkWidget *parent, GtkWidget *child) {

  DebOut("mgtk_add_child(parent %lx, child %lx)\n",parent,child);

  if(child->parent != NULL) {
    WarnOut("mgtk_add_child: child %x has already a parent (%x), new parent now: %x\n",(int) child,(int) child->parent,(int) parent);
  }

  child->parent=parent;

#if 0
  /* we should get the window through MUI.. */
  if((child->MuiWin!=NULL) && (child->MuiWin!=parent->MuiWin)) {
    WarnOut("mgtk_add_child: child %x has already a MuiWin (%x), new MuiWin now: %x\n",(int) child,(int) child->MuiWin,(int) parent->MuiWin);
  }

  if(parent->MuiWin!=NULL) {
    child->MuiWin=parent->MuiWin;
  }
  else {
    DebOut("WARNING: mgtk_add_child: someone forgot to set MuiWin for %x\n",(int) parent);
  }
#endif

  return 0;
}

/* to iterate is human .. */
void mgtk_initchange(GtkWidget *change) {

  DebOut("mgtk_initchange(%lx)\n",change);

  if(!change || !change->parent) {
    return;
  }

  /*
  if(change->type != IS_GROUP) {
    mgtk_initchange((APTR) change->parent);
    return;
  }

  mgtk_initchange((APTR) change->parent);*/

  change=(APTR) change->parent;

  DebOut("  change->parent=%lx\n",change->parent);
  DebOut("  mgtk_initchange: DoMethod(%lx,MUIM_Group_InitChange)\n",GtkObj(change));

  DoMethod(GtkObj(change),MUIM_Group_InitChange);

  DebOut("  mgtk_initchange: done\n");

  return;
}

void mgtk_exitchange(GtkWidget *change) {

  DebOut("mgtk_exitchange(%lx)\n",change);

  if(!change || !change->parent) {
    return;
  }

  /*
  if(change->type != IS_GROUP) {
    mgtk_exitchange((APTR) change->parent);
    return;
  }
  */

  change=(APTR) change->parent;

  DebOut("  mgtk_exitchange: DoMethod(%lx,MUIM_Group_ExitChange)\n",change);
  DoMethod(GtkObj(change),MUIM_Group_ExitChange);

  return;
}


void mgtk_file_selection_show(GtkWidget *widget);

/*********************************
 * gtk_widget_show
 *********************************/

void mgtk_widget_show(GtkWidget *widget) {

  int realized;
  int type;
  int newstyle=0;

	DebOut("mgtk_widget_show(%lx)\n",widget);

  if(!GTK_IS_WIDGET (widget)) {
    ErrOut("Widget %lx is not GTK_IS_WIDGET!\n",widget);
    /*WarnOut("  *%lx: %lx\n",widget, *widget);*/
    g_return_if_fail (GTK_IS_WIDGET (widget));
  }

  if(GTK_WIDGET_REALIZED(widget)) {
    DebOut("  widget is already realized\n");
    return;
  }

  newstyle=1;
  type=G_OBJECT_TYPE(widget);
  DebOut("    type    : %s\n",g_type_name(type));

  if(newstyle && GTK_IS_FILE_SELECTION(widget)) {
    DebOut("  widget is a GTK_TYPE_FILE_SELECTION\n");
    mgtk_file_selection_show(widget);
    return;
  }

  /* MenuBars don't have MuiObjects, which is ok */
  if(!GtkObj(widget)&&!(GTK_IS_MENU_BAR(widget))) {
    WarnOut("  Widget %lx has a NULL MuiObject!\n",(ULONG) widget);
    return;
  }

  realized=GTK_OBJECT(widget)->flags && GTK_REALIZED;

  /* set realized flag */
  GTK_OBJECT(widget)->flags |= GTK_REALIZED;

  if(newstyle && ((type == GTK_TYPE_WINDOW)||type == GTK_TYPE_DIALOG)) {
    DebOut("  widget is a window\n");
    /* CX_HIDE !!
    set(global_mgtk->MuiApp, MUIA_Application_Iconified, TRUE);
    */
    set(GTK_MUI(widget)->MuiWin,MUIA_Window_Open,TRUE);

    return;
  }
#if 0
  else if (newstyle && (GTK_MUI(widget)->mainclass == CL_AREA))
  {
    DebOut("better not use ShowMe (?)\n");
/*    SetAttrs(GtkObj(widget), MUIA_Group_Forward, FALSE, MUIA_ShowMe, TRUE, TAG_DONE);*/
    set(GtkObj(widget), MUIA_Group_Forward, FALSE);
    set(GtkObj(widget), MUIA_ShowMe, TRUE);
  }
  else
  {
    DebOut("gtk_widget_show() for non window/area ignored, type: %x\n",type);
  }
#endif

/* seems to be wrong.. is better done in gtk_widget ..?
  if(newstyle && !realized) {
    g_signal_emit_by_name(widget,"realized");
  }
*/
	DebOut("  mgtk_widget_show exit\n");
}

void mgtk_widget_hide(GtkWidget *widget) {

  DebOut("mgtk_widget_hide(%lx)\n",widget);

  if (GTK_MUI(widget)->mainclass == CL_WINDOW) {
    set(GtkObj(widget), MUIA_Window_Open, FALSE);
  }
#if 0
  else if (GTK_MUI(widget)->mainclass == CL_AREA) {
    SetAttrs(GtkObj(widget), MUIA_Group_Forward, FALSE, MUIA_ShowMe, FALSE, TAG_DONE);
  }
  else {
    WarnOut("gtk_widget_hide() for non window/area ignored\n");
  }
#endif
  DebOut("mgtk_widget_hide(%lx) left\n",widget);
}

void mgtk_widget_show_all(GtkWidget *widget) {

  if(GTK_IS_WINDOW(widget)) {
    set(GtkObj(widget),MUIA_Window_Open,TRUE);
    return;
  }
#if 0
  else if (GTK_MUI(widget)->mainclass == CL_AREA) {
    /* MUIA_ShowMe is forwarded to children */

    set(GtkObj(widget), MUIA_ShowMe, TRUE);
  }
  else {
    WarnOut("gtk_widget_show_all() for non window/area ignored\n");
  }
#endif
}

void mgtk_widget_hide_all(GtkWidget *widget) {

  if (GTK_MUI(widget)->mainclass == CL_WINDOW)
  {
    set(GtkObj(widget), MUIA_Window_Open, FALSE);
  }
#if 0
  else if (GTK_MUI(widget)->mainclass == CL_AREA)
  {
    set(GtkObj(widget), MUIA_ShowMe, FALSE);
  }
  else
  {
    WarnOut("gtk_widget_hide_all() for non window/area ignored\n");
  }
#endif
}

/* just a hack */
int is_in_notebook(Object *o) {

  GtkWidget *w;

  if(!o) {
    return FALSE;
  }

  w=(GtkWidget *) xget(o,MA_Widget);

  if(w) {
    /* some important object ;) */

    if(GTK_IS_NOTEBOOK(w)) {
      return TRUE;
    }
  }

  return is_in_notebook((Object *) xget(o,MUIA_Parent));
}

void update_note(Object *o) {

  GtkWidget *w;
  Object *child;
  LONG n;

  DebOut("update_note(%lx)\n",o);

  if(!o) {
    ErrOut("update_note did not find a notebook, which it should!\n");
    return; 
  }

  w=(GtkWidget *) xget(o,MA_Widget);

  if(w) {
    /* some important object ;) */

    if(GTK_IS_NOTEBOOK(w)) {
      DebOut("  GTK_IS_NOTEBOOK\n");
      n=xget(GtkObj(w),MUIA_Group_ActivePage);
      DebOut("  n=%d\n",n);

      child=(Object *) DoMethod(GtkObj(w),MM_NoteBook_PageNObject,(LONG) n);
      if(child) {
        DoMethod(child,MUIM_Group_InitChange);
        DoMethod(child,MUIM_Group_ExitChange); /* here it can crash, for ex. if in j-uae you click on coherent on,
	                                        * while the gui is updated (after start)
						*/
      }
      else {
        ErrOut("gtk-mui.c: update_note error\n");
      }
      return;
    }
  }

  update_note((Object *) xget(o,MUIA_Parent));
}


/***************************************
 * This is a workaround for a zune bug:
 * If you do a MUIM_Group_InitChange/
 * MUIM_Group_ExitChange ona note here, 
 * it fails. The note needs to be
 * updated in update_note manually..
 ***************************************/
void mgtk_widget_set_sensitive(GtkWidget *widget, gboolean sensitive) {
  LONG note;

  DebOut("mgtk_widget_set_sensitive(%lx,%d)\n",widget,sensitive);

  if(GTK_WIDGET_SENSITIVE (widget) == sensitive) {
    DebOut("  nothing to do here\n");
    return;
  }

  DebOut("GtkObj(%lx)=%lx\n",widget,GtkObj(widget));

  if (GTK_MUI(widget)->mainclass == CL_AREA) {
    note=is_in_notebook(GtkObj(widget));
    if(!note) {
      DoMethod(GtkObj(widget),MUIM_Group_InitChange);
    }

    set(GtkObj(widget), MUIA_Disabled, !sensitive);

    if(!note) {
      DoMethod(GtkObj(widget),MUIM_Group_ExitChange);
    }

    if(note) {
      update_note(GtkObj(widget));
    }
  }
  else {
    WarnOut("gtk_widget_set_sensitive() - only area class objects supported\n");
  }

}

/* Creates the GDK (windowing system) resources associated with a widget. 
 * For example, widget->window will be created when a widget is realized. 
 * Normally realization happens implicitly; if you show a widget and all 
 * its parent containers, then the widget will be realized and mapped 
 * automatically.

 * Realizing a widget requires all the widget's parent widgets to be 
 * realized; calling gtk_widget_realize() realizes the widget's parents 
 * in addition to widget itself. If a widget is not yet inside a toplevel 
 * window when you realize it, bad things will happen.

 * This function is primarily used in widget implementations, and isn't 
 * very useful otherwise. Many times when you think you might need it, 
 * a better approach is to connect to a signal that will be called after 
 * the widget is realized automatically, such as "expose_event". Or simply 
 * g_signal_connect_after() to the "realize" signal.
 */
#if 0

// this doesn't work, or?? win.. ->/dev/null

void gtk_widget_realize(GtkWidget *widget) {
  GdkWindow *win;

  DebOut("gtk_widget_realize(%lx\n",widget);

  if(!widget) {
    DebOut("WARNING: widget is NULL\n");
    return;
  }

  if(!widget->window) {
    win=g_new0(GdkWindow,1);
    /* and store it dirty */
    win->mgtk_widget=(APTR) widget;
  }
}
#endif

#if 0
void mgtk_container_add (GtkContainer *container, GtkWidget *button) {

  DebOut("mgtk_container_add(window %lx, button %lx)\n",container,button);

  if(container == NULL) {
    ErrOut("gtk_container_add: window is NULL!?\n");
  }

/*
  DoMethod(container,MUIM_Group_InitChange);
  DoMethod(container,OM_ADDMEMBER,button->MuiObject);
  DoMethod(container,MUIM_Group_ExitChange);
  */

/* was:
  mgtk_add_child((APTR) container,button);
   now*
  button->parent=GTK_WIDGET(container);
*/

  if(!GtkObj(container) || !GtkObj(button)) {
    ErrOut("mgtk_container_add: One widget has no Mui Object (%lx->%lx,%lx->%lx)!\n",container,GtkObj(container),button,GtkObj(button));
    return;
  }

  mgtk_initchange(button);
  DoMethod(GtkObj(container),OM_ADDMEMBER,(LONG) GtkObj(button));
  mgtk_exitchange(button);

  return;
}
#endif

/******************************************
 * mgtk_add(parent,son);
 ******************************************/
void mgtk_add(GtkMui *parent, GtkMui *son) {
  DebOut("mgtk_add(%lx,%lx)\n",parent,son);

  if(!GtkObj(parent) || !GtkObj(son)) {
    ErrOut("mgtk_container_add: One widget has no Mui Object (%lx->%lx,%lx->%lx)!\n",parent,GtkObj(parent),son,GtkObj(son));
    return;
  }
  
  mgtk_initchange(GTK_WIDGET(parent));
  DoMethod(GtkObj(parent),OM_ADDMEMBER,(LONG) GtkObj(son));
  mgtk_exitchange(GTK_WIDGET(parent));

  return;
}
/******************************************
 * mgtk_check_windows()
 *
 * If someone destroyes the last Gtk Window
 * the application should quit.
 *
 * So we check here, if there are any
 * windows left.
 ******************************************/
void mgtk_check_windows() {

  GETGLOBAL

  if(g_slist_length(mgtk->windows)==0) {
    /* no windows anymore */
    DebOut("  all windows removed\n");
    gtk_main_quit();
  }
}

#if 0
/* not needed anymore ? */
void mgtk_send_delayed_signals() {
  int i;
  mgtk_stored_signal *s;
  GETGLOBAL

  if(!mgtk->delayed_signals) {
    return;
  }

  DebOut("mgtk_send_delayed_signals()\n");

  for(i=0;i<g_list_length(mgtk->delayed_signals);i++) {
    if(!gtk_do_main_quit) {
      s=(mgtk_stored_signal *) g_list_nth_data(mgtk->delayed_signals,i);
      DebOut(" send delayed signal nr %d..\n",i);

      gtk_signal_emit(s->object,s->signal_id,s->type);
    }
  }

  g_list_free(mgtk->delayed_signals);
  mgtk->delayed_signals=NULL;
}
#endif



void Close_Libs(void);

void gtk_main(void) {
  ULONG sigs = 0;
  ULONG iconified;
  ULONG return_id;
  GtkWidget *widget;
  GETGLOBAL

  DebOut("gtk_main()\n");

  //while (DoMethod(mgtk->MuiApp,MUIM_Application_NewInput,(LONG) &sigs) != MUIV_Application_ReturnID_Quit) {
  while (TRUE) {
    return_id=DoMethod(mgtk->MuiApp,MUIM_Application_NewInput,(LONG) &sigs);
    /*
     * this is now done in gtk_widget_destroy ..
     * mgtk_check_windows();*
     */

    if(gtk_do_main_quit) {
      break;
    }

    if (sigs) {
      sigs = Wait(sigs | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
      if (sigs & SIGBREAKF_CTRL_C) {
        break;
      }
      /* the SIGBREAKF_CTRL_F is generated by the broker */
      if (sigs & SIGBREAKF_CTRL_F) {
        DebOut("SIGBREAKF_CTRL_F\n");
        iconified=!xget(global_mgtk->MuiApp, MUIA_Application_Iconified);
        set(global_mgtk->MuiApp, MUIA_Application_Iconified, iconified);
        widget=mgtk_get_widget_from_obj(global_mgtk->MuiRoot);
        if(iconified) {
          gtk_signal_emit_by_name (GTK_MUI (widget), "commodity-hide");
        }
        else {
          gtk_signal_emit_by_name (GTK_MUI (widget), "commodity-show");
        }
      }
    }
#if 0
    mgtk_send_delayed_signals();
#endif
  }

  DebOut("gtk_main left while loop..\n");

  DebOut("IntuitionBase: %lx\n",IntuitionBase);
  DebOut("GfxBase: %lx\n",GfxBase);
  DebOut("GadToolsBase: %lx\n",GadToolsBase);

  DebOut("set(%lx,MUIA_Window_Open,FALSE)\n",global_mgtk->MuiRoot);
  set(global_mgtk->MuiRoot,MUIA_Window_Open,FALSE);

  DebOut("  Free Screen resources\n");

  if (global_mgtk->screen) {
    if (global_mgtk->dri) {
      DebOut("global_mgtk->visualinfo: %lx\n",global_mgtk->visualinfo);
      if (global_mgtk->visualinfo) {
        FreeVisualInfo(global_mgtk->visualinfo);
      }
      FreeScreenDrawInfo(global_mgtk->screen, global_mgtk->dri);
    }
    UnlockPubScreen(NULL, global_mgtk->screen);
    global_mgtk->screen=NULL;
  }

  if(global_mgtk->white_black_optained) {
    if ((global_mgtk->screen = LockPubScreen(0))) {
      ReleasePen(global_mgtk->screen->ViewPort.ColorMap, global_mgtk->white_pen);
      ReleasePen(global_mgtk->screen->ViewPort.ColorMap, global_mgtk->black_pen);
      global_mgtk->white_black_optained=0;
      UnlockPubScreen(NULL, global_mgtk->screen);
      global_mgtk->screen=NULL;
    }
  }
 
/*
  ReleasePen(ViewAddress()->ViewPort->ColorMap,mgtk->black_pen);
  ReleasePen(ViewAddress()->ViewPort->ColorMap,mgtk->white_pen);
  */

  DebOut("  MUI_DisposeObject(mgtk->MuiApp)\n");
  MUI_DisposeObject(global_mgtk->MuiApp);

  /* free icon */
  if(global_mgtk->icon) {
    FreeDiskObject(global_mgtk->icon);
  }

#ifndef __MORPHOS__
  DebOut("  FreeRemember & CloseLibs\n");
  FreeRemember(&rememberKey,TRUE);
  Close_Libs();
#endif
#if defined ENABLE_RT
  RT_Exit();
#endif
}

void mgtk_optionmenu_pack(GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding ) {
  WarnOut("mgtk_optionmenu_pack missing at the moment!\n");
}

void gtk_main_quit (void) {
  DebOut("gtk_main_quit()\n");

  gtk_do_main_quit=1;
}


/* does not work !? */
void mgtk_widget_set_size_request(GtkWidget           *widget,
                                 gint                 width,
                                 gint                 height) {
  DebOut("mgtk_widget_set_size_request(%lx,%ld,%ld)\n",widget,width,height);

/*gtk_widget_set_size_request is disabled in parts.. */
#if 0
  if(widget->type == IS_WINDOW) {
    DebOut("MuiWin: %lx\n",widget->MuiWin);

    SetAttrs(widget->MuiObject,
      MA_RootGroup_DefWidth, width,
      MA_RootGroup_DefHeight, height,
    TAG_DONE);
  }
  else {
    printf("  gtk_widget_set_size_request on non window!?\n");
    if(width>0) {
      SetAttrs(widget->MuiObject,MUIA_FixWidth,width, TAG_DONE);
      g_object_set_data(widget,"MUIA_FixWidth",GINT_TO_POINTER(width));
    }
    if(height>0) {
      SetAttrs(widget->MuiObject,MUIA_FixHeight,height, TAG_DONE);
      g_object_set_data(widget,"MUIA_FixHeight",GINT_TO_POINTER(height));
    }
  }
#endif

  /* if MUI object doesnt support this... too bad
  ** however using MUIA_FixWidth/MUIA_FixHeight is not sufficient
  */

  SetAttrs(GtkObj(widget),
    MA_DefWidth, width,
    MA_DefHeight, height,
  TAG_DONE);
}

#if 0
/* Deprecated: Use gtk_widget_set_size_request() instead. */
void gtk_widget_set_usize(GtkWidget *widget, gint width, gint height) {

  DebOut("gtk_widget_set_usize(%lx,%d,%d)\n",widget,width,height);

  gtk_widget_set_size_request(widget,width,height);
}
#endif

/* not perfect.. */
void gtk_widget_grab_default (GtkWidget *widget) {

  DebOut("gtk_widget_grab_default(%lx)\n",widget);
	DebOut("  disabled\n");

  // only area class objects can become default objects
/*
	DebOut("  muiobj: %lx\n",widget->MuiObject);

  if (widget->mainclass == CL_AREA && _win(widget->MuiObject))
  {
    set(_win(widget->MuiObject), MUIA_Window_DefaultObject, (ULONG)widget->MuiObject);
  }
  else
  {
    WarnOut("gtk_widget_grab_default(): not in window or object is wrong type\n");
  }
	*/

  return;
}

void mgtk_widget_set_state(GtkWidget *widget, GtkStateType state) {

  DebOut("mgtk_widget_set_state(%lx,%d)\n",widget,state);

  if(!GtkObj(widget)) {
    ErrOut("widget->MuiObject is NULL!\n");
    return;
  }

  switch(state) {
    case GTK_STATE_NORMAL:
      set(GtkObj(widget),MUIA_Selected,(ULONG) FALSE);
      return;
    case GTK_STATE_ACTIVE:
    case GTK_STATE_SELECTED:
      set(GtkObj(widget),MUIA_Selected,(ULONG) TRUE);
      return;
    case GTK_STATE_PRELIGHT:
      return;
    case GTK_STATE_INSENSITIVE:
      gtk_widget_set_sensitive(widget,FALSE);
      return;
  }
}

#if 0
void gtk_widget_unref(GtkWidget *widget) {
  DebOut("gtk_widget_unref(%lx) is just a fake\n",widget);
  return;
}

GtkWidget *gtk_widget_ref(GtkWidget *widget) {
  DebOut("gtk_widget_ref(%lx) is just a fake\n",widget);
  return (GtkWidget *) widget;
}
#endif

/* not needed any more !?
#include <gtk/gtkwidget.h>
gpointer gtk_type_class(GtkType type) {

  GtkWidgetClass *ret;

  DebOut("gtk_type_class(%ld)\n",type);

  DebOut("WARNING: gtk_type_class is just a dummy returning empty Class\n");

  if(type != 0) {
    DebOut("WARNING: unkown widget type %d supplied to gtk_type_class\n");
  }
  ret=g_new(GtkWidgetClass,1);

  return ret;
}
*/

#if 0
/* well, maybe a function to set the events would be
 * more usefull ;)
 */
gint gtk_widget_get_events(GtkWidget *widget) {

  DebOut("gtk_widget_get_events(%lx)\n",widget);

  return widget->gdkevents;
}
#endif

#if 0
GdkVisual *mgtk_widget_get_visual(GtkWidget *widget) {

  DebOut("mgtk_widget_get_visual(%lx)\n",widget);

  if(!widget->visual) {
    widget->visual=g_new(GdkVisual,1);
  }

  /* we might need that! */
  widget->visual->mgtk_widget=(int *) widget;

  return widget->visual;
}
#endif

void mgtk_widget_queue_draw(GtkWidget *widget) {
  DebOut("gtk_widget_queue_draw(%lx)\n");

  if(GtkObj(widget)) {
    DoMethod(GtkObj(widget),MUIM_Group_InitChange);
    DoMethod(GtkObj(widget),MUIM_Group_ExitChange);
  }
  else {
    DebOut(" WARNING: gtk_widget_queue_draw called on non MUI widget, what to do?n");
  }
}

#if 0
void gtk_widget_style_get(GtkWidget *widget, const gchar *first_property_name, ...) {

  DebOut("gtk_widget_style_get(%lx,..)\n",widget);

  DebOut("WARNING: Just a dummy\n");
}
#endif

/* Sets the GtkStyle for a widget (widget->style). You probably 
 * don't want to use this function; it interacts badly with 
 * themes, because themes work by replacing the GtkStyle.
 */
void gtk_widget_set_style(GtkWidget *widget, GtkStyle *style) {

  GETGLOBAL

  DebOut("gtk_widget_set_style(%lx,%lx)\n",widget,style);

  if(!style) {
    DebOut("  style is NULL => set default style\n");
    widget->style=mgtk_get_default_style(mgtk->MuiRoot);
  }
  else {
    widget->style=style;
    g_object_ref(G_OBJECT(style));
  }
}
#ifdef NEED_GLIB
/* gtk_widget_set_rc_style is deprecated and should not be 
 * used in newly-written code. 
 */
void gtk_widget_set_rc_style(GtkWidget *widget) {

  DebOut("gtk_widget_set_rc_style(%lx)\n",widget);

  gtk_widget_set_style(widget,NULL);
}
#endif

/* remove this.. */
GType gdk_gc_get_type() {
    return GTK_TYPE_MUI;
}


void GtkSetObj(GtkWidget *widget, APTR obj) {
  APTR old;

  old=GTK_MUI(widget)->MuiObject;

  if(old) {
    DebOut("GtkSetObj(%s %lx,%lx): release old object %lx\n",gtk_widget_get_name(widget),widget,obj,old);
    if(xget(obj,MUIA_Parent)) {
      ErrOut("GtkSetObj(%lx,%lx): old object %lx has a parent (%lx)!\n",widget,obj,old,xget(obj,MUIA_Parent));
    }
    GTK_MUI(widget)->MuiObject=NULL;
    DisposeObject(old);
  }

  GTK_MUI(widget)->MuiObject=obj;
}

void mgtk_mui_list_obj(APTR obj, gchar *space) {
  struct MinList *childs;
  gchar *newspace;
  APTR i;
  int anz;
  
  DebOut("mgtk_mui_list_obj(%lx)\n",obj);

  if(!obj) {
    return;
  }
  
  DebOut("%s (%lx)\n",space,obj);

  childs=(struct MinList *) xget(obj,MUIA_Family_List);

  DebOut("childs: %lx\n",childs);

  if(!childs) {
    return;
  }

  ListLength(childs,anz);

  DebOut("  nr childs: %d\n",anz);

  if(!childs) {
    return;
  }


  newspace=g_strdup_printf("%s   ",space);

  //object_state = mydata->md_CollectionList.lh_Head;
  while (( i = NextObject(&childs) )) {
    mgtk_mui_list_obj(i,newspace);
  }

  g_free(newspace);
}

void gtk_mui_list_obj(APTR obj) {
  gchar *space;

  DebOut("vvvvvvvvvvv gtk_mui_list_obj vvvvvvvvvvv\n");

  space=g_strdup("  ");
  mgtk_mui_list_obj(obj, space);
  g_free(space);
  DebOut("^^^^^^^^^^^                  ^^^^^^^^^^^\n");
}

void mgtk_mui_list(GtkWidget *widget, gchar *space) {
  GList *childs;
  int i;
  gchar *newspace;
  
  DebOut("%s%s (%lx)\n",space,g_type_name(G_OBJECT_TYPE (widget)),widget);

  if(GTK_IS_CONTAINER(widget)) {

    childs=gtk_container_get_children(GTK_CONTAINER(widget));
    newspace=g_strdup_printf("%s   ",space);

    for(i=0;i<g_list_length(childs);i++) {
      mgtk_mui_list(GTK_WIDGET(g_list_nth(childs,i)->data),newspace);
    }

    g_free(newspace);
  }
}

void gtk_mui_list(GtkMui *widget) {
  gchar *space;

  DebOut("vvvvvvvvvvv gtk_mui_list vvvvvvvvvvv\n");

  if(!GTK_IS_CONTAINER(widget)) {
    DebOut("%lx is not a container!\n");
  }

  space=g_strdup("  ");
  mgtk_mui_list(GTK_WIDGET(widget), space);
  g_free(space);
  DebOut("^^^^^^^^^^^              ^^^^^^^^^^^\n");
}


/*******************************************************
 * gtk_mui_list_child_tree
 *
 * try to dump the MUI object tree to DEBOUT
 * if an object is part of a widget, add widegt name
 *
 * nearly the same as above :(, sometimes you forget
 * your own helper functions ;). But this one is
 * nicer.
 *******************************************************/
static void gtk_mui_list_child_tree_int(APTR obj,int i) {
#if defined(MGTK_DEBUG)
  struct List *list;
  struct Node *state;
  Object *o;
  gchar *pre;
  const gchar *name;
  GtkWidget *act;

  list=(struct List *) xget(obj,MUIA_Group_ChildList);
  if(list) {
    state = list->lh_Head;
    if(state) {
      pre=g_strnfill(i*2,' ');
      while ( (o = NextObject( &state )) ) {
	act=(GtkWidget *) xget(o,MA_Widget);
	if(act) {
	  name=gtk_widget_get_name(act);
	  DebOut("%schild: %lx (%s)\n",pre,o,name);
	}
	else {
	  DebOut("%schild: %lx\n",pre,o);
	}
	gtk_mui_list_child_tree_int(o,i+1);
      }
      g_free(pre);
    }
  }
#endif
}

void gtk_mui_list_child_tree(GtkMui *w) {

  DebOut("---------------V-----------------\n");
  DebOut("mgtk_debug_list_child_tree(%lx)\n",GtkObj(w));
  gtk_mui_list_child_tree_int(GtkObj(w),1);
  DebOut("---------------^-----------------\n");
}


void gtk_mui_application_iconify(gboolean state) {

  DebOut("gtk_mui_application_iconify(%lx, %d)\n");
  set(global_mgtk->MuiApp, MUIA_Application_Iconified, state);
}


gboolean gtk_mui_application_is_iconified(void) {

	ULONG iconified;

	iconified=xget(global_mgtk->MuiApp, MUIA_Application_Iconified);

	DebOut("gtk_mui_application_is_iconified: %d\n", iconified);

	return (gboolean) iconified;
}

void gtk_mui_application_gui_hotkey(char *cx_popkey) {

  CxObj *popfilter;
  CxObj *broker;
  struct MsgPort *brokermp;

  DebOut("gtk_mui_application_sethotkey(%s)\n", cx_popkey);

  broker  =xget(global_mgtk->MuiApp, MUIA_Application_Broker);
  brokermp=xget(global_mgtk->MuiApp, MUIA_Application_BrokerPort);

  if(!broker || !brokermp) {
    DebOut("WARNING: could not create broker!\n");
    return;
  }

  popfilter = CxFilter(cx_popkey);

  if (popfilter) {
    CxObj *popsig = CxSignal(FindTask(NULL), SIGBREAKB_CTRL_F);
    if (popsig) {
      CxObj *trans;
      AttachCxObj(popfilter, popsig);
      trans = CxTranslate(NULL);
      if (trans) {
        AttachCxObj(popfilter, trans);
      }
    }
    AttachCxObj(broker, popfilter);
  }
}

gint gtk_mui_application_icon(char *iconname) {

  struct DiskObject *diskobj;
  char iconname2[256];
  char *dot;

  DebOut("icon: %s\n", iconname);

  diskobj=GetDiskObject(iconname);

  if(!diskobj && strlen(iconname)>5) {
    /* strip extension */
    strncpy(iconname2, iconname, 255);
    dot=rindex(iconname2, '.');
    if(dot && !strcasecmp(dot, ".info")) {
      DebOut("stripping .info..\n");
      dot=(char) 0;
      diskobj=GetDiskObject(iconname2);
    }
  }

  if(!diskobj) {
    DebOut("Unable to open %s or %s with GetDiskObject!\n", iconname, iconname2);
    return FALSE;
  }

  set(global_mgtk->MuiApp, MUIA_Application_DiskObject, diskobj);

  /* free old one, if we already had one */
  if(global_mgtk->icon) {
    FreeDiskObject(global_mgtk->icon);
  }

  global_mgtk->icon=diskobj;

  return TRUE;
}
