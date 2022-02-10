
/*
 * Modified by the GTK-MUI Team 2006
 *
 * $Id: addharddrives.c,v 1.2 2011/07/04 12:16:21 o1i Exp $
 */

/*
 * this is a test, from uae:
 * you click on a button, and a new window appears.
 * this window is build dynamically (show after each new element),
 * not as you are supposed to do.
 */

#include <gtk/gtk.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkbbox.h>
#include <gtk/gtkspinbutton.h>
#include <../debug.h>

static void create_dirdlg (const char *title) {

    GtkWidget *dialog_vbox, *dialog_hbox, *vbox, *frame, *table, *hbox, *thing, *label, *button;
    GtkWidget *dirdlg,*path_entry, *devname_entry, *volname_entry, *bootpri_widget, *readonly_widget, *dirdlg_ok;

    DebOut("create_dirdlg()\n");

    dirdlg = gtk_dialog_new ();

    gtk_window_set_title (GTK_WINDOW (dirdlg), title);
    gtk_window_set_position (GTK_WINDOW (dirdlg), GTK_WIN_POS_MOUSE);
    gtk_window_set_modal (GTK_WINDOW (dirdlg), TRUE);
    gtk_widget_show (dirdlg);

    dialog_vbox = GTK_DIALOG (dirdlg)->vbox;
    gtk_widget_show (dialog_vbox);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);
    gtk_box_pack_start (GTK_BOX (dialog_vbox), vbox, TRUE, FALSE, 0);

    frame = gtk_frame_new ("Mount host folder");
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 8);

    hbox = gtk_hbox_new (FALSE, 4);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);

    label  = gtk_label_new ("Path");
//    gtk_label_set_pattern (GTK_LABEL (label), "_");
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

#ifndef GTKMUI
    thing = gtk_entry_new_with_max_length (255);
#else
    thing = gtk_entry_new();
#endif
#if 0
    gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
#endif
    gtk_mui_list(GTK_MUI(dialog_vbox));

    /* this dies: */
    gtk_box_pack_start (GTK_BOX (hbox), thing, TRUE, TRUE, 0);
    gtk_widget_show (thing);
    path_entry = thing;

    button = gtk_button_new_with_label ("Select...");
#if 0
    gtk_signal_connect (GTK_OBJECT (button), "clicked", (GtkSignalFunc) did_dirdlg_select, (gpointer) path_entry);
#endif
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
    gtk_widget_show (button);

    frame = gtk_frame_new ("As Amiga disk");
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 8);

    ErrOut("table:");
    table = gtk_table_new (3, 4, FALSE);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table), 4);
    gtk_table_set_col_spacings (GTK_TABLE (table), 4);

        label = gtk_label_new ("Device name");
        gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                        (GtkAttachOptions) (GTK_FILL),
                        (GtkAttachOptions) (0), 0, 0);
        gtk_widget_show (label);
#ifndef GTKMUI
        thing = gtk_entry_new_with_max_length (255);
#else
        thing = gtk_entry_new();
#endif
#if 0
	gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
#endif
        gtk_widget_show (thing);
        gtk_table_attach (GTK_TABLE (table), thing, 1, 2, 0, 1,
                        (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                        (GtkAttachOptions) (0), 0, 0);
        gtk_widget_set_usize (thing, 200, -1);
        devname_entry = thing;

        label = gtk_label_new ("Volume name");
        gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
                        (GtkAttachOptions) (GTK_FILL),
                        (GtkAttachOptions) (0), 0, 0);

        gtk_widget_show (label);
#ifndef GTKMUI
        thing = gtk_entry_new_with_max_length (255);
#else
        thing = gtk_entry_new();
#endif
#if 0
	gtk_signal_connect (GTK_OBJECT (thing), "changed", (GtkSignalFunc) dirdlg_on_change, (gpointer) NULL);
	ErrOut("alive aa!");
#endif
        gtk_table_attach (GTK_TABLE (table), thing, 1, 2, 1, 2,
                        (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                        (GtkAttachOptions) (0), 0, 0);

        gtk_widget_show (thing);
        gtk_widget_set_usize (thing, 200, -1);
	ErrOut("alive c!");
        volname_entry = thing;

	ErrOut("alive2!");
	/* here we die */
        label = gtk_label_new ("Boot priority");
        gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 2,
                        (GtkAttachOptions) (GTK_FILL),
                        (GtkAttachOptions) (0), 0, 0);
        gtk_widget_show (label);
        thing = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, -128, 127, 1, 5, 5)), 1, 0);
        gtk_table_attach (GTK_TABLE (table), thing, 3, 4, 0, 2,
                        (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                        (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
        gtk_widget_show (thing);
        bootpri_widget = thing;

        readonly_widget = gtk_check_button_new_with_label ("Read only");
        gtk_table_attach (GTK_TABLE (table), readonly_widget, 0, 4, 2, 3,
                        (GtkAttachOptions) (GTK_EXPAND),
                        (GtkAttachOptions) (0), 0, 0);
        gtk_widget_show (readonly_widget);

    dialog_hbox = GTK_DIALOG (dirdlg)->action_area;

	ErrOut("alive3!");
#ifndef GTKMUI
    hbox = gtk_hbutton_box_new();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_END);
#else
    hbox = gtk_hbox_new(FALSE,4);
#endif
    gtk_box_pack_start (GTK_BOX (dialog_hbox), hbox, TRUE, TRUE, 0);
    gtk_widget_show (hbox);

    button = gtk_button_new_with_label ("OK");
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
#if 0
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC(newdir_ok), NULL);
#endif
//    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
//    gtk_widget_grab_default (button);
    gtk_widget_show (button);
    dirdlg_ok = button;

    button = gtk_button_new_with_label ("Cancel");
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
			       GTK_SIGNAL_FUNC (gtk_widget_destroy),
			       GTK_OBJECT (dirdlg));
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);
    ErrOut("alive4!");

}

void callback(GtkWidget *widget, gpointer data) {
  g_print ("Button %s was pressed\n", (char *) data);

  create_dirdlg("Test window");

  g_print ("Test window closed\n");
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

  gtk_window_set_title (GTK_WINDOW (window), "Add Button");

  /* It's a good idea to do this for all windows. */
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Create a box to hold the arrows/buttons */
  box = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 2);
  gtk_container_add (GTK_CONTAINER (window), box);

  gtk_widget_show (box);

  button = gtk_button_new_with_label ("open window");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 3);

  g_signal_connect (G_OBJECT (button), "clicked",
		    G_CALLBACK (callback), (gpointer) "add window");

  gtk_widget_show (window);
  
  /* Rest in gtk_main and wait for the fun to begin! */
  gtk_main ();
  
  return 0;
}
