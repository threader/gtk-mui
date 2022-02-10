#ifndef GTK_DIALOG_H
#define GTK_DIALOG_H

#include <gtk/gtkwindow.h>
#include <gtk/gtkdialog.h>

GtkWidget *vmgtk_dialog_new_with_buttons(struct MUI_CustomClass *mcc, int gtype, const gchar *title, GtkWindow *parent, GtkDialogFlags flags, const gchar *first_button_text, va_list args);
GtkWidget *mgtk_dialog_new_with_buttons(struct MUI_CustomClass *mcc, int gtype, const gchar *title, GtkWindow *parent, GtkDialogFlags flags, const gchar *first_button_text, ...);

#endif /* GTK_DIALOG_H */
