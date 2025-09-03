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

#include "image.h"
#include "imgpos0.xpm"
#include "imgpos1.xpm"

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include <gettext.h>
#include <defaults.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"

GtkWidget *imageTab;
GtkWidget *imageScalingAllFrame;
GtkObject *imageScaleAllAdj;

GtkWidget *imageHUEFrame, *imageHUEButton;
GtkObject *imageHUEAdj;
GtkWidget *imageSatFrame, *imageSatButton;
GtkObject *imageSatAdj;
GtkWidget *imagePosFrame;
GtkWidget *scaleTypeCombo;
GtkWidget *imageScaleAllScale;
GtkWidget *imagePosBut[3][3];

void imageTabSetDefaults(void)
{
	int i1, i2;
	GtkStyle *style;
	GdkBitmap *mask;
	GdkPixmap *pm;
	GtkWidget *image;

	if (DEBUG)
		printf("Get imageTabDefaults\n");

	/* Position */
	for (i1 = 0; i1 < 3; i1++) {
		for (i2 = 0; i2 < 3; i2++) {
			gtk_container_remove(GTK_CONTAINER(imagePosBut[i1][i2]),
					     g_list_nth_data
					     (gtk_container_children
					      (GTK_CONTAINER
					       (imagePosBut[i1][i2])), 0));
			style = gtk_widget_get_style(mainWindow);
			pm = gdk_pixmap_create_from_xpm_d(mainWindow->window,
							  &mask,
							  &style->
							  bg[GTK_STATE_NORMAL],
							  (gchar **)
							  pixmap_imgpos0);
			image = gtk_pixmap_new(pm, mask);
			gtk_widget_show(image);
			gtk_container_add(GTK_CONTAINER(imagePosBut[i1][i2]),
					  image);
		}
	}

	i1 = ImagePos[0] * 10 + ImagePos[1];
	imagePosChanged((GtkWidget *) NULL, GINT_TO_POINTER(i1));

	/* Image Scaling */
	gtk_list_select_item(GTK_LIST(GTK_COMBO(scaleTypeCombo)->list),
			     imageScalingType);

	/* HUE */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(imageHUEAdj), hue);
	/* Saturation */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(imageSatAdj), sat);

}

void imageScalingAdjChanged(GtkWidget * widget, gpointer data)
{
	switch (imageScalingType) {
	case 0:		/* None */

		break;
	case 1:		/* Sheet */
		imageScalingScalingValue =
		    GTK_ADJUSTMENT(imageScaleAllAdj)->value;
		break;
	case 2:		/* Image */
		imageScalingNaturalValue =
		    GTK_ADJUSTMENT(imageScaleAllAdj)->value;
		break;
	case 3:		/* PPI */
		imageScalingPPIValue = GTK_ADJUSTMENT(imageScaleAllAdj)->value;
		break;
	}
}

void imageScalingAllChanged(GtkWidget * widget, gpointer data)
{
	char tmp[MAXLINE + 1];

	snprintf(tmp, (size_t) MAXLINE, "%s",
		 gtk_entry_get_text(GTK_ENTRY
				    (GTK_COMBO(scaleTypeCombo)->entry)));
	if (DEBUG)
		printf("Scaling Combo changed: %s\n", tmp);

	if (strlen(tmp) == 0) {
		if (DEBUG)
			printf
			    ("Ignore empty PopUp-Result in imageScalingAllChanged\n");
		return;
	}

	if (strcmp(tmp, str2str(_("None"))) == 0)
		imageScalingType = 0;
	else if (strcmp(tmp, str2str(_("Sheet relativ"))) == 0)
		imageScalingType = 1;
	else if (strcmp(tmp, str2str(_("Image relativ"))) == 0)
		imageScalingType = 2;
	else if (strcmp(tmp, str2str(_("PPI"))) == 0)
		imageScalingType = 3;
	else {
		if (DEBUG)
			printf("Damned Combo-Box-Error !\n");
		emergency();
	}
	switch (imageScalingType) {
	case 0:		/* None */
		GTK_ADJUSTMENT(imageScaleAllAdj)->value = 0;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), FALSE);
		break;
	case 1:		/* Sheet */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 800;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment =
		    IMAGE_SCALE_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment =
		    IMAGE_SCALE_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;

		GTK_ADJUSTMENT(imageScaleAllAdj)->value =
		    imageScalingScalingValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	case 2:		/* Image */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 800;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment =
		    IMAGE_SCALE_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment =
		    IMAGE_SCALE_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;

		GTK_ADJUSTMENT(imageScaleAllAdj)->value =
		    imageScalingNaturalValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	case 3:		/* PPI */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 1200;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment =
		    IMAGE_PPI_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment =
		    IMAGE_PPI_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;

		GTK_ADJUSTMENT(imageScaleAllAdj)->value = imageScalingPPIValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	}

}

void ImageScalingAllFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *hbox1;
	GList *STList = NULL;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, TRUE, TRUE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	imageScalingAllFrame = gtk_frame_new(str2str(_("Scaling")));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageScalingAllFrame, TRUE, TRUE,
			   0);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(imageScalingAllFrame), vbox);
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	hbox1 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, INFRAME_SPACING_V);
	gtk_widget_show(hbox1);

	/* Type-Combo-Box */
	scaleTypeCombo = gtk_combo_new();
	STList = g_list_append(STList, str2str(_("None")));
	STList = g_list_append(STList, str2str(_("Sheet relativ")));
	STList = g_list_append(STList, str2str(_("Image relativ")));
	STList = g_list_append(STList, str2str(_("PPI")));

	gtk_combo_set_popdown_strings(GTK_COMBO(scaleTypeCombo), STList);
	gtk_combo_set_value_in_list(GTK_COMBO(scaleTypeCombo), TRUE, FALSE);
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(scaleTypeCombo)->list),
				    GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(scaleTypeCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), scaleTypeCombo, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(scaleTypeCombo);

	/* Slider */
	imageScaleAllAdj = gtk_adjustment_new(0, 1, 1, 1, 1, 0);
	imageScaleAllScale = gtk_hscale_new(GTK_ADJUSTMENT(imageScaleAllAdj));
	gtk_scale_set_digits(GTK_SCALE(imageScaleAllScale),
			     IMAGE_SCALE_NATURAL_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageScaleAllScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageScaleAllScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageScaleAllScale),
				    GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageScaleAllScale, TRUE, TRUE,
			   INFRAME_SPACING_V);
	gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), FALSE);
	gtk_widget_show(imageScaleAllScale);

	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(scaleTypeCombo)->entry),
			   "changed", GTK_SIGNAL_FUNC(imageScalingAllChanged),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(imageScaleAllAdj), "value-changed",
			   GTK_SIGNAL_FUNC(imageScalingAdjChanged), NULL);
}

void ImageHUEFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *imageHUEScale, *button1,
	    *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	imageHUEFrame = gtk_frame_new(str2str(_("Color HUE rotation")));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageHUEFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(imageHUEFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	imageHUEAdj = gtk_adjustment_new(0, -360, 360, HUE_STEP, HUE_PAGE, 0);
	imageHUEScale = gtk_hscale_new(GTK_ADJUSTMENT(imageHUEAdj));
	gtk_scale_set_digits(GTK_SCALE(imageHUEScale), HUE_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageHUEScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageHUEScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageHUEScale),
				    GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageHUEScale, TRUE, TRUE,
			   INFRAME_SPACING_H);
	gtk_widget_show(imageHUEScale);

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
			   GTK_SIGNAL_FUNC(resetButton), "HUEDefault");
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "HUE");

}

void ImageSatFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *imageSatScale, *button1,
	    *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	imageSatFrame = gtk_frame_new(str2str(_("Color saturation")));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageSatFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(imageSatFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	imageSatAdj = gtk_adjustment_new(0, 0, 200, SAT_STEP, HUE_PAGE, 0);
	imageSatScale = gtk_hscale_new(GTK_ADJUSTMENT(imageSatAdj));
	gtk_scale_set_digits(GTK_SCALE(imageSatScale), SAT_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageSatScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageSatScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageSatScale),
				    GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageSatScale, TRUE, TRUE,
			   INFRAME_SPACING_H);
	gtk_widget_show(imageSatScale);

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
			   GTK_SIGNAL_FUNC(resetButton), "SatDefault");
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(resetButton), "Sat");

}

