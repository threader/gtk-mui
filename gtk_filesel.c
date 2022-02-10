/* GTK - The GIMP Toolkit
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
 * $Id: gtk_filesel.c,v 1.12 2007/03/19 09:00:06 o1i Exp $
 */

/* 
 * at the moment, gtk_widget_show called on a file requester
 * opens a asl request (and so blocks the main task).
 * This works in most cases, but sometimes fails badly,
 * if the GTK program relies on paralell execution.
 * Would be better to run the requester async and just
 * set the main app to sleep mode.
 */
#include <stdio.h>
#include <string.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <libraries/asl.h>
#include <mui.h>

#include "debug.h"
#include "gtk/gtk.h"
#include "gtk_globals.h"
#include "gtk/gtkfilesel.h"

enum {
  PROP_0,
  PROP_SHOW_FILEOPS,
  PROP_FILENAME,
  PROP_SELECT_MULTIPLE
};

enum {
  DIR_COLUMN
};

enum {
  FILE_COLUMN
};

static void gtk_file_selection_class_init    (GtkFileSelectionClass *klass);
static void gtk_file_selection_set_property  (GObject         *object,
					      guint            prop_id,
					      const GValue    *value,
					      GParamSpec      *pspec);
static void gtk_file_selection_get_property  (GObject         *object,
					      guint            prop_id,
					      GValue          *value,
					      GParamSpec      *pspec);
static void gtk_file_selection_init          (GtkFileSelection      *filesel);

static GtkWindowClass *parent_class = NULL;

GType gtk_file_selection_get_type (void) {

  static GType file_selection_type = 0;

  DebOut("gtk_file_selection_get_type()\n");

  if (!file_selection_type) {
    static const GTypeInfo filesel_info = {
      sizeof (GtkFileSelectionClass),
      NULL,		/* base_init */
      NULL,		/* base_finalize */
      (GClassInitFunc) gtk_file_selection_class_init,
      NULL,		/* class_finalize */
      NULL,		/* class_data */
      sizeof (GtkFileSelection),
      0,		/* n_preallocs */
      (GInstanceInitFunc) gtk_file_selection_init,
    };

    file_selection_type = g_type_register_static (GTK_TYPE_DIALOG, "GtkFileSelection",
      &filesel_info, 0);
  }

  return file_selection_type;
}

