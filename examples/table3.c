
/*
 * Modified by the GTK-MUI Team 2007
 *
 * $Id: table3.c,v 1.2 2011/07/04 12:16:21 o1i Exp $
 */

#include <gtk/gtk.h>

/* Our callback.
 * The data passed to this function is printed to stdout */
void callback( GtkWidget *widget,
               gpointer   data )
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
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
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *table;
    int x,y;

    gtk_init (&argc, &argv);


    /* Create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_show (window);

    /* Set the window title */
    gtk_window_set_title (GTK_WINDOW (window), "Big Table!");

    /* Set a handler for delete_event that immediately
     * exits GTK. */
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (delete_event), NULL);

    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);

    /* Create a 2x2 table */
    table = gtk_table_new (2, 2, TRUE);
    gtk_widget_show (table);

    /* Put the table in the main window */
    gtk_container_add (GTK_CONTAINER (window), table);

    /* nice loop */
    for(x=0;x<9;x++) {
      for(y=0;y<9;y++) {
	gchar *label;
	label=g_strdup_printf("< %d,%d >",x,y);
	printf("attach %s\n",label);
	button = gtk_button_new_with_label (label);
	gtk_widget_show (button);
	gtk_table_attach_defaults (GTK_TABLE (table), button, x, x+1, y, y+1);
    	g_signal_connect (G_OBJECT (button), "clicked",
	              G_CALLBACK (callback), (gpointer) GTK_BUTTON(button)->label_text);
	g_free(label);
      }
    }

//    gtk_mui_list(GTK_MUI(table));

    gtk_main ();

    return 0;
}
