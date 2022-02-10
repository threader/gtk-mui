
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: table.c,v 1.3 2011/07/04 12:16:21 o1i Exp $
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
    GtkWidget *table2;
    GtkWidget *bigtable;
    GtkWidget *dummygroup;
    GtkWidget *notebook;
    GtkWidget *frame;
    GtkWidget *label;

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

    notebook=gtk_notebook_new ();

    /* Create a 2x2 table */
    table = gtk_table_new (2, 2, TRUE);
    bigtable = gtk_table_new (2, 2, FALSE);

    /* Put the table in the main window */
    gtk_container_add (GTK_CONTAINER (window), notebook);

    /* Create first button */
    button = gtk_button_new_with_label (">>>>>>>>>> button 1 <<<<<<<<<<<<");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 1" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
	              G_CALLBACK (callback), (gpointer) "button 1");


    /* Insert button 1 into the upper left quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 1, 0, 1);

    gtk_widget_show (button);

    /* Create second button */

    button = gtk_button_new_with_label ("button 2");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 2" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (callback), (gpointer) "button 2");
    /* Insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), button, 1, 2, 0, 1);

    gtk_widget_show (button);

    /* Create "Quit" button */
    button = gtk_button_new_with_label ("Quit");

    /* When the button is clicked, we call the "delete_event" function
     * and the program exits */
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (delete_event), NULL);

    dummygroup=gtk_vbox_new(FALSE,0);
    gtk_widget_show (dummygroup);

    /* Insert the quit button into the both 
     * lower quadrants of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), dummygroup, 0, 2, 1, 2);
    gtk_box_pack_start (GTK_BOX (dummygroup), button,TRUE,TRUE,0);

    gtk_widget_show (button);


    /*************************** FALSE ****************************/

    /* Create a 2x2 table */
    table2 = gtk_table_new (2, 2, FALSE);

    /* Create first button */
    button = gtk_button_new_with_label (">>>>>>>>>> button 1 <<<<<<<<<<<<");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 1" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
	              G_CALLBACK (callback), (gpointer) "button 1");


    /* Insert button 1 into the upper left quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table2), button, 0, 1, 0, 1);

    gtk_widget_show (button);

    /* Create second button */

    button = gtk_button_new_with_label ("button 2");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 2" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (callback), (gpointer) "button 2");
    /* Insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table2), button, 1, 2, 0, 1);

    gtk_widget_show (button);

    /* Create "Quit" button */
    button = gtk_check_button_new_with_label ("Quit2");

    /* When the button is clicked, we call the "delete_event" function
     * and the program exits */
    g_signal_connect (G_OBJECT (button), "clicked",
                      G_CALLBACK (delete_event), NULL);

    /* Insert the quit button into the both 
     * lower quadrants of the table */
    gtk_table_attach_defaults (GTK_TABLE (table2), button, 0, 2, 1, 2);

    gtk_widget_show (button);

    gtk_table_attach_defaults (GTK_TABLE (bigtable), table, 0, 1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (bigtable), table2, 0, 1, 1, 2);

    frame=gtk_frame_new("dummy frame");
    gtk_widget_show (frame);

    gtk_container_add (GTK_CONTAINER (frame), bigtable);

    label = gtk_label_new ("dummy notebook page");
    gtk_widget_show (label);
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

    gtk_widget_show (table);
    gtk_widget_show (table2);
    gtk_widget_show (bigtable);
    gtk_widget_show (window);

    gtk_mui_list(GTK_MUI(notebook));

    gtk_main ();

    return 0;
}
