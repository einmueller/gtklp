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

#include "general.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <cups/ppd.h>

#include <gettext.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"

GtkWidget *generalTab;
GtkWidget *printerFrame;
GtkWidget *printerFrameCombo, *printerFrameInfoText, *printerFrameLocationText;
GtkWidget *printerNumCopiesFrame, *printerNumField;
GtkWidget *collateCheck;
GtkWidget *printerMediaFrame, *printerMediaSizeCombo, *printerMediaTypeCombo,
    *printerMediaSourceCombo;
GtkWidget *MediaTypeFrame, *MediaSizeFrame, *MediaSourceFrame;
GtkWidget *printerDuplexFrame;
GtkWidget *gtklpqFrame;
GtkWidget *CustomMediaSizeFrame;
GtkWidget *customSizeFieldX, *customSizeFieldY, *customSizeCombo;
GSList *DuplexButtons;

void generalTabSetDefaults(void)
{
	GList *mediaSize = NULL, *mediaType = NULL, *mediaSource =
	    NULL, *customSizeFieldUnit = NULL;
	int i1, flg;
	ppd_option_t *option;
	ppd_choice_t *choice;
	char buf[128];

	if (DEBUG)
		printf("Get generalTabDefaults\n");

	/* PrinterName */
	gtk_list_select_item(GTK_LIST(GTK_COMBO(printerFrameCombo)->list),
			     PrinterChoice);
	/* PrinterInfo */
	gtk_label_set_text(GTK_LABEL(printerFrameInfoText),
			   str2str(PrinterInfos[PrinterChoice]));
	/* PrinterLocation */
	snprintf(buf, 128, "%s: %s", str2str(_("Location")),
		 str2str(PrinterLocations[PrinterChoice]));
	gtk_label_set_text(GTK_LABEL(printerFrameLocationText), buf);
	/* Number of Copies */
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(printerNumField),
				  (gfloat) NumberOfCopies);
	/* Collate Copies */
	if (collate) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(collateCheck),
					     1);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(collateCheck),
					     0);
	}

	/* Media Types */
	if ((ppdFindOption(printerPPD, "PageSize") == NULL) &&
	    (ppdFindOption(printerPPD, "MediaType") == NULL) &&
	    (ppdFindOption(printerPPD, "InputSlot") == NULL) &&
	    (ppdFindOption(printerPPD, "PageRegion") == NULL))
		gtk_widget_hide(printerMediaFrame);
	else {
		gtk_widget_show(printerMediaFrame);

		/* Look if PageSize or PageRegion is marked! */
		flg = 0;
		if ((option = ppdFindOption(printerPPD, "PageSize")) != NULL) {
			for (i1 = option->num_choices, choice = option->choices;
			     i1 > 0; i1--, choice++) {
				if (choice->marked)
					flg = 1;
			}
		}

		if (flg == 0) {	/* PageSize should win over PageRegion (PPD-Specs) */
			if ((option =
			     ppdFindOption(printerPPD, "PageRegion")) != NULL) {
				for (i1 = option->num_choices, choice =
				     option->choices; i1 > 0; i1--, choice++) {
					if (choice->marked)
						flg = 2;
				}
			}
		}

		if (flg != 2) {	/* PageSize or nothing is marked, use PageSize */
			option = ppdFindOption(printerPPD, "PageSize");
			for (i1 = option->num_choices, choice = option->choices;
			     i1 > 0; i1--, choice++) {
				mediaSize =
				    g_list_append(mediaSize,
						  str2str(choice->text));
				if (choice->marked)
					MediaSizeSelected =
					    option->num_choices - i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO
						      (printerMediaSizeCombo),
						      mediaSize);
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(printerMediaSizeCombo)->
					      list), MediaSizeSelected);
			gtk_entry_set_position(GTK_ENTRY
					       (GTK_COMBO
						(printerMediaSizeCombo)->entry),
					       0);
			gtk_widget_show(MediaSizeFrame);
		} else {
			if (flg == 2) {	/* PageRegion marked, PageSize not */
				option =
				    ppdFindOption(printerPPD, "PageRegion");
				for (i1 = option->num_choices, choice =
				     option->choices; i1 > 0; i1--, choice++) {
					mediaSize =
					    g_list_append(mediaSize,
							  str2str(choice->
								  text));
					if (choice->marked)
						MediaSizeSelected =
						    option->num_choices - i1;
				}
				gtk_combo_set_popdown_strings(GTK_COMBO
							      (printerMediaSizeCombo),
							      mediaSize);
				gtk_list_select_item(GTK_LIST
						     (GTK_COMBO
						      (printerMediaSizeCombo)->
						      list), MediaSizeSelected);
				gtk_entry_set_position(GTK_ENTRY
						       (GTK_COMBO
							(printerMediaSizeCombo)->
							entry), 0);
				gtk_widget_show(MediaSizeFrame);
			} else {	/* We dont have PageSize and we dont have PageRegion, broken PPD! */
				gtk_widget_hide(MediaSizeFrame);
			}
		}
		if ((option = ppdFindOption(printerPPD, "MediaType")) != NULL) {
			for (i1 = option->num_choices, choice = option->choices;
			     i1 > 0; i1--, choice++) {
				mediaType =
				    g_list_append(mediaType,
						  str2str(choice->text));
				if (choice->marked)
					MediaTypeSelected =
					    option->num_choices - i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO
						      (printerMediaTypeCombo),
						      mediaType);
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(printerMediaTypeCombo)->
					      list), MediaTypeSelected);
			gtk_entry_set_position(GTK_ENTRY
					       (GTK_COMBO
						(printerMediaTypeCombo)->entry),
					       0);
			gtk_widget_show(MediaTypeFrame);
		} else {
			gtk_widget_hide(MediaTypeFrame);
		}
		if ((option = ppdFindOption(printerPPD, "InputSlot")) != NULL) {
			for (i1 = option->num_choices, choice = option->choices;
			     i1 > 0; i1--, choice++) {
				mediaSource =
				    g_list_append(mediaSource,
						  str2str(choice->text));
				if (choice->marked)
					MediaSourceSelected =
					    option->num_choices - i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO
						      (printerMediaSourceCombo),
						      mediaSource);
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO
					      (printerMediaSourceCombo)->list),
					     MediaSourceSelected);
			gtk_entry_set_position(GTK_ENTRY
					       (GTK_COMBO
						(printerMediaSourceCombo)->
						entry), 0);
			gtk_widget_show(MediaSourceFrame);
		} else {
			gtk_widget_hide(MediaSourceFrame);
		}
	}

	/* Custom Media Type */
	customSizeFieldUnit = g_list_append(customSizeFieldUnit, str2str("pt"));
	customSizeFieldUnit = g_list_append(customSizeFieldUnit, str2str("in"));
	customSizeFieldUnit = g_list_append(customSizeFieldUnit, str2str("cm"));
	customSizeFieldUnit = g_list_append(customSizeFieldUnit, str2str("mm"));
	gtk_combo_set_popdown_strings(GTK_COMBO(customSizeCombo),
				      customSizeFieldUnit);

	/* Duplex */
	if ((option = ppdFindOption(printerPPD, "Duplex")) == NULL) {
		gtk_widget_hide(printerDuplexFrame);
	} else {
		choice = ppdFindChoice(option, "None");
		if (choice != NULL) {
			if (choice->marked) {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
							     (g_slist_nth_data
							      (DuplexButtons,
							       2)), TRUE);
			} else {
				choice =
				    ppdFindChoice(option, "DuplexNoTumble");
				if (choice != NULL) {
					if (choice->marked) {
						gtk_toggle_button_set_active
						    (GTK_TOGGLE_BUTTON
						     (g_slist_nth_data
						      (DuplexButtons, 1)),
						     TRUE);
					} else {
						choice =
						    ppdFindChoice(option,
								  "DuplexTumble");
						if (choice != NULL) {
							if (choice->marked)
								gtk_toggle_button_set_active
								    (GTK_TOGGLE_BUTTON
								     (g_slist_nth_data
								      (DuplexButtons,
								       0)),
								     TRUE);
						}
					}
				}
			}
		}
		gtk_widget_show(printerDuplexFrame);
	}
}

