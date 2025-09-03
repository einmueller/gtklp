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

#include "output.h"

#include <stdio.h>
#include <ctype.h>
#include <gtk/gtk.h>

#include <gettext.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"

#include "black.xpm"
#include "white.xpm"
#include "borderDD.xpm"
#include "borderD.xpm"
#include "borderkdd.xpm"
#include "borderkd.xpm"
#include "bordernone.xpm"
#include "btlr.xpm"
#include "btrl.xpm"
#include "lrbt.xpm"
#include "lrtb.xpm"
#include "rlbt.xpm"
#include "rltb.xpm"
#include "tblr.xpm"
#include "tbrl.xpm"
#include "duplex_none.xpm"
#include "landscape.xpm"

GSList *rangeButtons, *sheetsButtons, *nupLayButtons, *borderButtons,
    *orientButtons;
GtkWidget *outputTab;
GtkWidget *rangesFrame, *rangeCheck, *rangeText, *outputOrderCheck;
GtkWidget *sheetsFrame;
GtkWidget *brightFrame, *gammaFrame;
GtkObject *brightAdj, *gammaAdj;
GtkWidget *nupCombo;
GtkWidget *nupStyleButton;
int nupStyle;
GtkWidget *nupPixMapWid = NULL;
GtkWidget *mirrorCheck;

void outputTabSetDefaults(void)
{
	if (DEBUG)
		printf("Get outputTabDefaults\n");

	/* Range Text */
	gtk_entry_set_text(GTK_ENTRY(rangeText), Range);
	if (wantRange) {
		gtk_widget_show(rangeText);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rangeCheck), 1);
	} else {
		gtk_widget_hide(rangeText);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rangeCheck), 0);
	}
	/* Range Buttons */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (g_slist_nth_data
				      (rangeButtons, rangeType)), TRUE);
	/* OutputOrder */
	if (wantReverseOutputOrder == 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (outputOrderCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (outputOrderCheck), 0);
	/* Mirror Output */
	if (mirroroutput == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mirrorCheck), 0);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mirrorCheck), 1);

	/* Sheets Type */

	switch (sheetsType) {
	case 1:
	case 2:
	case 3:
	case 4:
		gtk_list_select_item(GTK_LIST(GTK_COMBO(nupCombo)->list),
				     sheetsType - 1);
		break;
	case 6:
		gtk_list_select_item(GTK_LIST(GTK_COMBO(nupCombo)->list), 4);
		break;
	case 9:
		gtk_list_select_item(GTK_LIST(GTK_COMBO(nupCombo)->list), 5);
		break;
	case 16:
		gtk_list_select_item(GTK_LIST(GTK_COMBO(nupCombo)->list), 6);
		break;
	}

	nupStyle = 0;
	/* Number-Up-Layout */
	if (strcmp(nupLayout, "btlr") == 0)
		nupStylePix(0);
	else if (strcmp(nupLayout, "btrl") == 0)
		nupStylePix(1);
	else if (strcmp(nupLayout, "lrbt") == 0)
		nupStylePix(2);
	else if (strcmp(nupLayout, "lrtb") == 0)
		nupStylePix(3);
	else if (strcmp(nupLayout, "rlbt") == 0)
		nupStylePix(4);
	else if (strcmp(nupLayout, "rltb") == 0)
		nupStylePix(5);
	else if (strcmp(nupLayout, "tblr") == 0)
		nupStylePix(6);
	else if (strcmp(nupLayout, "tbrl") == 0)
		nupStylePix(7);

	/* page-border */
	if (strcmp(borderType, "none") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (borderButtons, 4)), TRUE);
	if (strcmp(borderType, "single") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (borderButtons, 3)), TRUE);
	if (strcmp(borderType, "single-thick") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (borderButtons, 2)), TRUE);
	if (strcmp(borderType, "double") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (borderButtons, 1)), TRUE);
	if (strcmp(borderType, "double-thick") == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (borderButtons, 0)), TRUE);

	/* Landscape */
	if (wantLandscape)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (orientButtons, 0)), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (g_slist_nth_data
					      (orientButtons, 1)), TRUE);
	/* Brightness */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), (int)brightness);
	/* Gamma */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), (int)ggamma);

}

void rangeCheckChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("rangeChanged:%i\n",
		       gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						    (rangeCheck)));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rangeCheck))) {
		gtk_widget_hide(rangeText);
		wantRange = 0;
	} else {
		gtk_widget_show(rangeText);
		wantRange = 1;
	}
}

void outputOrderCheckChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("outputOrderChanged:%i\n",
		       gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						    (outputOrderCheck)));
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(outputOrderCheck))) {
		wantReverseOutputOrder = 0;
	} else {
		wantReverseOutputOrder = 1;
	}
}

void rangeTextChanged(GtkWidget * widget, gpointer data)
{
	char tmp[MAXRANGEENTRY + 1];
	unsigned i1, flg = 0;

	strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(rangeText), 0, -1),
		(size_t) MAXRANGEENTRY);
	if (DEBUG)
		printf("rangeText: %s\n", tmp);

	for (i1 = 0; i1 < strlen(tmp); i1++) {
		if ((isdigit(tmp[i1]) == 0) && (tmp[i1] != ',')
		    && (tmp[i1] != '-')) {
			gtk_entry_set_text(GTK_ENTRY(rangeText), Range);
			gtk_editable_set_position(GTK_EDITABLE(rangeText), -1);
			flg = 1;
			break;
		}
	}
	if ((flg == 0) && tmp[0] != (uintptr_t) NULL) {
		strncpy(Range, tmp, (size_t) MAXRANGEENTRY);
		if (DEBUG)
			printf("Range: Value changed: %s\n", Range);
	}

}

void rangeButtonsChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("Range toggled: %s\n", (char *)data);
	if (gtk_toggle_button_get_active
	    (GTK_TOGGLE_BUTTON(g_slist_nth_data(rangeButtons, 2)))) {
		if (DEBUG)
			printf("ALL\n");
		rangeType = 2;
		return;
	} else
	    if (gtk_toggle_button_get_active
		(GTK_TOGGLE_BUTTON(g_slist_nth_data(rangeButtons, 1)))) {
		if (DEBUG)
			printf("EVEN\n");
		rangeType = 1;
		return;
	} else
	    if (gtk_toggle_button_get_active
		(GTK_TOGGLE_BUTTON(g_slist_nth_data(rangeButtons, 0)))) {
		if (DEBUG)
			printf("ODD\n");
		rangeType = 0;
		return;
	} else {
		if (DEBUG)
			printf("PROG-ERROR: Whats that for an range Type ?\n");
		emergency();
	}
}

void sheetsChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("sheetsChanged: %s\n",
		       gtk_entry_get_text(GTK_ENTRY(widget)));

	sheetsType = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
	return;
}

void landscapeCheckChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("landscapeChanged: %s\n", (char *)data);
	if (strcmp((char *)data, "portrait") == 0) {
		wantLandscape = 0;
	} else {
		wantLandscape = 1;
	}
}

void borderChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("Border changed: %s\n", (char *)data);
	strncpy(borderType, (char *)data, 12);
}

