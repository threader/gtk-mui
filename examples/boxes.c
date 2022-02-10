
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: boxes.c,v 1.2 2011/07/04 12:16:21 o1i Exp $
 */

#define MGTK_DEBUG 1
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>

int main( int   argc,
          char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *fullbox;
  GtkWidget *smallframe;
  GtkWidget *smallbox;
  GtkWidget *growframe;
  GtkWidget *growbox;
  GtkWidget *label1;
  GtkWidget *smallc;

  /* Initialise GTK */
  gtk_init (&argc, &argv);
    
  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Boxes Example");

  /* Here we connect the "destroy" event to a signal handler */ 
  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_set_size_request (window, 300, 300);
  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  growframe = gtk_frame_new (NULL);
  gtk_frame_set_label(GTK_FRAME(growframe),"growing frame");
  growbox = gtk_hbox_new (FALSE,0);
  gtk_container_add (GTK_CONTAINER (growbox), growframe);
  label1=gtk_label_new("XXX biG!!!!");
  gtk_container_add (GTK_CONTAINER (growframe), label1);

  smallframe = gtk_frame_new (NULL);
  gtk_frame_set_label(GTK_FRAME(smallframe),"small frame");
  smallbox = gtk_hbox_new (FALSE,0);
  smallc=gtk_label_new("small");
  //smallc=gtk_button_new_with_label("label 2");
  gtk_container_add (GTK_CONTAINER (smallbox), smallframe);
  gtk_container_add (GTK_CONTAINER (smallframe), smallc);

  fullbox = gtk_vbox_new (FALSE,0);
  gtk_container_add (GTK_CONTAINER (window),fullbox);

  gtk_box_pack_start (GTK_BOX (fullbox), growbox, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (fullbox), smallbox, FALSE, FALSE, 0);

  /* Display the window */
  gtk_widget_show_all (fullbox);
  gtk_widget_show (window);

  label1=gtk_button_new_with_label("XXX biG!!!!");
  gtk_widget_show(label1);
  gtk_container_add (GTK_CONTAINER (growbox), label1);


  //gtk_mui_list(GTK_MUI(fullbox));

  /* Enter the event loop */
  gtk_main ();
    
  return 0;
}