void NumOfCopiesChanged(GtkWidget * widget, gpointer data)
{
#if GTK_MAJOR_VERSION != 1
	gtk_spin_button_update(GTK_SPIN_BUTTON(printerNumField));
#endif
	NumberOfCopies =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(printerNumField));
	if (DEBUG)
		printf("NumberOfCopiesEntry: Value changed: %u\n",
		       NumberOfCopies);
}

int customSizeFieldChanged(GtkWidget * widget, gpointer data)
{
	char *z1, *z2;
	char *text;
	int haserror = 0;
	int countdigit = 0;

	text = (char *)gtk_entry_get_text(GTK_ENTRY(widget));
	z1 = text;
	while (*z1 != (uintptr_t) NULL) {
		if (*z1 == '.') {
			countdigit++;
		}
		if ((!isdigit(*z1) && *z1 != '.') || countdigit > 1) {
			if (*z1 == '.') {
				countdigit--;
			}
			haserror = 1;
			z2 = z1;
			while (*z2 != (uintptr_t) NULL) {
				*z2 = *(z2 + 1);
				z2++;
			}
		}
		z1++;
	}
	z1 = text;
	if (*z1 == (uintptr_t) NULL) {
		*z1++ = '0';
		*z1 = (uintptr_t) NULL;
	}
	if (haserror == 1) {
		gtk_entry_set_text(GTK_ENTRY(widget), text);
		gtk_widget_grab_focus(GTK_WIDGET(widget));
	}

	return (FALSE);
}

