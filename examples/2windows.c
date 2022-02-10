
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: 2windows.c,v 1.1 2011/07/04 12:19:46 o1i Exp $
 */

/*
 * this is a hbox test:
 * - two buttons in one hbox
 * - button "fixed" has a fixed width and must not grow horizontally
 * - button "expand" has expand and fill set, so it should fill all
 *   space, if the window width is enlarged
 */

#include <gtk/gtk.h>

#define GTKMUI 1
static GtkWidget *make_message_box (const gchar *title, const gchar *message, int modal) {

    GtkWidget *dialog;
    GtkWidget *vbox;
    GtkWidget *label;
    GtkWidget *hseparator;
    GtkWidget *hbuttonbox;
    GtkWidget *button;
    GtkAccelGroup *accel_group;

    accel_group = gtk_accel_group_new ();

    dialog = gtk_window_new ( GTK_WINDOW_TOPLEVEL /*GTK_WINDOW_DIALOG*/);
    gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_window_set_modal (GTK_WINDOW (dialog), modal);
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (dialog), vbox);

    label = gtk_label_new (message);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator, FALSE, FALSE, 8);

#if !defined GTKMUI
    hbuttonbox = gtk_hbutton_box_new ();
#else
    hbuttonbox = gtk_hbox_new (FALSE,4);
#endif
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, FALSE, 0);
#if !defined GTKMUI
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox), 4);
#endif

    button = gtk_button_new_with_label ("_Okay");
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
			                           GTK_SIGNAL_FUNC (gtk_widget_destroy),
			                           GTK_OBJECT (dialog));

    gtk_widget_grab_default (button);
    gtk_widget_show( dialog );

    return dialog;
}


void callback( GtkWidget *widget,
               gpointer   data )
{

	make_message_box("Test", "Test text", 0);
    
}


/* This callback quits the program */
gint delete_event( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
    gtk_main_quit ();
    return FALSE;
}


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

  button = gtk_button_new_with_label ("Open Requester");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 3);

	g_signal_connect (G_OBJECT (button), "clicked",
	              G_CALLBACK (callback), (gpointer) "button 1");

  button = gtk_button_new_with_label ("Quit");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 3);
  

	/* When the button is clicked, we call the "delete_event" function
     * and the program exits */
	g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (delete_event), NULL);


  gtk_widget_show (window);
  
  /* Rest in gtk_main and wait for the fun to begin! */
  gtk_main ();
  
  return 0;
}
