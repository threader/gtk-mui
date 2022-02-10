/*
 *File name: hbox.c
 *
 * taken from: 
 * http://www.linuxheadquarters.com/howto/programming/gtk_examples/combo/combo.shtml
 */

/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: combo.c,v 1.3 2011/07/04 12:16:21 o1i Exp $
 */


#include <gtk/gtk.h>
#include <glib.h>

#include "../debug.h"

/*-- This function allows the program to exit properly when the window is closed --*/
gint destroyapp (GtkWidget *widget, gpointer gdata) {

  g_print ("Quitting...\n");
  gtk_main_quit();
  return (FALSE);
}

static void on_choice_changed (GtkWidget *w, GtkWidget *chooser) {
  GtkList *list   = GTK_LIST (GTK_COMBO (chooser)->list);
  GList   *choice = list->selection;

  DebOut("on_choice_changed!\n");
  if (choice) {
    DebOut("child position: %d\n",gtk_list_child_position (list, choice->data));
    g_print("selected nr: %d\n",gtk_list_child_position (list, choice->data));
  }
  else {
    DebOut("no child position !?\n");
    g_print("ERROR!\n");
  }
}

int main (int argc, char *argv[]) {

  /*-- Declare the GTK Widgets used in the program --*/
  GtkWidget *window;
  GtkWidget *combo;
  GList *items = NULL;

  /*--Initialize GTK--*/
  gtk_init (&argc, &argv);

  /*--Create the new window--*/
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /*--Connect the window to the destroyapp function --*/
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroyapp), NULL);

  /*-- Items to be added to combo widget--*/
  items = g_list_append (items, "Banana");
  items = g_list_append (items, "Orange");
  items = g_list_append (items, "Peach");
  items = g_list_append (items, "Strawberry");

  /*--Create a new combo widget --*/
  combo = gtk_combo_new();
  /*-- Set items in the popup list--*/
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);
  g_list_free(items);

  gtk_signal_connect (GTK_OBJECT (GTK_COMBO(combo)->popwin), "hide", GTK_SIGNAL_FUNC (on_choice_changed), combo);

  /*--- Add combo widget to window-*/
  gtk_container_add(GTK_CONTAINER (window), combo);

  /*-- Add a border to the window to give the button a little room --*/
  gtk_container_border_width (GTK_CONTAINER (window), 15);
         
  /*-- Display the widgets --*/
  gtk_widget_show(combo);
  gtk_widget_show(window);

  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 2);
  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 3);
  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 0);
	printf("sleep ..\n");
	sleep(2);
	printf("select 1 (Orange)..\n");
  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 1);
	sleep(2);
	printf("select 2 (Peach)..\n");
  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 2);
	sleep(2);
	printf("select 3 (Strawberry)..\n");
  gtk_list_select_item (GTK_LIST (GTK_COMBO (combo)->list), 3);




  /*-- Start the GTK event loop--*/
  gtk_main();

  /*--Return 0 if exit is successful--*/
  return 0;

}