static void gtk_file_selection_class_init (GtkFileSelectionClass *class) {

  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  DebOut("gtk_file_selection_class_init(%lx)\n",class);

  gobject_class = (GObjectClass*) class;
  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;

  parent_class = g_type_class_peek_parent (class);

#if 0
  gobject_class->finalize = gtk_file_selection_finalize;
#endif
  gobject_class->set_property = gtk_file_selection_set_property;
  gobject_class->get_property = gtk_file_selection_get_property;
   
  g_object_class_install_property (gobject_class,
                                   PROP_FILENAME,
                                   g_param_spec_string ("filename",
                                                        P_("Filename"),
                                                        P_("The currently selected filename"),
                                                        NULL,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
				   PROP_SHOW_FILEOPS,
				   g_param_spec_boolean ("show_fileops",
							 P_("Show file operations"),
							 P_("Whether buttons for creating/manipulating files should be displayed"),
							 FALSE,
							 G_PARAM_READABLE |
							 G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
				   PROP_SELECT_MULTIPLE,
				   g_param_spec_boolean ("select_multiple",
							 P_("Select multiple"),
							 P_("Whether to allow multiple files to be selected"),
							 FALSE,
							 G_PARAM_READABLE |
							 G_PARAM_WRITABLE));
#if 0
  object_class->destroy = gtk_file_selection_destroy;
  widget_class->map = gtk_file_selection_map;
#endif
}

static void gtk_file_selection_set_property (GObject         *object,
					     guint            prop_id,
					     const GValue    *value,
					     GParamSpec      *pspec)
{
  GtkFileSelection *filesel;

  filesel = GTK_FILE_SELECTION (object);

  switch (prop_id)
    {
    case PROP_FILENAME:
      gtk_file_selection_set_filename (filesel,
                                       g_value_get_string (value));
      break;
    case PROP_SHOW_FILEOPS:
      if (g_value_get_boolean (value))
        gtk_file_selection_show_fileop_buttons (filesel);
#if 0
      else
        gtk_file_selection_hide_fileop_buttons (filesel);
      break;
    case PROP_SELECT_MULTIPLE:
      gtk_file_selection_set_select_multiple (filesel, g_value_get_boolean (value));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_file_selection_get_property (GObject         *object,
					     guint            prop_id,
					     GValue          *value,
					     GParamSpec      *pspec)
{
  GtkFileSelection *filesel;

  filesel = GTK_FILE_SELECTION (object);

  switch (prop_id)
    {
    case PROP_FILENAME:
      g_value_set_string (value,
                          gtk_file_selection_get_filename(filesel));
      break;

    case PROP_SHOW_FILEOPS:
      /* This is a little bit hacky, but doing otherwise would require
       * adding a field to the object.
       */
      g_value_set_boolean (value, (filesel->fileop_c_dir && 
				   filesel->fileop_del_file &&
				   filesel->fileop_ren_file));
      break;
#if 0
    case PROP_SELECT_MULTIPLE:
      g_value_set_boolean (value, gtk_file_selection_get_select_multiple (filesel));
      break;
#endif
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void gtk_file_selection_init (GtkFileSelection *filesel) {

  DebOut("gtk_file_selection_init(%lx)\n",filesel);

  /*  The Cancel button  */
  filesel->cancel_button = gtk_button_new_with_label ("Cancel");
  /*  The OK button  */
  filesel->ok_button = gtk_button_new_with_label ("Ok");

  gtk_file_selection_show_fileop_buttons (filesel);
#if 0
  GtkWidget *entry_vbox;
  GtkWidget *label;
  GtkWidget *list_hbox, *list_container;
  GtkWidget *confirm_area;
  GtkWidget *pulldown_hbox;
  GtkWidget *scrolled_win;
  GtkWidget *eventbox;
  GtkWidget *spacer;
  GtkDialog *dialog;

  GtkListStore *model;
  GtkTreeViewColumn *column;
  
  gtk_widget_push_composite_child ();

  dialog = GTK_DIALOG (filesel);

  filesel->cmpl_state = cmpl_init_state ();

  /* The dialog-sized vertical box  */
  filesel->main_vbox = dialog->vbox;
  gtk_container_set_border_width (GTK_CONTAINER (filesel), 10);

  /* The horizontal box containing create, rename etc. buttons */
  filesel->button_area = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (filesel->button_area), GTK_BUTTONBOX_START);
  gtk_box_set_spacing (GTK_BOX (filesel->button_area), 0);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), filesel->button_area, 
		      FALSE, FALSE, 0);
  gtk_widget_show (filesel->button_area);
  
  gtk_file_selection_show_fileop_buttons (filesel);

  /* hbox for pulldown menu */
  pulldown_hbox = gtk_hbox_new (TRUE, 5);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), pulldown_hbox, FALSE, FALSE, 0);
  gtk_widget_show (pulldown_hbox);
  
  /* Pulldown menu */
  filesel->history_pulldown = gtk_option_menu_new ();
  gtk_widget_show (filesel->history_pulldown);
  gtk_box_pack_start (GTK_BOX (pulldown_hbox), filesel->history_pulldown, 
		      FALSE, FALSE, 0);
    
  /*  The horizontal box containing the directory and file listboxes  */

  spacer = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_size_request (spacer, -1, 5);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), spacer, FALSE, FALSE, 0);
  gtk_widget_show (spacer);
  
  list_hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), list_hbox, TRUE, TRUE, 0);
  gtk_widget_show (list_hbox);
  if (WANT_HPANED)
    list_container = g_object_new (GTK_TYPE_HPANED,
				   "visible", TRUE,
				   "parent", list_hbox,
				   "border_width", 0,
				   NULL);
  else
    list_container = list_hbox;

  spacer = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_size_request (spacer, -1, 5);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), spacer, FALSE, FALSE, 0);  
  gtk_widget_show (spacer);
  
  /* The directories list */

  model = gtk_list_store_new (1, G_TYPE_STRING);
  filesel->dir_list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
  g_object_unref (model);

  column = gtk_tree_view_column_new_with_attributes (_("Folders"),
						     gtk_cell_renderer_text_new (),
						     "text", DIR_COLUMN,
						     NULL);
  label = gtk_label_new_with_mnemonic (_("Fol_ders"));
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), filesel->dir_list);
  gtk_widget_show (label);
  gtk_tree_view_column_set_widget (column, label);
  gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (filesel->dir_list), column);

  gtk_widget_set_size_request (filesel->dir_list,
			       DIR_LIST_WIDTH, DIR_LIST_HEIGHT);
  g_signal_connect (filesel->dir_list, "row_activated",
		    G_CALLBACK (gtk_file_selection_dir_activate), filesel);

  /*  gtk_clist_column_titles_passive (GTK_CLIST (filesel->dir_list)); */

  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win), GTK_SHADOW_IN);  
  gtk_container_add (GTK_CONTAINER (scrolled_win), filesel->dir_list);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_win), 0);
  if (GTK_IS_PANED (list_container))
    gtk_paned_pack1 (GTK_PANED (list_container), scrolled_win, TRUE, TRUE);
  else
    gtk_container_add (GTK_CONTAINER (list_container), scrolled_win);
  gtk_widget_show (filesel->dir_list);
  gtk_widget_show (scrolled_win);

  /* The files list */
  model = gtk_list_store_new (1, G_TYPE_STRING);
  filesel->file_list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
  g_object_unref (model);

  column = gtk_tree_view_column_new_with_attributes (_("Files"),
						     gtk_cell_renderer_text_new (),
						     "text", FILE_COLUMN,
						     NULL);
  label = gtk_label_new_with_mnemonic (_("_Files"));
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), filesel->file_list);
  gtk_widget_show (label);
  gtk_tree_view_column_set_widget (column, label);
  gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (filesel->file_list), column);

  gtk_widget_set_size_request (filesel->file_list,
			       FILE_LIST_WIDTH, FILE_LIST_HEIGHT);
  g_signal_connect (filesel->file_list, "row_activated",
		    G_CALLBACK (gtk_file_selection_file_activate), filesel);
  g_signal_connect (gtk_tree_view_get_selection (GTK_TREE_VIEW (filesel->file_list)), "changed",
		    G_CALLBACK (gtk_file_selection_file_changed), filesel);

  /* gtk_clist_column_titles_passive (GTK_CLIST (filesel->file_list)); */

  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (scrolled_win), filesel->file_list);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_win), 0);
  gtk_container_add (GTK_CONTAINER (list_container), scrolled_win);
  gtk_widget_show (filesel->file_list);
  gtk_widget_show (scrolled_win);

  /* action area for packing buttons into. */
  filesel->action_area = gtk_hbox_new (TRUE, 0);
  gtk_box_pack_start (GTK_BOX (filesel->main_vbox), filesel->action_area, 
		      FALSE, FALSE, 0);
  gtk_widget_show (filesel->action_area);
  
  /*  The OK/Cancel button area */
  confirm_area = dialog->action_area;

  /*  The Cancel button  */
  filesel->cancel_button = gtk_dialog_add_button (dialog,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_CANCEL);
  /*  The OK button  */
  filesel->ok_button = gtk_dialog_add_button (dialog,
                                              GTK_STOCK_OK,
                                              GTK_RESPONSE_OK);

  gtk_dialog_set_alternative_button_order (dialog,
					   GTK_RESPONSE_OK,
					   GTK_RESPONSE_CANCEL,
					   -1);

  gtk_widget_grab_default (filesel->ok_button);

  /*  The selection entry widget  */
  entry_vbox = gtk_vbox_new (FALSE, 2);
  gtk_box_pack_end (GTK_BOX (filesel->main_vbox), entry_vbox, FALSE, FALSE, 2);
  gtk_widget_show (entry_vbox);
  
  eventbox = gtk_event_box_new ();
  filesel->selection_text = label = gtk_label_new ("");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_container_add (GTK_CONTAINER (eventbox), label);
  gtk_box_pack_start (GTK_BOX (entry_vbox), eventbox, FALSE, FALSE, 0);
  gtk_widget_show (label);
  gtk_widget_show (eventbox);

  filesel->selection_entry = gtk_entry_new ();
  g_signal_connect (filesel->selection_entry, "key_press_event",
		    G_CALLBACK (gtk_file_selection_key_press), filesel);
  g_signal_connect (filesel->selection_entry, "insert_text",
		    G_CALLBACK (gtk_file_selection_insert_text), NULL);
  g_signal_connect_swapped (filesel->selection_entry, "changed",
			    G_CALLBACK (gtk_file_selection_update_fileops), filesel);
  g_signal_connect_swapped (filesel->selection_entry, "focus_in_event",
			    G_CALLBACK (grab_default),
			    filesel->ok_button);
  g_signal_connect_swapped (filesel->selection_entry, "activate",
			    G_CALLBACK (gtk_button_clicked),
			    filesel->ok_button);
  
  gtk_box_pack_start (GTK_BOX (entry_vbox), filesel->selection_entry, TRUE, TRUE, 0);
  gtk_widget_show (filesel->selection_entry);

  gtk_label_set_mnemonic_widget (GTK_LABEL (filesel->selection_text),
				 filesel->selection_entry);

  if (!cmpl_state_okay (filesel->cmpl_state))
    {
      gchar err_buf[256];

      g_snprintf (err_buf, sizeof (err_buf), _("Folder unreadable: %s"), cmpl_strerror (cmpl_errno));

      gtk_label_set_text (GTK_LABEL (filesel->selection_text), err_buf);
    }
  else
    {
      gtk_file_selection_populate (filesel, "", FALSE, TRUE);
    }

  gtk_widget_grab_focus (filesel->selection_entry);

  gtk_widget_pop_composite_child ();