void nupStylePix(int i1)
{
	GdkBitmap *mask;
	GtkStyle *style;
	GdkPixmap *nupPixMap;

	nupPixMap = NULL;

	style = gtk_widget_get_style(mainWindow);

	if (nupPixMapWid != NULL)
		gtk_container_remove(GTK_CONTAINER(nupStyleButton),
				     nupPixMapWid);

	switch (i1) {
	case 0:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_btlr);
		strncpy(nupLayout, "btlr", 4);
		break;
	case 1:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_btrl);
		strncpy(nupLayout, "btrl", 4);
		break;
	case 2:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_lrbt);
		strncpy(nupLayout, "lrbt", 4);
		break;
	case 3:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_lrtb);
		strncpy(nupLayout, "lrtb", 4);
		break;
	case 4:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_rlbt);
		strncpy(nupLayout, "rlbt", 4);
		break;
	case 5:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_rltb);
		strncpy(nupLayout, "rltb", 4);
		break;
	case 6:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_tblr);
		strncpy(nupLayout, "tblr", 4);
		break;
	case 7:
		nupPixMap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_tbrl);
		strncpy(nupLayout, "tbrl", 4);
		break;

	}

	nupPixMapWid = gtk_pixmap_new(nupPixMap, mask);
	gtk_container_add(GTK_CONTAINER(nupStyleButton), nupPixMapWid);
	gtk_widget_show(nupPixMapWid);

	if (DEBUG)
		printf("nupLayout: %s\n", nupLayout);

}

void nupStyleChange(GtkWidget * widget, gpointer data)
{
	nupStyle += atoi((char *)data);
	if (nupStyle < 0)
		nupStyle = 7;
	if (nupStyle > 7)
		nupStyle = 0;
	if (DEBUG)
		printf("nupStyleChanged: %i\n", nupStyle);
	nupStylePix(nupStyle);
}

void wantMirrorCheck(GtkWidget * widget, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mirrorCheck))) {
		mirroroutput = 0;
	} else {
		mirroroutput = 1;
	}
	if (DEBUG)
		printf("mirroroutput changed: %u\n", mirroroutput);
}

void sheetUsageFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *label, *borBox;
	GtkWidget *bor0, *bor1, *bor2, *bor3, *bor4;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;
	GList *nupList;
	GtkWidget *left, *right, *leftButton, *rightButton;
	GtkWidget *pixmapwid;
	GtkWidget *or0, *or1;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, 0);
	gtk_widget_show(mainHBox);
	sheetsFrame = gtk_frame_new(str2str(_("Sheet Usage")));
	gtk_box_pack_start(GTK_BOX(mainHBox), sheetsFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(sheetsFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0 * INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_widget_show(hbox1);

	label = gtk_label_new(str2str(_("Sheets per page")));
	gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(label);

	nupList = NULL;
	nupList = g_list_append(nupList, "1");
	nupList = g_list_append(nupList, "2");
	nupList = g_list_append(nupList, "3");
	nupList = g_list_append(nupList, "4");
	nupList = g_list_append(nupList, "6");
	nupList = g_list_append(nupList, "9");
	nupList = g_list_append(nupList, "16");

	nupCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(nupCombo)->list),
				    GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(nupCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox1), nupCombo, FALSE, FALSE, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(nupCombo), nupList);
	gtk_widget_set_size_request(nupCombo, NUPCOMBOWIDTH, -1);
	gtk_widget_show(nupCombo);

#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(nupCombo)->entry), "changed",
			   sheetsChanged, NULL);
#else
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(nupCombo)->entry), "changed",
			   GTK_SIGNAL_FUNC(sheetsChanged), NULL);