void imagePosChanged(GtkWidget * widget, gpointer data)
{
	GtkStyle *style;
	GdkBitmap *mask;
	GdkPixmap *pm;
	GtkWidget *image;

	gtk_container_remove(GTK_CONTAINER
			     (imagePosBut[ImagePos[0]][ImagePos[1]]),
			     g_list_nth_data(gtk_container_children
					     (GTK_CONTAINER
					      (imagePosBut[ImagePos[0]]
					       [ImagePos[1]])), 0));
	style = gtk_widget_get_style(mainWindow);
	pm = gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					  &style->bg[GTK_STATE_NORMAL],
					  (gchar **) pixmap_imgpos0);
	image = gtk_pixmap_new(pm, mask);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(imagePosBut[ImagePos[0]][ImagePos[1]]),
			  image);

	ImagePos[0] = GPOINTER_TO_INT(data) / 10;
	ImagePos[1] = GPOINTER_TO_INT(data) % 10;

	gtk_container_remove(GTK_CONTAINER
			     (imagePosBut[ImagePos[0]][ImagePos[1]]),
			     g_list_nth_data(gtk_container_children
					     (GTK_CONTAINER
					      (imagePosBut[ImagePos[0]]
					       [ImagePos[1]])), 0));
	style = gtk_widget_get_style(mainWindow);
	pm = gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					  &style->bg[GTK_STATE_NORMAL],
					  (gchar **) pixmap_imgpos1);
	image = gtk_pixmap_new(pm, mask);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(imagePosBut[ImagePos[0]][ImagePos[1]]),
			  image);

	if (DEBUG)
		printf("imagePos -> %i %i\n", ImagePos[0], ImagePos[1]);
}

void ImagePosFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox[3], *ihbox, *ivbox, *image;
	int i1, i2, i3;
	GtkStyle *style;
	GdkBitmap *mask;
	GdkPixmap *pm;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, TRUE, TRUE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	imagePosFrame = gtk_frame_new(str2str(_("Position")));
	gtk_box_pack_start(GTK_BOX(mainHBox), imagePosFrame, TRUE, TRUE, 0);

	/* Boxes */
	ivbox = gtk_vbox_new(TRUE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(imagePosFrame), ivbox);
	gtk_widget_show(ivbox);

	ihbox = gtk_hbox_new(TRUE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(ivbox), ihbox, TRUE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(ihbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ihbox), hbox, TRUE, FALSE, 0);
	gtk_widget_show(hbox);

	for (i1 = 0; i1 < 3; i1++) {
		vbox[i1] = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox), vbox[i1], FALSE, FALSE, 0);
		for (i2 = 0; i2 < 3; i2++) {
			imagePosBut[i1][i2] = gtk_button_new();
			gtk_container_set_border_width(GTK_CONTAINER
						       (imagePosBut[i1][i2]),
						       0);
			gtk_box_pack_start(GTK_BOX(vbox[i1]),
					   imagePosBut[i1][i2], FALSE, FALSE,
					   0);
			i3 = i1 * 10 + i2;
			gtk_signal_connect(GTK_OBJECT(imagePosBut[i1][i2]),
					   "clicked",
					   GTK_SIGNAL_FUNC(imagePosChanged),
					   GINT_TO_POINTER(i3));
			style = gtk_widget_get_style(hbox);
			pm = gdk_pixmap_create_from_xpm_d(mainWindow->window,
							  &mask,
							  &style->
							  bg[GTK_STATE_NORMAL],
							  (gchar **)
							  pixmap_imgpos0);
			image = gtk_pixmap_new(pm, mask);
			gtk_container_add(GTK_CONTAINER(imagePosBut[i1][i2]),
					  image);
			gtk_widget_show(image);
			gtk_widget_show(imagePosBut[i1][i2]);

		}
		gtk_widget_show(vbox[i1]);
	}
}

void ImageTab(void)
{
	GtkWidget *hbox;

	imageTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(imageTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), imageTab,
				 gtk_label_new(str2str(_("Image"))));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(imageTab), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	ImagePosFrame(hbox);
	ImageScalingAllFrame(hbox);

	ImageHUEFrame(imageTab);
	ImageSatFrame(imageTab);
}