void wantCollateCheck(GtkWidget * widget, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(collateCheck))) {
		collate = 0;
	} else {
		collate = 1;
	}
	if (DEBUG)
		printf("collate changed: %u\n", collate);
}

void PrinterChanged(GtkWidget * widget, gpointer data)
{
	int i1, strangeerrortrap, ret, merkconflict;
	char tmp[DEF_PRN_LEN + 1];

	snprintf(tmp, (size_t) DEF_PRN_LEN, "%s",
		 gtk_entry_get_text(GTK_ENTRY
				    (GTK_COMBO(printerFrameCombo)->entry)));

	if (DEBUG)
		printf("Printer changed: %s\n", (char *)tmp);

	if (tmp[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf("Skipping emptry selection !\n");
		return;
	}
	if (!strcmp(tmp, "-----")) {
		gtk_entry_set_text(GTK_ENTRY
				   (GTK_COMBO(printerFrameCombo)->entry),
				   PrinterNames[PrinterChoice]);
		return;
	}

	merkconflict = conflict_active;
	conflict_active = 0;
	i1 = 0;
	strangeerrortrap = 0;
	do {
		if (strcmp(str2str(PrinterNames[i1]), tmp) == 0) {
			lastPrinterChoice = PrinterChoice;
			PrinterChoice = i1;
			strangeerrortrap = 1;
		}
		i1++;
	}
	while (PrinterNames[i1][0] != (uintptr_t) NULL);

	if (strangeerrortrap == 0) {
		if (DEBUG)
			printf("Damned Combo-Box-Error !\n");
		emergency();
	}

	hasAskedPWD = 0;
	getBannerList(tmp);
	getOptions(tmp, -1);	/* Get PrinterOptions without PPD-Opts to have an password if saved. */
	ret = getPPDOpts(tmp);
	if (ret == -2) {	/* Not Authorized */
		gtk_entry_set_text(GTK_ENTRY
				   (GTK_COMBO(printerFrameCombo)->entry),
				   PrinterNames[lastPrinterChoice]);
	} else if (ret == 0) {	/* OK, printer has ppd */
		getOptions(tmp, 0);
		gtk_widget_hide(tabs);
		setDefaults();
		gtk_widget_show(tabs);
	}
	conflict_active = merkconflict;
	if (printerPPD != NULL)
		constraints(ppdConflicts(printerPPD));
}

void DuplexChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("Duplex toggled: %s\n", (char *)data);

	if (gtk_toggle_button_get_active
	    (GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 2)))) {
		if (strcmp((char *)data, "Button1") != 0)
			return;
		if (DEBUG)
			printf("No Duplex\n");
		constraints(ppdMarkOption(printerPPD, "Duplex", "None"));
		return;
	} else
	    if (gtk_toggle_button_get_active
		(GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 1)))) {
		if (DEBUG)
			printf("Duplex long edge binding\n");
		if (strcmp((char *)data, "Button2") != 0)
			return;
		constraints(ppdMarkOption
			    (printerPPD, "Duplex", "DuplexNoTumble"));
		return;
	} else
	    if (gtk_toggle_button_get_active
		(GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 0)))) {
		if (DEBUG)
			printf("Duplex short edge binding\n");
		if (strcmp((char *)data, "Button3") != 0)
			return;
		constraints(ppdMarkOption
			    (printerPPD, "Duplex", "DuplexTumble"));
		return;
	} else {
		if (DEBUG)
			printf("PROG-ERROR: Whats that for an Duplex mode ?\n");
		emergency();
	}
}

