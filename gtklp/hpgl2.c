/*
 GtkLP
  graphical Frontends for non-administrative CUPS users.
 
 Copyright (C) 1998-2004 Tobias Mueller

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software 
 Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 Boston, MA 02110-1301, USA

*/

#include <config.h>

#include "hpgl2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>

#include <gettext.h>
#include <defaults.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"

GtkWidget *hpgl2Tab;
GtkWidget *hpgl2OptFrame;
GtkWidget *hpgl2BlackCheck, *hpgl2FitCheck;
GtkWidget *hpgl2PenFrame, *hpgl2PenField;

void hpgl2TabSetDefaults(void)
{
	char tmp[MAX_HPGL_PEN_DIGIT + 1];

	if (DEBUG)
		printf("Get hpgl2Defaults\n");
	/* Black */
	if (hpgl2black) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2BlackCheck),
					     1);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2BlackCheck),
					     0);
	}
	/* Fit */
	if (hpgl2fit) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2FitCheck),
					     1);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2FitCheck),
					     0);
	}
	/* Pen */
	snprintf(tmp, (size_t) MAX_HPGL_PEN_DIGIT, "%u", hpgl2pen);
	gtk_entry_set_text(GTK_ENTRY(hpgl2PenField), tmp);

}

void hpgl2CheckChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("hpgl2Changed:%s\n", (char *)data);

	if (strcmp(data, "Black") == 0) {
		if (gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(hpgl2BlackCheck))) {
			hpgl2black = 0;
		} else {
			hpgl2black = 1;
		}
		return;
	}

	if (strcmp(data, "Fit") == 0) {
		if (gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(hpgl2FitCheck))) {
			hpgl2fit = 0;
		} else {
			hpgl2fit = 1;
		}
		return;
	}

	if (DEBUG)
		printf("PROG-ERROR: Unknown HPGL2-Button !\n");
	emergency();

}

void Hpgl2OptFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	hpgl2OptFrame = gtk_frame_new(str2str(_("Options")));
	gtk_box_pack_start(GTK_BOX(mainHBox), hpgl2OptFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(hpgl2OptFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	/* Buttons */
	hpgl2BlackCheck =
	    gtk_check_button_new_with_label(str2str(_("Printing in Black")));
	gtk_box_pack_start(GTK_BOX(vbox), hpgl2BlackCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(hpgl2BlackCheck), "pressed",
			   GTK_SIGNAL_FUNC(hpgl2CheckChanged), "Black");
	gtk_widget_show(hpgl2BlackCheck);

	hpgl2FitCheck =
	    gtk_check_button_new_with_label(str2str(_("Fit on Page")));
	gtk_box_pack_start(GTK_BOX(vbox), hpgl2FitCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(hpgl2FitCheck), "pressed",
			   GTK_SIGNAL_FUNC(hpgl2CheckChanged), "Fit");
	gtk_widget_show(hpgl2FitCheck);

}

void hpgl2PenChanged(GtkWidget * widget, gpointer data)
{
	char tmp[MAX_HPGL_PEN_DIGIT + 1];
	unsigned i1, flg = 0;

	strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(hpgl2PenField), 0, -1),
		(size_t) MAX_HPGL_PEN_DIGIT);
	if (DEBUG)
		printf("HPGL2-Pen: %s\n", tmp);

	for (i1 = 0; i1 < strlen(tmp); i1++) {
		if (isdigit(tmp[i1]) == 0) {
			snprintf(tmp, (size_t) MAX_HPGL_PEN_DIGIT, "%u",
				 hpgl2pen);
			gtk_entry_set_text(GTK_ENTRY(hpgl2PenField), tmp);
			gtk_editable_set_position(GTK_EDITABLE(hpgl2PenField),
						  -1);
			flg = 1;
			break;
		}
	}

	if ((flg == 0) && tmp[0] != (uintptr_t) NULL) {
		hpgl2pen = atoi(tmp);
		if (DEBUG)
			printf("HPGL2-Pen: Value changed: %u\n", hpgl2pen);
	}

}

void Hpgl2PenFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *label, *button1;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	hpgl2PenFrame = gtk_frame_new(str2str(_("Pen Width")));
	gtk_box_pack_start(GTK_BOX(mainHBox), hpgl2PenFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(hpgl2PenFrame), hbox);
	gtk_widget_show(hbox);

	/* TextBox */
	hpgl2PenField = gtk_entry_new_with_max_length(MAX_HPGL_PEN_DIGIT);
	gtk_widget_set_size_request(hpgl2PenField, MAX_HPGL_PEN_FIELD_LENGTH,
				    -1);
	gtk_entry_set_text(GTK_ENTRY(hpgl2PenField), "");
	gtk_box_pack_start(GTK_BOX(hbox), hpgl2PenField, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(hpgl2PenField);

	/* label */
	label = gtk_label_new(str2str(_("micrometers")));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	/* Buttons */
	button1 = gtk_button_new_with_label(str2str(_("Default")));
	button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox), button1, FALSE, FALSE, 0);
	gtk_widget_show(button1);

	/* Signals */
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(hpgl2PenField)), "changed",
			   GTK_SIGNAL_FUNC(hpgl2PenChanged), NULL);
	gtk_signal_connect(GTK_OBJECT(button1), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "HPGL2PenDefault");

}

void Hpgl2Tab(void)
{
	hpgl2Tab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hpgl2Tab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), hpgl2Tab,
				 gtk_label_new(str2str(_("HP-GL/2"))));

	Hpgl2OptFrame(hpgl2Tab);
	Hpgl2PenFrame(hpgl2Tab);
}
