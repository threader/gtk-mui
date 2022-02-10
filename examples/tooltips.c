/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: tooltips.c,v 1.3 2007/06/06 14:03:06 o1i Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>


void toggle_tooltips( GtkWidget *checkbutton, GtkTooltips *tt ) {

  if(GTK_TOGGLE_BUTTON (checkbutton)->active) {
    gtk_tooltips_enable(tt);
  }
  else {
    gtk_tooltips_disable(tt);
  }
}

int main( int   argc,
          char *argv[] )
{

	GtkWidget *window;

   GtkWidget *load_button, *save_button, *hbox;
   GtkWidget *check;
   GtkTooltips *button_bar_tips;

	 gtk_init (&argc, &argv);
   
   button_bar_tips = gtk_tooltips_new ();
   
   /* Create the buttons and pack them into a GtkHBox */
   hbox = gtk_hbox_new (TRUE, 2);
   
   load_button = gtk_button_new_with_label ("Load a file");
   gtk_box_pack_start (GTK_BOX (hbox), load_button, TRUE, TRUE, 2);
   gtk_widget_show (load_button);
   
   save_button = gtk_button_new_with_label ("Save a file");
   gtk_box_pack_start (GTK_BOX (hbox), save_button, TRUE, TRUE, 2);
   gtk_widget_show (save_button);

   check = gtk_check_button_new_with_label ("Tooltips on/off");
   gtk_box_pack_start (GTK_BOX (hbox), check, TRUE, TRUE, 0);
   gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
   gtk_widget_show (check);

   gtk_widget_show (hbox);
   
   /* Add the tips */
   gtk_tooltips_set_tip (GTK_TOOLTIPS (button_bar_tips), load_button,
				 "Load a new document into this window",
				 "Requests the filename of a document. This will then be loaded into the current window, replacing the contents of whatever is already loaded.");
   gtk_tooltips_set_tip (GTK_TOOLTIPS (button_bar_tips), save_button,
				 "Saves the current document to a file",
				 "If you have saved the document previously, then the new version will be saved over the old one. Otherwise, you will be prompted for a filename."); 

   g_signal_connect (G_OBJECT (check), "toggled",
                   G_CALLBACK (toggle_tooltips), (gpointer) button_bar_tips);


	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title (GTK_WINDOW (window), "GTK Tooltips");
	g_signal_connect (G_OBJECT (window), "destroy",
	                      G_CALLBACK (gtk_main_quit), NULL);

	g_signal_connect_swapped (G_OBJECT (window), "delete_event",
												G_CALLBACK (gtk_widget_destroy),
												G_OBJECT (window));
	gtk_container_add (GTK_CONTAINER (window), hbox);

	gtk_widget_show (window);
	gtk_main();
	return 0;
}