void PrinterDuplex(GtkWidget * container)
{
#include "duplex_none.xpm"
#include "duplex_long.xpm"
#include "duplex_short.xpm"
	GtkWidget *mainHBox, *vbox, *hbox;
	GtkWidget *button1, *button2, *button3;
	GtkWidget *lbox1, *lbox2, *lbox3;
	GtkWidget *pmw1, *pmw2, *pmw3;
	GdkPixmap *pm1, *pm2, *pm3;
	GdkBitmap *mask1, *mask2, *mask3;
	GtkStyle *style;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	printerDuplexFrame = gtk_frame_new(str2str(_("Duplex Settings")));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerDuplexFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(printerDuplexFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	/* CheckBoxes */
	lbox1 = gtk_hbox_new(FALSE, 0);
	style = gtk_widget_get_style(vbox);
	pm1 = gdk_pixmap_create_from_xpm_d(vbox->window, &mask1,
					   &style->bg[GTK_STATE_NORMAL],
					   duplex_none_xpm);
	pmw1 = gtk_pixmap_new(pm1, mask1);
	button1 =
	    gtk_radio_button_new_with_label(DuplexButtons,
					    str2str(_("No Duplex")));
	gtk_box_pack_start(GTK_BOX(lbox1), pmw1, FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(lbox1), button1, FALSE, FALSE, 3);
	gtk_widget_show(pmw1);
	gtk_box_pack_start(GTK_BOX(vbox), lbox1, FALSE, FALSE, 0);
	DuplexButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button1));
	gtk_widget_show(button1);
	gtk_widget_show(lbox1);

	lbox2 = gtk_hbox_new(FALSE, 0);
	style = gtk_widget_get_style(vbox);
	pm2 = gdk_pixmap_create_from_xpm_d(vbox->window, &mask2,
					   &style->bg[GTK_STATE_NORMAL],
					   duplex_long_xpm);
	pmw2 = gtk_pixmap_new(pm2, mask2);
	button2 =
	    gtk_radio_button_new_with_label(DuplexButtons,
					    str2str(_("Long edge binding")));
	gtk_box_pack_start(GTK_BOX(lbox2), pmw2, FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(lbox2), button2, FALSE, FALSE, 3);
	gtk_widget_show(pmw2);
	gtk_box_pack_start(GTK_BOX(vbox), lbox2, FALSE, FALSE, 0);
	DuplexButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button2));
	gtk_widget_show(button2);
	gtk_widget_show(lbox2);

	lbox3 = gtk_hbox_new(FALSE, 0);
	style = gtk_widget_get_style(vbox);
	pm3 = gdk_pixmap_create_from_xpm_d(vbox->window, &mask3,
					   &style->bg[GTK_STATE_NORMAL],
					   duplex_short_xpm);
	pmw3 = gtk_pixmap_new(pm3, mask3);
	button3 =
	    gtk_radio_button_new_with_label(DuplexButtons,
					    str2str(_("Short edge binding")));
	gtk_box_pack_start(GTK_BOX(lbox3), pmw3, FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(lbox3), button3, FALSE, FALSE, 3);
	gtk_widget_show(pmw3);
	gtk_box_pack_start(GTK_BOX(vbox), lbox3, FALSE, FALSE, 0);
	DuplexButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button3));
	gtk_widget_show(button3);
	gtk_widget_show(lbox3);

	gtk_signal_connect(GTK_OBJECT(button1), "toggled",
			   GTK_SIGNAL_FUNC(DuplexChanged), "Button1");
	gtk_signal_connect(GTK_OBJECT(button2), "toggled",
			   GTK_SIGNAL_FUNC(DuplexChanged), "Button2");
	gtk_signal_connect(GTK_OBJECT(button3), "toggled",
			   GTK_SIGNAL_FUNC(DuplexChanged), "Button3");

}