#endif

	label = gtk_label_new(str2str(_("Layout")));
	gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, FALSE,
			   FRAME_SPACING_H);
	gtk_widget_show(label);

	leftButton = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox1), leftButton, FALSE, FALSE, 0);
	left = gtk_arrow_new(GTK_ARROW_LEFT, GTK_SHADOW_OUT);
	gtk_container_add(GTK_CONTAINER(leftButton), left);
	gtk_widget_show(left);
	gtk_widget_show(leftButton);

	nupStyleButton = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox1), nupStyleButton, FALSE, FALSE, 0);
	nupStylePix(0);
	gtk_widget_show(nupStyleButton);

	rightButton = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox1), rightButton, FALSE, FALSE, 0);
	right = gtk_arrow_new(GTK_ARROW_RIGHT, GTK_SHADOW_OUT);
	gtk_container_add(GTK_CONTAINER(rightButton), right);
	gtk_widget_show(right);
	gtk_widget_show(rightButton);

	gtk_signal_connect(GTK_OBJECT(leftButton), "pressed",
			   GTK_SIGNAL_FUNC(nupStyleChange), "-1");
	gtk_signal_connect(GTK_OBJECT(rightButton), "pressed",
			   GTK_SIGNAL_FUNC(nupStyleChange), "1");

	/* --- */
	borBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), borBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(borBox);

	label = gtk_label_new(str2str(_("Border")));
	gtk_box_pack_start(GTK_BOX(borBox), label, FALSE, FALSE,
			   FRAME_SPACING_H);
	gtk_widget_show(label);

	bor4 = gtk_radio_button_new(NULL);
	gtk_box_pack_start(GTK_BOX(borBox), bor4, FALSE, FALSE, 0);
	borderButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(bor4));
	style = gtk_widget_get_style(vbox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_bordernone);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(bor4), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(bor4);

	bor0 = gtk_radio_button_new(borderButtons);
	gtk_box_pack_start(GTK_BOX(borBox), bor0, FALSE, FALSE, 0);
	borderButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(bor0));
	style = gtk_widget_get_style(vbox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_borderkd);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(bor0), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(bor0);

	bor1 = gtk_radio_button_new(borderButtons);
	gtk_box_pack_start(GTK_BOX(borBox), bor1, FALSE, FALSE, 0);
	borderButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(bor1));
	style = gtk_widget_get_style(vbox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_borderD);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(bor1), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(bor1);

	bor2 = gtk_radio_button_new(borderButtons);
	gtk_box_pack_start(GTK_BOX(borBox), bor2, FALSE, FALSE, 0);
	borderButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(bor2));
	style = gtk_widget_get_style(vbox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_borderkdd);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(bor2), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(bor2);

	bor3 = gtk_radio_button_new(borderButtons);
	gtk_box_pack_start(GTK_BOX(borBox), bor3, FALSE, FALSE, 0);
	borderButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(bor3));
	style = gtk_widget_get_style(vbox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_borderDD);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(bor3), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(bor3);

	gtk_signal_connect(GTK_OBJECT(bor0), "pressed",
			   GTK_SIGNAL_FUNC(borderChanged), "single");
	gtk_signal_connect(GTK_OBJECT(bor1), "pressed",
			   GTK_SIGNAL_FUNC(borderChanged), "single-thick");
	gtk_signal_connect(GTK_OBJECT(bor2), "pressed",
			   GTK_SIGNAL_FUNC(borderChanged), "double");
	gtk_signal_connect(GTK_OBJECT(bor3), "pressed",
			   GTK_SIGNAL_FUNC(borderChanged), "double-thick");
	gtk_signal_connect(GTK_OBJECT(bor4), "pressed",
			   GTK_SIGNAL_FUNC(borderChanged), "none");

	/* --- */
	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 0);
	gtk_widget_show(hbox2);

	label = gtk_label_new(str2str(_("Orientation")));
	gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE,
			   FRAME_SPACING_H);
	gtk_widget_show(label);

	style = gtk_widget_get_style(mainWindow);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) duplex_none_xpm);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	or0 = gtk_radio_button_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox2), or0, FALSE, FALSE, 0);
	orientButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(or0));
	gtk_container_add(GTK_CONTAINER(or0), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(or0);

	style = gtk_widget_get_style(mainWindow);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) landscape_xpm);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	or1 = gtk_radio_button_new(orientButtons);
	gtk_box_pack_start(GTK_BOX(hbox2), or1, FALSE, FALSE, 0);
	orientButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(or1));
	gtk_container_add(GTK_CONTAINER(or1), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_widget_show(or1);

	mirrorCheck =
	    gtk_check_button_new_with_label(str2str(_("Mirror Output")));
	gtk_box_pack_end(GTK_BOX(hbox2), mirrorCheck, FALSE, FALSE,
			 FRAME_SPACING_H);
	gtk_widget_show(mirrorCheck);

	gtk_signal_connect(GTK_OBJECT(mirrorCheck), "pressed",
			   GTK_SIGNAL_FUNC(wantMirrorCheck), NULL);

	gtk_signal_connect(GTK_OBJECT(or0), "pressed",
			   GTK_SIGNAL_FUNC(landscapeCheckChanged), "portrait");
	gtk_signal_connect(GTK_OBJECT(or1), "pressed",
			   GTK_SIGNAL_FUNC(landscapeCheckChanged), "landscape");

}

void RangesFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *label;
	GtkWidget *button1, *button2, *button3;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	rangesFrame = gtk_frame_new(str2str(_("Ranges")));
	gtk_box_pack_start(GTK_BOX(mainHBox), rangesFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(rangesFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);
	gtk_widget_show(hbox1);

	rangeCheck = gtk_check_button_new_with_label(str2str(_("Print Range")));
	gtk_box_pack_start(GTK_BOX(hbox1), rangeCheck, FALSE, FALSE,
			   FRAME_SPACING_H);
	gtk_signal_connect(GTK_OBJECT(rangeCheck), "pressed",
			   GTK_SIGNAL_FUNC(rangeCheckChanged), "RangeCheck");
	gtk_widget_show(rangeCheck);

	rangeText = gtk_entry_new_with_max_length(MAXRANGEENTRY);
	gtk_widget_set_size_request(rangeText, RANGE_FIELDLENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY(rangeText), "");
	gtk_box_pack_start(GTK_BOX(hbox1), rangeText, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(rangeText);

	label = gtk_label_new(str2str(_("(e.g. 1-4,9,10-12)")));
	gtk_box_pack_end(GTK_BOX(hbox1), label, FALSE, FALSE, FRAME_SPACING_H);
	gtk_widget_show(label);

	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(rangeText)), "changed",
			   GTK_SIGNAL_FUNC(rangeTextChanged), NULL);

	hbox2 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox2);

	button1 = gtk_radio_button_new_with_label(NULL, str2str(_("All")));
	gtk_box_pack_start(GTK_BOX(hbox2), button1, FALSE, FALSE,
			   FRAME_SPACING_H);
	rangeButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button1));
	gtk_widget_show(button1);

	button2 =
	    gtk_radio_button_new_with_label(rangeButtons, str2str(_("Even")));
	gtk_box_pack_start(GTK_BOX(hbox2), button2, FALSE, FALSE, 0);
	rangeButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button2));
	gtk_widget_show(button2);

	button3 =
	    gtk_radio_button_new_with_label(rangeButtons, str2str(_("Odd")));
	gtk_box_pack_start(GTK_BOX(hbox2), button3, FALSE, FALSE, 0);
	rangeButtons = gtk_radio_button_group(GTK_RADIO_BUTTON(button2));
	gtk_widget_show(button3);

	outputOrderCheck =
	    gtk_check_button_new_with_label(str2str(_("Reverse Output Order")));
	gtk_box_pack_end(GTK_BOX(hbox2), outputOrderCheck, FALSE, FALSE,
			 FRAME_SPACING_H);
	gtk_signal_connect(GTK_OBJECT(outputOrderCheck), "pressed",
			   GTK_SIGNAL_FUNC(outputOrderCheckChanged),
			   "outputOrderCheck");
	gtk_widget_show(outputOrderCheck);

	gtk_signal_connect(GTK_OBJECT(button1), "toggled",
			   GTK_SIGNAL_FUNC(rangeButtonsChanged), "Button1");
	gtk_signal_connect(GTK_OBJECT(button2), "toggled",
			   GTK_SIGNAL_FUNC(rangeButtonsChanged), "Button2");
	gtk_signal_connect(GTK_OBJECT(button3), "toggled",
			   GTK_SIGNAL_FUNC(rangeButtonsChanged), "Button3");

}

void BrightFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *brightScale, *button1,
	    *button2;
	GtkWidget *pixmapwid;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	brightFrame = gtk_frame_new(str2str(_("Brightness")));
	gtk_box_pack_start(GTK_BOX(mainHBox), brightFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(brightFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	style = gtk_widget_get_style(mainHBox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_black);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(hbox1), pixmapwid, FALSE, FALSE,
			   MESSAGE_SPACING_H);
	gtk_widget_show(pixmapwid);

	brightAdj =
	    gtk_adjustment_new(0, BRIGHT_MIN, BRIGHT_MAX, BRIGHT_STEP,
			       BRIGHT_PAGE, 0);
	brightScale = gtk_hscale_new(GTK_ADJUSTMENT(brightAdj));
	gtk_scale_set_digits(GTK_SCALE(brightScale), BRIGHT_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(brightScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(brightScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(brightScale),
				    GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), brightScale, TRUE, TRUE, 0);
	gtk_widget_show(brightScale);

	style = gtk_widget_get_style(mainHBox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_white);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(hbox1), pixmapwid, FALSE, FALSE,
			   MESSAGE_SPACING_H);
	gtk_widget_show(pixmapwid);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox2);

	button1 = gtk_button_new_with_label(str2str(_("Default")));
	button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox2), button1, FALSE, FALSE,
			 BUTTON_SPACING_H);
	gtk_widget_show(button1);

	button2 = gtk_button_new_with_label(str2str(_("Reset")));
	button_pad(button2);
	gtk_box_pack_end(GTK_BOX(hbox2), button2, FALSE, FALSE, 0);
	gtk_widget_show(button2);

	gtk_signal_connect(GTK_OBJECT(button1), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "BrightnessDefault");
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "Brightness");

}

void GammaFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *gammaScale, *button1,
	    *button2;
	GtkWidget *pixmapwid;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	gammaFrame = gtk_frame_new(str2str(_("Gamma correction")));
	gtk_box_pack_start(GTK_BOX(mainHBox), gammaFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(gammaFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	style = gtk_widget_get_style(mainHBox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_black);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(hbox1), pixmapwid, FALSE, FALSE,
			   MESSAGE_SPACING_H);
	gtk_widget_show(pixmapwid);

	gammaAdj =
	    gtk_adjustment_new(0, GAMMA_MIN, GAMMA_MAX, GAMMA_STEP, GAMMA_PAGE,
			       0);
	gammaScale = gtk_hscale_new(GTK_ADJUSTMENT(gammaAdj));
	gtk_scale_set_digits(GTK_SCALE(gammaScale), GAMMA_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(gammaScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(gammaScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(gammaScale),
				    GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), gammaScale, TRUE, TRUE, 0);
	gtk_widget_show(gammaScale);

	style = gtk_widget_get_style(mainHBox);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_white);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(hbox1), pixmapwid, FALSE, FALSE,
			   MESSAGE_SPACING_H);
	gtk_widget_show(pixmapwid);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox2);

	button1 = gtk_button_new_with_label(str2str(_("Default")));
	button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox2), button1, FALSE, FALSE,
			 BUTTON_SPACING_H);
	gtk_widget_show(button1);

	button2 = gtk_button_new_with_label(str2str(_("Reset")));
	button_pad(button2);
	gtk_box_pack_end(GTK_BOX(hbox2), button2, FALSE, FALSE, 0);
	gtk_widget_show(button2);

	gtk_signal_connect(GTK_OBJECT(button1), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "GammaDefault");
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "Gamma");

}

void OutputTab(void)
{
	outputTab = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), outputTab,
				 gtk_label_new(str2str(_("Output"))));

	RangesFrame(outputTab);
	sheetUsageFrame(outputTab);
	BrightFrame(outputTab);
	GammaFrame(outputTab);
}
