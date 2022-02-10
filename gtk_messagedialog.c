/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Ilkka Lehtoranta
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: gtk_messagedialog.c,v 1.3 2007/06/19 13:33:17 o1i Exp $
 *
 *****************************************************************************/

#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "classes/classes.h"
#include "gtk_dialog.h"
#include "gtk_globals.h"
#include "gtk-mui.h"

#include <gtk/gtk.h>
#include <gtk/gtkhbbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkmessagedialog.h>

#include "debug.h"

/**********************************************************************
	mgtk_message_dialog_new
**********************************************************************/

STATIC GtkWidget *mgtk_message_dialog_new(struct MUI_CustomClass *mcc, int gtype, GtkWindow *parent, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, va_list args)
{
	GtkMessageDialog *dialog = NULL;
	const gchar *title;

	switch (type)
	{
		default:
		case GTK_MESSAGE_INFO    : title = "Information"; break;
		case GTK_MESSAGE_WARNING : title = "Warning"; break;
		case GTK_MESSAGE_QUESTION: title = "Question"; break;
		case GTK_MESSAGE_ERROR   : title = "Error"; break;
	}

	switch (buttons)
	{
		case GTK_BUTTONS_NONE:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, NULL);
			break;

		case GTK_BUTTONS_OK:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
			break;

		case GTK_BUTTONS_CLOSE:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
			break;

		case GTK_BUTTONS_CANCEL:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			break;

		case GTK_BUTTONS_YES_NO:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, NULL);
			break;

		case GTK_BUTTONS_OK_CANCEL:
			dialog = (GtkMessageDialog *)mgtk_dialog_new_with_buttons(mcc, gtype, title, parent, flags, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			break;
	}

	if (dialog)
	{
		char *text;

		text = g_strdup_vprintf(message_format, args);

		dialog->label = gtk_label_new(text);

		if (dialog->label)
		{
			// copypasted from the original GTK source code

			GtkWidget *hbox, *vbox;

			hbox = gtk_hbox_new(FALSE, 12);
			vbox = gtk_vbox_new(FALSE, 12);

			gtk_box_pack_start(vbox, dialog->label, FALSE, FALSE, 0);
			#if 0
			gtk_box_pack_start(vbox, priv->secondary_label, TRUE, TRUE, 0);
			gtk_box_pack_start(hbox, dialog->image, FALSE, FALSE, 0);
			#endif
			gtk_box_pack_start(hbox, vbox, TRUE, TRUE, 0);
			gtk_box_pack_start(dialog->parent_instance.vbox, hbox, FALSE, FALSE, 0);

			gtk_widget_show_all(hbox);
		}

		if (text)
		{
			g_free(text);
		}
	}

	return (GtkWidget *)dialog;
}

/**********************************************************************
	gtk_message_dialog_new
**********************************************************************/

GtkWidget *gtk_message_dialog_new(GtkWindow *parent, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ...)
{
	GtkWidget *widget;
	va_list args;
	va_start(args, message_format);
	widget = mgtk_message_dialog_new(CL_MessageDialog, IS_MESSAGEDIALOG, parent, flags, type, buttons, message_format, args);
	va_end(args);
	return widget;
}

/**********************************************************************
	gtk_message_dialog_new_with_markup

	Markup (Pango) is not really supported
**********************************************************************/

GtkWidget *gtk_message_dialog_new_with_markup(GtkWindow *parent, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ...)
{
	GtkWidget *widget;
	va_list args;
	va_start(args, message_format);
	widget = mgtk_message_dialog_new(CL_MessageDialog, IS_MESSAGEDIALOG, parent, flags, type, buttons, message_format, args);
	va_end(args);
	return widget;
}

/**********************************************************************
	gtk_message_dialog_set_markup

	Markup (Pango) is not really supported
**********************************************************************/

void gtk_message_dialog_set_markup(GtkMessageDialog *message_dialog, const gchar *str)
{
  gtk_label_set_markup(message_dialog->label, str);
}