void PrinterMedia(GtkWidget * container)
{
	GtkWidget *mainHBox, *vbox;
	GtkWidget *labelSize, *labelType, *labelSource, *labelSizeX,
	    *labelSizeSpacing;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	printerMediaFrame = gtk_frame_new(str2str(_("Media Selections")));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerMediaFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
	gtk_container_add(GTK_CONTAINER(printerMediaFrame), vbox);
	gtk_widget_show(vbox);

	/* MediaSize */
	MediaSizeFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaSizeFrame);

	labelSize = gtk_label_new(str2str(_("Media Size")));
	gtk_misc_set_alignment(GTK_MISC(labelSize), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelSize), MEDIA_FIELDLENGTH,
				    -1);
	gtk_box_pack_start(GTK_BOX(MediaSizeFrame), labelSize, FALSE, FALSE, 0);
	gtk_widget_show(labelSize);

	printerMediaSizeCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(printerMediaSizeCombo)->list),
				    GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(printerMediaSizeCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(MediaSizeFrame), printerMediaSizeCombo,
			   FALSE, FALSE, 0);
	gtk_widget_show(printerMediaSizeCombo);

	/* Custom Media Size */
	CustomMediaSizeFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), CustomMediaSizeFrame);

	labelSizeSpacing = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(labelSizeSpacing), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelSizeSpacing),
				    MEDIA_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(CustomMediaSizeFrame), labelSizeSpacing,
			   FALSE, FALSE, 0);
	gtk_widget_show(labelSizeSpacing);

	customSizeFieldX =
	    gtk_entry_new_with_max_length(MAX_CUSTOM_SIZE_LENGTH);
	gtk_widget_set_size_request(customSizeFieldX,
				    MAX_CUSTOM_SIZE_FIELD_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY(customSizeFieldX), "");
	gtk_box_pack_start(GTK_BOX(CustomMediaSizeFrame), customSizeFieldX,
			   FALSE, FALSE, 0);
	gtk_widget_show(customSizeFieldX);

	labelSizeX = gtk_label_new(str2str("x"));
	gtk_box_pack_start(GTK_BOX(CustomMediaSizeFrame), labelSizeX, FALSE,
			   FALSE, 0);
	gtk_widget_show(labelSizeX);

	customSizeFieldY =
	    gtk_entry_new_with_max_length(MAX_CUSTOM_SIZE_LENGTH);
	gtk_widget_set_size_request(customSizeFieldY,
				    MAX_CUSTOM_SIZE_FIELD_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY(customSizeFieldY), "");
	gtk_box_pack_start(GTK_BOX(CustomMediaSizeFrame), customSizeFieldY,
			   FALSE, FALSE, 0);
	gtk_widget_show(customSizeFieldY);

	customSizeCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(customSizeCombo)->list),
				    GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(customSizeCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(CustomMediaSizeFrame), customSizeCombo,
			   FALSE, FALSE, 0);
	gtk_widget_set_size_request(GTK_WIDGET(customSizeCombo), 60, -1);
	gtk_widget_show(customSizeCombo);

	/* Signals */
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(customSizeFieldX)),
			   "focus-out-event",
			   GTK_SIGNAL_FUNC(customSizeFieldChanged), NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(customSizeFieldY)),
			   "focus-out-event",
			   GTK_SIGNAL_FUNC(customSizeFieldChanged), NULL);

	/* MediaType */
	MediaTypeFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaTypeFrame);

	labelType = gtk_label_new(str2str(_("Media Type")));
	gtk_misc_set_alignment(GTK_MISC(labelType), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelType), MEDIA_FIELDLENGTH,
				    -1);
	gtk_box_pack_start(GTK_BOX(MediaTypeFrame), labelType, FALSE, FALSE, 0);
	gtk_widget_show(labelType);

	printerMediaTypeCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(printerMediaTypeCombo)->list),
				    GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(printerMediaTypeCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(MediaTypeFrame), printerMediaTypeCombo,
			   FALSE, FALSE, 0);
	gtk_widget_show(printerMediaTypeCombo);

	/* MediaSource */
	MediaSourceFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaSourceFrame);

	labelSource = gtk_label_new(str2str(_("Media Source")));
	gtk_misc_set_alignment(GTK_MISC(labelSource), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelSource), MEDIA_FIELDLENGTH,
				    -1);
	gtk_box_pack_start(GTK_BOX(MediaSourceFrame), labelSource, FALSE, FALSE,
			   0);
	gtk_widget_show(labelSource);

	printerMediaSourceCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(printerMediaSourceCombo)->list),
				    GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(printerMediaSourceCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(MediaSourceFrame), printerMediaSourceCombo,
			   FALSE, FALSE, 0);
	gtk_widget_show(printerMediaSourceCombo);

