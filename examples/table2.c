
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: table2.c,v 1.3 2011/07/04 12:16:21 o1i Exp $
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
    GtkWidget *frame;
    GtkWidget *label;
    GtkWidget *chooser;
    GtkWidget *check;
    GList     *list = 0;

    gtk_init (&argc, &argv);


    /* Create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* Set the window title */
    gtk_window_set_title (GTK_WINDOW (window), "Table");

    /* Set a handler for delete_event that immediately
     * exits GTK. */
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (delete_event), NULL);

    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);

    /* Create a 3x3 table */
    table = gtk_table_new (4, 4, FALSE);

    frame=gtk_frame_new("table frame");

    /* Put the frame in the main window */
    gtk_container_add (GTK_CONTAINER (window), frame);

    /* and the table in the frame */
    gtk_container_add (GTK_CONTAINER (frame), table);

/*
    gtk_frame_set_label(GTK_FRAME(frame),"still working?");
    */

    label=gtk_label_new("Speed");
    /* Insert button 1 into the upper left quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
    gtk_widget_show (label);

    chooser=gtk_combo_new();
    gtk_combo_set_use_arrows( GTK_COMBO(chooser), FALSE);
    gtk_combo_set_value_in_list ( GTK_COMBO(chooser), TRUE, FALSE);
    list = g_list_append (list,"68000");
    list = g_list_append (list,"68010");
    list = g_list_append (list,"68020");
    list = g_list_append (list,"68020 XXXXXXXXXXXXXXXXXXX");

    gtk_combo_set_popdown_strings ( GTK_COMBO(chooser), list);
    g_list_free (list);

    //button = gtk_button_new_with_label ("2XXXXXXXXXXXXXXXXXXXXXXXXXX");
    gtk_table_attach_defaults (GTK_TABLE (table), chooser, 1, 3, 0, 1);
    gtk_widget_show (chooser);
    //gtk_widget_show (button);

/*
    button = gtk_button_new_with_label ("3");
    gtk_table_attach_defaults (GTK_TABLE (table), button, 2, 3, 0, 1);
    gtk_widget_show (button);
    */


    button = gtk_button_new_with_label ("4");
    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 1, 1, 2);
    gtk_widget_show (button);

    check=gtk_check_button_new_with_label ("Idle on HALT instruction");
    gtk_table_attach_defaults (GTK_TABLE (table), check, 1, 2, 1, 2);
    gtk_widget_show (check);

    button = gtk_button_new_with_label ("!");
    gtk_table_attach_defaults (GTK_TABLE (table), button, 2, 3, 1, 2);
    gtk_widget_show (button);

//    button = gtk_button_new_with_label ("Q!");
//    gtk_table_attach_defaults (GTK_TABLE (table), button, 3, 4, 3, 4);
//    gtk_widget_show (button);

    /* Create "Quit" button */
    button = gtk_button_new_with_label ("Quit");

    /* When the button is clicked, we call the "delete_event" function
     * and the program exits */
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (delete_event), NULL);

    /* Insert the quit button into the both 
     * lower quadrants of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 4, 2, 3);

    check=gtk_check_button_new_with_label ("Idle on HALT 2");
    gtk_table_attach_defaults (GTK_TABLE (table), check, 1, 2, 3, 4);
    gtk_widget_show (check);

    gtk_widget_show (button);

    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
