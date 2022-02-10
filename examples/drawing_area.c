#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "debug.h"

#if 0
gboolean expose_event_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data) {

  DebOut("entered----------------\n");

  gdk_draw_rectangle(widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
      TRUE,
       0, 0, 50, 50);

  DebOut("left----------------\n");
     
  return TRUE;
}
#endif

int main(int argc, char *argv[]) {

  GtkWidget *drawing_area;
  GtkWidget *window;
  GtkWidget *vbox;

  DebOut("started..\n");
  gtk_init (&argc, &argv);
  /* create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (GTK_WIDGET (window), 200, 100);
  gtk_window_set_title (GTK_WINDOW (window), "GTK Drawing Area");
#if 0
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect_swapped (G_OBJECT (window), "delete_event",
                            G_CALLBACK (gtk_widget_destroy), 
                            G_OBJECT (window));
#endif

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 100, 100);
  DebOut("drawing_area: %lx\n", drawing_area);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",  
                    G_CALLBACK (expose_event_callback), NULL);
  DebOut("bla2..\n");

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);
  gtk_widget_show (drawing_area);

  gtk_widget_show (window);

  gdk_draw_rectangle(drawing_area->window,
      drawing_area->style->fg_gc[GTK_WIDGET_STATE (drawing_area)],
      TRUE,
       0, 0, 50, 50);

  gtk_main();
}