#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(printerMediaSizeCombo)->entry),
			   "changed", PPD_DropDown_changed, "PageSize");
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(printerMediaTypeCombo)->entry),
			   "changed", PPD_DropDown_changed, "MediaType");
	gtk_signal_connect(GTK_OBJECT
			   (GTK_COMBO(printerMediaSourceCombo)->entry),
			   "changed", PPD_DropDown_changed, "InputSlot");
#else
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(printerMediaSizeCombo)->entry),
			   "changed", GTK_SIGNAL_FUNC(PPD_DropDown_changed),
			   "PageSize");
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(printerMediaTypeCombo)->entry),
			   "changed", GTK_SIGNAL_FUNC(PPD_DropDown_changed),
			   "MediaType");
	gtk_signal_connect(GTK_OBJECT
			   (GTK_COMBO(printerMediaSourceCombo)->entry),
			   "changed", GTK_SIGNAL_FUNC(PPD_DropDown_changed),
			   "InputSlot");
#endif

}

void PrinterNumCopies(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox;
	GtkObject *adjust;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, 0);
	gtk_widget_show(mainHBox);

	printerNumCopiesFrame = gtk_frame_new(str2str(_("Number of Copies")));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerNumCopiesFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(printerNumCopiesFrame), hbox);
	gtk_widget_show(hbox);

	/* Spinbutton */
	//adjust = gtk_adjustment_new (1.0, 1.0, 999, 1.0, 1.0, 1.0);
	adjust = gtk_adjustment_new(1.0, 1.0, 999, 1.0, 1.0, 0);
	printerNumField = gtk_spin_button_new(GTK_ADJUSTMENT(adjust), 1.0, 0);
	gtk_widget_set_size_request(printerNumField, 3 * 16, -1);
	gtk_box_pack_start(GTK_BOX(hbox), printerNumField, FALSE, FALSE, 0);
	gtk_widget_show(printerNumField);

	/* collate Check */
	collateCheck =
	    gtk_check_button_new_with_label(str2str(_("Collate Copies")));
	gtk_box_pack_end(GTK_BOX(hbox), collateCheck, FALSE, FALSE, 0);
	gtk_widget_show(collateCheck);

	/* Signals */
#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect(GTK_OBJECT(GTK_SPIN_BUTTON(printerNumField)),
			   "changed", GTK_SIGNAL_FUNC(NumOfCopiesChanged),
			   NULL);
#else
	gtk_signal_connect(GTK_OBJECT(GTK_SPIN_BUTTON(printerNumField)),
			   "value-changed", GTK_SIGNAL_FUNC(NumOfCopiesChanged),
			   NULL);
#endif
	gtk_signal_connect(GTK_OBJECT(collateCheck), "pressed",
			   GTK_SIGNAL_FUNC(wantCollateCheck), NULL);

	PrinterCallGtklpq(mainHBox);
}

void callGtkLPQFunc(GtkButton * button, gpointer data)
{
	system(GTKLPQCOM);
}

void recreatePrinterList(void)
{
	GList *printerList = NULL;
	int i1;

	if (DEBUG)
		printf("Recreating printer list\n");
	for (i1 = 0; PrinterFavNames[i1][0] != (uintptr_t) NULL; i1++) {
		if (DEBUG)
			printf("Adding favorite %s to printer list\n",
			       PrinterFavNames[i1]);
		printerList =
		    g_list_append(printerList, str2str(PrinterFavNames[i1]));
	}
	if (PrinterFavNames[0][0] != (uintptr_t) NULL)
		printerList = g_list_append(printerList, "-----");
	for (i1 = 0; i1 <= PrinterNum; i1++) {
		printerList =
		    g_list_append(printerList, str2str(PrinterNames[i1]));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(printerFrameCombo),
				      printerList);
}