#endif
}

#define OK_WIDGET 1;
#define CANCEL_WIDGET 2;

GtkWidget* gtk_file_selection_new(const gchar *title){

  GETGLOBAL
  GtkFileSelection *ret;
	GSList *gl;

  DebOut("gtk_file_selection_new(%s)\n",title);

  ret=g_object_new (GTK_TYPE_FILE_SELECTION, NULL);

	GTK_WINDOW(ret)->title=g_strdup(title);

	/* we store in draw_value, which button this is */
#if 0
	ok->draw_value=OK_WIDGET;
	cancel->draw_value=CANCEL_WIDGET;

	ret->ok_button=(APTR) ok;
	ret->cancel_button=(APTR) cancel;

	mgtk_add_child(ret,ok);
	mgtk_add_child(ret,cancel);
#endif
        /* we don't use them, but someonw might call sth. like gtk_signal_connect on those */

	gl=mgtk->windows;
	gl=g_slist_append(gl,ret);
	mgtk->windows=gl;

  return GTK_WIDGET(ret);
}

const gchar* gtk_file_selection_get_filename(GtkFileSelection *filesel) {

	DebOut("gtk_file_selection_get_filename(%lx)\n",filesel);

  if(!filesel) {
    ErrOut("gtk_file_selection_get_filename called with NULL\n");
    return NULL;
  }

  DebOut("  filesel->fileop_file: >%s<\n",filesel->fileop_file);

	return filesel->fileop_file;
}

