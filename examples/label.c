
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: label.c,v 1.2 2011/07/04 12:16:21 o1i Exp $
 */

#include <gtk/gtk.h>


int main( int   argc,
          char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *label;

  /* Initialize the toolkit */
  gtk_init (&argc, &argv);

  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (window), "Simple Oli Test");

  gtk_mui_application_gui_hotkey("ctrl alt y");

  /* It's a good idea to do this for all windows. */
  //g_signal_connect (G_OBJECT (window), "destroy",
   //                 G_CALLBACK (gtk_main_quit), NULL);

  label=gtk_label_new("Hello World??\nLine 2\nLine3");

  gtk_label_set_text(GTK_LABEL(label),"AAAAAAAAAAAAA\nBBBBBBBBBBBBB\nCCCCCCCCCCCCCCCC\nEnde");

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox);

  vbox=gtk_vbox_new (TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  gtk_widget_show (vbox);
  gtk_widget_show (label);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (window);
  
  /* Rest in gtk_main and wait for the fun to begin! */
  gtk_main ();
  
  return 0;
}