void loadFavorites(void)
{
	char *home;

	/* load */
	home = getenv("HOME");
	if (home) {
		FILE *file = NULL;
		char filepath[MAXPATH + 1];

		snprintf(filepath, (size_t) MAXPATH, "%s/favorites", confdir);
		if (PrinterFavNames[0][0] != (uintptr_t) NULL)
			memset(PrinterFavNames, 0, sizeof(PrinterFavNames));
		if (DEBUG)
			printf("Load favorites from %s\n", filepath);
		file = fopen(filepath, "r");
		if (file) {
			int i1 = 0;
			while (fscanf(file, "%s", PrinterFavNames[i1]) > 0) {
				if (DEBUG)
					printf("%u: %s\n", i1,
					       PrinterFavNames[i1]);
				i1++;
			}
			fclose(file);
			if (DEBUG)
				printf("%u Favorites loaded.\n", i1);
			checkFavorites();
		} else {
			if (DEBUG)
				printf("Could not open favorites file\n");
		}
	}
}

void saveFavorites(void)
{
	char *home;

	/* save */
	home = getenv("HOME");
	if (home) {
		FILE *file = NULL;
		char filepath[MAXPATH + 1];

		snprintf(filepath, (size_t) MAXPATH, "%s/favorites", confdir);
		if (DEBUG)
			printf("Favorites now:\n");
		if (PrinterFavNames[0][0] != (uintptr_t) NULL) {
			int i1;

			if (DEBUG)
				printf("Save to %s\n", filepath);
			file = fopen(filepath, "w");
			i1 = 0;
			while (PrinterFavNames[i1][0] != (uintptr_t) NULL) {
				if (DEBUG)
					printf("%u: %s\n", i1,
					       PrinterFavNames[i1]);
				if (file)
					fprintf(file, "%s\n",
						PrinterFavNames[i1]);
				i1++;
			}
			if (file)
				fclose(file);
		} else {
			if (DEBUG)
				printf("Delete %s\n", filepath);
			unlink(filepath);
		}
	}
}

void callAddFavFunc(GtkButton * button, gpointer data)
{
	int found;
	int i1;
	char tmp[DEF_PRN_LEN + 1];

	snprintf(tmp, (size_t) DEF_PRN_LEN, "%s",
		 gtk_entry_get_text(GTK_ENTRY
				    (GTK_COMBO(printerFrameCombo)->entry)));
	/* Add to favorites, if not yet there */
	i1 = 0;
	found = 0;
	while (PrinterFavNames[i1][0] != (uintptr_t) NULL) {
		if (strcmp(tmp, str2str(PrinterFavNames[i1])) == 0) {
			if (DEBUG)
				printf("Printer %s already in favorites\n",
				       tmp);
			found = 1;
			break;
		}
		i1++;
	}
	if (!found && (i1 < MAX_PRT)) {
		strcpy(PrinterFavNames[i1], tmp);
		if (DEBUG)
			printf("Printer %s added to favorites\n", tmp);
		recreatePrinterList();
		saveFavorites();
	}
}

void callRmFavFunc(GtkButton * button, gpointer data)
{
	int found;
	int i1;
	char tmp[DEF_PRN_LEN + 1];

	snprintf(tmp, (size_t) DEF_PRN_LEN, "%s",
		 gtk_entry_get_text(GTK_ENTRY
				    (GTK_COMBO(printerFrameCombo)->entry)));
	/* Remove from favorites, if there */
	i1 = 0;
	found = 0;
	while (PrinterFavNames[i1][0] != (uintptr_t) NULL) {
		if (strcmp(tmp, str2str(PrinterFavNames[i1])) == 0) {
			found = 1;
			break;
		}
		i1++;
	}
	if (found) {
		/* find last entry and move here */
		unsigned i2;

		if (DEBUG)
			printf("Remove %s from favorites\n",
			       PrinterFavNames[i1]);
		i2 = i1 + 1;
		while (PrinterFavNames[i2][0])
			i2++;
		i2--;
		if (i2 != i1) {
			strcpy(PrinterFavNames[i1], PrinterFavNames[i2]);
		}
		PrinterFavNames[i2][0] = 0;
		recreatePrinterList();
		saveFavorites();
	} else {
		if (DEBUG)
			printf("Printer %s not in favorites\n", tmp);
	}
}