void gtk_file_selection_set_filename (GtkFileSelection *filesel, const gchar *filename) {

	DebOut("gtk_file_selection_set_filename(%lx,%s)\n",filesel,filename);

	filesel->fileop_file=g_strdup(filename);

	return;
}

/*
 * this is the main function:
 * - show file selector with all stored parameters
 * - call the stored callbacks for ok/cancel
 */
void mgtk_file_selection_show(GtkWidget *widget) {
  GETGLOBAL
	struct FileRequester *req;
	char file[256];
	char drawer[256];
	char *title;
  char *tmp;
	GSList *gl;
  int ok;
  int len;
  struct Window *win;

	DebOut("mgtk_file_selection_show(%lx)\n",widget);

  /* the following tries to handle fileop_file strings:
     - if it starts with '~/' we let it start with PROGDIR:
     - if it contains a '/' we assume, we get the path from start to last '/'
     - if it contains no '/' and  a ':' we assume, we get the path from start to last ':'
     - if it is empty, we use "PROGDIR:"
   */

	if(GTK_FILE_SELECTION(widget)->fileop_file) {   /* we already have a filename */
    /* replace ~/ with PROGDIR: */
    if((GTK_FILE_SELECTION(widget)->fileop_file[0] == '~') &&
       (GTK_FILE_SELECTION(widget)->fileop_file[1] == '/')) {
      DebOut("  found tilde: %s\n",GTK_FILE_SELECTION(widget)->fileop_file);
      tmp=g_strdup_printf("PROGDIR:%s",GTK_FILE_SELECTION(widget)->fileop_file+2);
      g_free(GTK_FILE_SELECTION(widget)->fileop_file);
      GTK_FILE_SELECTION(widget)->fileop_file=tmp;
      tmp=NULL;
      DebOut("  replaced tilde with PROGDIR: %s\n",GTK_FILE_SELECTION(widget)->fileop_file);
    }

    /* get path and filename */
    if(strrchr(GTK_FILE_SELECTION(widget)->fileop_file, '/')) {
      len = strrchr(GTK_FILE_SELECTION(widget)->fileop_file, '/') - GTK_FILE_SELECTION(widget)->fileop_file + 1;
    }
    else if(strrchr(GTK_FILE_SELECTION(widget)->fileop_file, ':')) {
      len = strrchr(GTK_FILE_SELECTION(widget)->fileop_file, ':') - GTK_FILE_SELECTION(widget)->fileop_file + 1;
    }
    else {
      len = 0;
    }

    if(len>254) {
      ErrOut("gtk_filesel.c: path is too long (%d)\n",len);
      len=254;
    }
    strncpy(drawer, GTK_FILE_SELECTION(widget)->fileop_file, len);
    drawer[len]='\0';
    strcpy(file,    GTK_FILE_SELECTION(widget)->fileop_file+len);
	}
	else {
		strcpy(file,"");
    strcpy(drawer,"PROGDIR:");
	}
  DebOut("drawer: %s\n",drawer);
  DebOut("file  : %s\n",file);

  if(GTK_WINDOW(widget)->title) {
    title=g_strdup(GTK_WINDOW(widget)->title);
  }
  else {
    DebOut("  title was empty\n");
    title=g_strdup("Choose file");
  }

  win=(struct Window *) xget(mgtk->MuiRoot, MUIA_Window_Window);
  DebOut("win=%lx\n",win);

  if(win) {
    req=MUI_AllocAslRequestTags(ASL_FileRequest,
                ASLFR_Window,win ,
                ASLFR_TitleText, title,
                ASLFR_InitialDrawer  , drawer,
                ASLFR_InitialFile, file,
/*                ASLFR_InitialPattern , "#?.iff",*/
/*                ASLFR_DoSaveMode     , save,
                  ASLFR_DoPatterns     , TRUE,*/
                ASLFR_RejectIcons    , TRUE,
                ASLFR_UserData       , widget,
                TAG_DONE);
  }
  else {
    WarnOut("gtk_filesel.c: mgtk->MuiRoot is NULL\n");
    req=MUI_AllocAslRequestTags(ASL_FileRequest,
                ASLFR_TitleText, title,
                ASLFR_InitialDrawer  , drawer,
                ASLFR_InitialFile, file,
/*                ASLFR_InitialPattern , "#?.iff",*/
/*                ASLFR_DoSaveMode     , save,
                  ASLFR_DoPatterns     , TRUE,*/
                ASLFR_RejectIcons    , TRUE,
                ASLFR_UserData       , widget,
                TAG_DONE);
  }

  if(req) {
    set(mgtk->MuiApp,MUIA_Application_Sleep,TRUE);

    DebOut("  sleep mode activated (req=%lx)\n",req);

    if (MUI_AslRequestTags(req,TAG_DONE)) {
      /* OK */
      ok=1;
      /* free old string */
      if(GTK_FILE_SELECTION(widget)->fileop_file) {
        DebOut("  free old fileop_file: %s\n",GTK_FILE_SELECTION(widget)->fileop_file);
        g_free(GTK_FILE_SELECTION(widget)->fileop_file);
        GTK_FILE_SELECTION(widget)->fileop_file=NULL;
      }

      if (*req->fr_File) {
        DebOut("  selected drawer:  >%s<\n",req->fr_Drawer);
        DebOut("  selected file: >%s<\n",req->fr_File);

        len=strlen(req->fr_Drawer);
        DebOut("  dir strlen=%d\n",len);
        if(len>0) {
          DebOut("  last char: %c\n",req->fr_Drawer[len-1]);
          if((req->fr_Drawer[len-1] == ':')||(req->fr_Drawer[len-1] == '/')) {
            GTK_FILE_SELECTION(widget)->fileop_file=g_strdup_printf("%s%s",req->fr_Drawer,req->fr_File);
          }
          else {
            GTK_FILE_SELECTION(widget)->fileop_file=g_strdup_printf("%s/%s",req->fr_Drawer,req->fr_File);
          }
        }
        else {
          DebOut("  drawer is empty\n");
          GTK_FILE_SELECTION(widget)->fileop_file=g_strdup(req->fr_File);
        }
        DebOut("  full filename: %s\n",GTK_FILE_SELECTION(widget)->fileop_file);
      }
      else {
        /* no file selected */
        if(*req->fr_Drawer) {
          /* but we had a drawer */
          GTK_FILE_SELECTION(widget)->fileop_file=g_strdup(req->fr_Drawer);
        }
        else {
          /* nothing selected at all */
          GTK_FILE_SELECTION(widget)->fileop_file=g_strdup("");
        }
      }
    }
    else { 
      /* CANCEL */
      ok=0;
    }

#ifndef __AROS__
    MUI_FreeAslRequest(req); /*??*/
#endif
    set(mgtk->MuiApp,MUIA_Application_Sleep,FALSE);

    if(ok) {
      g_signal_emit_by_name(GTK_FILE_SELECTION(widget)->ok_button,"clicked");
    }
    else {
      g_signal_emit_by_name(GTK_FILE_SELECTION(widget)->cancel_button,"clicked");
    }

  }
  else {
    ErrOut("gtk_filesel.c: ERROR: unable to AllocAslRequest!\n");
  }

	g_free(title);

	gl=mgtk->windows;
	gl=g_slist_remove(gl,widget);
	mgtk->windows=gl;

  /* we did it, phew. */
	return;
}

