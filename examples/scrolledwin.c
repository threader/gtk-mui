/**************************************************************
 * scrolledwin
 *
 * small test case for a GTK_SCROLLED_WINDOW
 *
 * - create a scrolleable window with 5 buttons
 *
 * $Id: scrolledwin.c,v 1.1 2007/03/19 09:00:13 o1i Exp $
 *
 * reduced version of the clist example, found somwhere on
 * the web.
 **************************************************************/

#include <gtk/gtk.h>
#include <gtk/gtkscrolledwindow.h>

int main(int argc, gchar *argv[] ) {                                  

    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button1, *button2, *button3, *button4, *button5;
    GtkWidget *scrolled_window;

    gtk_init(&argc, &argv);
    
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, 150, 150);

    gtk_window_set_title(GTK_WINDOW(window), "GtkScrolledWindow Example");
    gtk_signal_connect(GTK_OBJECT(window),
                       "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit),
                       NULL);
    
    vbox=gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);
    
    /* Create a scrolled window to pack the widgets into */

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    /* Create big buttons and add them to the window */

    button1 = gtk_button_new_with_label("========= Testbutton number 1 =========");
    button2 = gtk_button_new_with_label("========= Testbutton number 2 =========");
    button3 = gtk_button_new_with_label("========= Testbutton number 3 =========");
    button4 = gtk_button_new_with_label("========= Testbutton number 4 =========");
    button5 = gtk_button_new_with_label("========= Testbutton number 5 =========");

    gtk_container_add(GTK_CONTAINER(scrolled_window), button1);
    gtk_container_add(GTK_CONTAINER(scrolled_window), button2);
    gtk_container_add(GTK_CONTAINER(scrolled_window), button3);
    gtk_container_add(GTK_CONTAINER(scrolled_window), button4);
    gtk_container_add(GTK_CONTAINER(scrolled_window), button5);

    gtk_widget_show(button1);
    gtk_widget_show(button2);
    gtk_widget_show(button3);
    gtk_widget_show(button4);
    gtk_widget_show(button5);

    gtk_widget_show(window);
    gtk_main();
    
    return(0);
}
