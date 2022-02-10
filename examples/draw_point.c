#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include <gtk/gtkdrawingarea.h>

int main(int argc, char *argv[]) 
{
    GtkWidget *window;
    GtkWidget *area;
    GtkWidget *vbox;
    GdkGC  *gc,*gc1;
    GdkColor color;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Drawing Point");
	gtk_widget_set_size_request(window, 256, 192);

	g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

	g_signal_connect_swapped (G_OBJECT (window), "delete_event",
                              G_CALLBACK (gtk_widget_destroy), 
                              G_OBJECT (window));
                            
    area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (area, 256, 192);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show (vbox);

	gtk_box_pack_start (GTK_BOX (vbox), area, TRUE, TRUE, 0);
	gtk_widget_show (area);

	gtk_widget_show (window);


    int x,y;
    float fa;

    gc = gdk_gc_new(area->window);

	gdk_color_parse("#000000", &color);
    gdk_gc_set_rgb_fg_color(gc, &color);

	gdk_draw_rectangle(area->window,
      gc,
      TRUE,
       0, 0, 256, 192);

	gdk_color_parse("#FFFF00", &color);
    gdk_gc_set_rgb_fg_color(gc, &color);

    for (x = 1; x < 255; x++) {
    fa = x*M_PI/125;
    y = 96+80*sin(fa);

    gdk_draw_point(area->window, 
                   gc,
                   x, y); }

	gdk_color_parse("#FF0000", &color);
    gdk_gc_set_rgb_fg_color(gc, &color);
    
    for (x = 1; x < 255; x++) {
    fa = x*M_PI/80;
    y = 96+80*cos(fa);

    gdk_draw_point(area->window, 
                   gc,
                   x, y); }

    gtk_main();

    g_object_unref(gc);

    return 0;
}
