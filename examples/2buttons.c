
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: 2buttons.c,v 1.1 2007/07/30 14:44:50 o1i Exp $
 */

/*
 * this is a hbox test:
 * - two buttons in one hbox
 * - button "fixed" has a fixed width and must not grow horizontally
 * - button "expand" has expand and fill set, so it should fill all
 *   space, if the window width is enlarged
 */

#include <gtk/gtk.h>

int main( int   argc,
          char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box;

  /* Initialize the toolkit */
  gtk_init (&argc, &argv);

  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (window), "Twin Buttons");

  /* It's a good idea to do this for all windows. */
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Create a box to hold the arrows/buttons */
  box = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 2);
  gtk_container_add (GTK_CONTAINER (window), box);

  /* Pack and show all our widgets */
  gtk_widget_show (box);

  button = gtk_button_new_with_label ("Button fixed");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 3);

  button = gtk_button_new_with_label ("Button expand");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 3);
  
  gtk_widget_show (window);
  
  /* Rest in gtk_main and wait for the fun to begin! */
  gtk_main ();
  
  return 0;
}