void PrinterCallGtklpq(GtkWidget * container)
{
	GtkWidget *hbox, *gtklpqbutton;

	gtklpqFrame = gtk_frame_new(str2str(_("Other actions")));
	gtk_box_pack_end(GTK_BOX(container), gtklpqFrame, TRUE, TRUE,
			 FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(gtklpqFrame), hbox);
	gtk_widget_show(hbox);

	/* Button */
	gtklpqbutton = gtk_button_new_with_label(str2str(_("Call GtkLPQ")));
	gtk_box_pack_start(GTK_BOX(hbox), gtklpqbutton, FALSE, TRUE, 0);
	gtk_widget_show(gtklpqbutton);
	gtk_signal_connect(GTK_OBJECT(GTK_BUTTON(gtklpqbutton)), "clicked",
			   GTK_SIGNAL_FUNC(callGtkLPQFunc), NULL);

}

void PrinterFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *vboxp, *hboxfav, *addfavbutton,
	    *rmfavbutton;

	loadFavorites();

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	printerFrame = gtk_frame_new(str2str(_("Printer")));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Printer Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(printerFrame), hbox);
	gtk_widget_show(hbox);

	/* Printer select vbox */
	vboxp = gtk_vbox_new(FALSE, 0);
	gtk_box_set_spacing(GTK_BOX(vboxp), 0);
	gtk_box_pack_start(GTK_BOX(hbox), vboxp, FALSE, FALSE, 0);
	gtk_widget_show(vboxp);

	printerFrameCombo = gtk_combo_new();

	recreatePrinterList();
	gtk_combo_set_value_in_list(GTK_COMBO(printerFrameCombo), TRUE, FALSE);
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(printerFrameCombo)->list),
				    GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(printerFrameCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(vboxp), printerFrameCombo, FALSE, FALSE, 0);
	gtk_widget_show(printerFrameCombo);

	/* Favorites hbox */
	hboxfav = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_end(GTK_BOX(vboxp), hboxfav, FALSE, FALSE, 0);
	gtk_widget_show(hboxfav);
	/* Add Favorites Button */
	addfavbutton = gtk_button_new_with_label(str2str(_("Add favorite")));
	gtk_box_pack_start(GTK_BOX(hboxfav), addfavbutton, FALSE, TRUE, 0);
	gtk_widget_show(addfavbutton);
	gtk_signal_connect(GTK_OBJECT(GTK_BUTTON(addfavbutton)), "clicked",
			   GTK_SIGNAL_FUNC(callAddFavFunc), NULL);

	/* Remove Favorites Button */
	rmfavbutton = gtk_button_new_with_label(str2str(_("Remove favorite")));
	gtk_box_pack_end(GTK_BOX(hboxfav), rmfavbutton, FALSE, TRUE, 0);
	gtk_widget_show(rmfavbutton);
	gtk_signal_connect(GTK_OBJECT(GTK_BUTTON(rmfavbutton)), "clicked",
			   GTK_SIGNAL_FUNC(callRmFavFunc), NULL);

	/* InfoBoxen */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_set_spacing(GTK_BOX(vbox), 0);
	gtk_box_pack_end(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_widget_show(vbox);

	/* Information */
	printerFrameInfoText = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameInfoText), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameInfoText),
			      GTK_JUSTIFY_LEFT);
	gtk_widget_set_size_request(printerFrameInfoText,
				    PRINTER_INFO_FIELD_LENGTH, -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameInfoText, FALSE, FALSE,
			   0);
	gtk_widget_show(printerFrameInfoText);

	/* Location */
	printerFrameLocationText = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameLocationText), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameLocationText),
			      GTK_JUSTIFY_LEFT);
	gtk_widget_set_size_request(printerFrameLocationText,
				    PRINTER_INFO_FIELD_LENGTH, -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameLocationText, FALSE,
			   FALSE, 0);
	gtk_widget_show(printerFrameLocationText);

	/* Signals */
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(printerFrameCombo)->entry),
			   "changed", GTK_SIGNAL_FUNC(PrinterChanged), NULL);
}

void GeneralTab(void)
{
	generalTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(generalTab), FRAME_BORDER);

	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), generalTab,
				 gtk_label_new(str2str(_("General"))));

	PrinterFrame(generalTab);
	PrinterNumCopies(generalTab);
	PrinterMedia(generalTab);
	PrinterDuplex(generalTab);
}