void
gtk_file_selection_show_fileop_buttons (GtkFileSelection *filesel)
{
  g_return_if_fail (GTK_IS_FILE_SELECTION (filesel));

  DebOut("gtk_file_selection_show_fileop_buttons(%lx)\n",filesel);
    
  /* delete, create directory, and rename */
  if (!filesel->fileop_c_dir) 
    {
      filesel->fileop_c_dir = gtk_button_new_with_mnemonic (_("_New Folder"));
#if 0
      g_signal_connect (filesel->fileop_c_dir, "clicked",
			G_CALLBACK (gtk_file_selection_create_dir),
			filesel);
      gtk_box_pack_start (GTK_BOX (filesel->button_area), 
			  filesel->fileop_c_dir, TRUE, TRUE, 0);
#endif
      gtk_widget_show (filesel->fileop_c_dir);
    }
	
  if (!filesel->fileop_del_file) 
    {
      filesel->fileop_del_file = gtk_button_new_with_mnemonic (_("De_lete File"));
#if 0
      g_signal_connect (filesel->fileop_del_file, "clicked",
			G_CALLBACK (gtk_file_selection_delete_file),
			filesel);
      gtk_box_pack_start (GTK_BOX (filesel->button_area), 
			  filesel->fileop_del_file, TRUE, TRUE, 0);
#endif
      gtk_widget_show (filesel->fileop_del_file);
    }

  if (!filesel->fileop_ren_file)
    {
      filesel->fileop_ren_file = gtk_button_new_with_mnemonic (_("_Rename File"));
#if 0
      g_signal_connect (filesel->fileop_ren_file, "clicked",
			G_CALLBACK (gtk_file_selection_rename_file),
			filesel);
      gtk_box_pack_start (GTK_BOX (filesel->button_area), 
			  filesel->fileop_ren_file, TRUE, TRUE, 0);
#endif
      gtk_widget_show (filesel->fileop_ren_file);
    }
  
#if 0
  gtk_file_selection_update_fileops (filesel);
#endif
  
  g_object_notify (G_OBJECT (filesel), "show-fileops");
}

