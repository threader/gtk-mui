
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: frame2.c,v 1.5 2011/07/04 12:16:21 o1i Exp $
 */

#define MGTK_DEBUG 1
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include "mui.h"

int main( int   argc,
          char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *frame2;
  GtkWidget *entry2;
  GtkWidget *label;
  GtkWidget *notebook;
  GtkWidget *vbox;
  GtkWidget *hbox2;
  GtkWidget *button2;

  /* Initialise GTK */
  gtk_init (&argc, &argv);
    
  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Frame Example");

  /* Here we connect the "destroy" event to a signal handler */ 
  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_set_size_request (window, 300, 300);
  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  notebook = gtk_notebook_new ();
  vbox = gtk_vbox_new (FALSE, 0);

  label = gtk_label_new ("test");

  frame2 = gtk_frame_new (NULL);
  entry2 = gtk_entry_new ();

//  gtk_entry_set_text (GTK_ENTRY (entry2), "THE BEAST IS HERE");
  //gtk_entry_set_text (GTK_ENTRY (entry2), "i");
  button2=gtk_button_new_with_label("grow");
  hbox2 = gtk_hbox_new (FALSE, 0);
//  gtk_widget_show(entry2);

  gtk_container_add (GTK_CONTAINER (window), notebook);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);

  /* bad frame */
  gtk_box_pack_start (GTK_BOX (vbox), frame2, TRUE, TRUE, 0);
  gtk_frame_set_label(GTK_FRAME(frame2),"this is my frame 2");

  /* frame stuff */
  gtk_container_add (GTK_CONTAINER (frame2), hbox2);
  gtk_box_pack_end (GTK_BOX (hbox2), entry2, TRUE, TRUE, 2);
  gtk_widget_show(entry2);
  gtk_box_pack_end (GTK_BOX (hbox2), button2, TRUE, TRUE, 2);
  gtk_widget_show (button2);

  gtk_widget_show (frame2);
  gtk_widget_show (notebook);
  gtk_widget_show (vbox);
  gtk_widget_show (label);
  /* if this is missing, we gt a freeze..!!!!*/
  gtk_widget_show (hbox2);

  gtk_mui_list(GTK_MUI(vbox));

  /* Display the window */
  gtk_widget_show (window);

  /* Enter the event loop */
  gtk_main ();
    
  return 0;
}
