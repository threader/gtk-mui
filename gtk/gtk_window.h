#ifndef GTK_WINDOW_H
#define GTK_WINDOW_H

#include <gtk/gtkwindow.h>

GtkWidget *mgtk_window_new(struct MUI_CustomClass *mcc, int gtype, GtkWindowType type);

#endif /* GTK_WINDOW_H */
