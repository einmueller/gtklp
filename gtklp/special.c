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

#include "special.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include <gettext.h>
#include <libgtklp.h>

#include "gtklp.h"

GtkWidget *specialTab;
GtkWidget *bannerFrame, *bannerFrameStartCombo, *bannerFrameStopCombo;
GtkWidget *jobNameFrame, *jobNameField;
GtkWidget *specialOptFrame, *specialRawCheck;
GtkWidget *extraOptFrame, *extraOptField;
GtkWidget *passFrame, *loginEntrySpecial, *passEntrySpecial;
extern void getBannerList(char *);
GList *bannerList;

int passWarn;

int bannerAnz;
char Banners[MAX_BANNER][MAXLINE + 1];

void specialTabSetDefaults(void)
{
	int i1 = 0;

	if (DEBUG)
		printf("Get specialDefaults\n");
	/* banners */
	if (bannerList == NULL)
		getBannerList(DEF_PRN);
	gtk_combo_set_popdown_strings(GTK_COMBO(bannerFrameStartCombo),
				      bannerList);
	gtk_combo_set_popdown_strings(GTK_COMBO(bannerFrameStopCombo),
				      bannerList);
	gtk_list_select_item(GTK_LIST(GTK_COMBO(bannerFrameStartCombo)->list),
			     0);
	gtk_list_select_item(GTK_LIST(GTK_COMBO(bannerFrameStopCombo)->list),
			     0);
	for (i1 = 0; i1 < bannerAnz; i1++) {
		if (strcmp
		    (str2str(Banners[i1]),
		     str2str(PrinterBannerStart[PrinterChoice])) == 0)
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(bannerFrameStartCombo)->
					      list), i1);
		if (strcmp
		    (str2str(Banners[i1]),
		     str2str(PrinterBannerEnd[PrinterChoice])) == 0)
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(bannerFrameStopCombo)->
					      list), i1);
	}
	/* Job Name */
	if (JobName[0] != (uintptr_t) NULL) ;
	gtk_entry_set_text(GTK_ENTRY(jobNameField), JobName);
	/* Options */
	if (wantraw)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(specialRawCheck),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(specialRawCheck),
					     0);
	/* Extra Options */
	if (ExtraOpt[0] != (uintptr_t) NULL) ;
	gtk_entry_set_text(GTK_ENTRY(extraOptField), ExtraOpt);

	/* Password and login */
	gtk_entry_set_text(GTK_ENTRY(loginEntrySpecial),
			   (const gchar *)cupsUser());
	gtk_entry_set_text(GTK_ENTRY(passEntrySpecial), passWord);
	passWarn = 1;
	hasAskedPWD = 0;
}

void BannerFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *vbox, *hbox1, *hbox2, *labelStart, *labelStop;

	bannerList = NULL;
	bannerAnz = 0;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	bannerFrame = gtk_frame_new(str2str(_("Banners")));
	gtk_box_pack_start(GTK_BOX(mainHBox), bannerFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* vbox */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(bannerFrame), vbox);
	gtk_widget_show(vbox);

	/* Start Box */
	hbox1 = gtk_hbox_new(FALSE, 0 * INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(hbox1);

	/* Stop Box */
	hbox2 = gtk_hbox_new(FALSE, 0 * INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_show(hbox2);

	/* Start */
	labelStart = gtk_label_new(str2str(_("Start:")));
	gtk_misc_set_alignment(GTK_MISC(labelStart), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelStart),
				    BANNERLABELFIELD_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox1), labelStart, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(labelStart);

	bannerFrameStartCombo = gtk_combo_new();
	gtk_combo_set_value_in_list(GTK_COMBO(bannerFrameStartCombo), TRUE,
				    FALSE);
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(bannerFrameStartCombo)->list),
				    GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(bannerFrameStartCombo)->entry),
			       FALSE);
	gtk_box_pack_start(GTK_BOX(hbox1), bannerFrameStartCombo, FALSE, FALSE,
			   0);
	gtk_widget_show(bannerFrameStartCombo);

	/* Stop */
	labelStop = gtk_label_new(str2str(_("End:")));
	gtk_misc_set_alignment(GTK_MISC(labelStop), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelStop),
				    BANNERLABELFIELD_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox2), labelStop, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(labelStop);

	bannerFrameStopCombo = gtk_combo_new();
	gtk_combo_set_value_in_list(GTK_COMBO(bannerFrameStopCombo), TRUE,
				    FALSE);
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(bannerFrameStopCombo)->list),
				    GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(bannerFrameStopCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), bannerFrameStopCombo, FALSE, FALSE,
			   0);
	gtk_widget_show(bannerFrameStopCombo);

}

gboolean loginEntrySpecialNew(GtkWidget * widget, GdkEventFocus * event,
			      gpointer user_data)
{
	cupsSetUser((const char *)
		    gtk_entry_get_text(GTK_ENTRY(loginEntrySpecial)));
	strncpy(UserName, (char *)gtk_entry_get_text(GTK_ENTRY(loginEntrySpecial)), (size_t) MAX_USERNAME);	/* no length-check, cause it's done with entrys max-length */
	if (DEBUG)
		printf("New Username: %s\n", cupsUser());

	return (0);
}

gboolean passEntrySpecialNew(GtkEditable * editable, gpointer data)
{
	strncpy(passWord, (char *)gtk_entry_get_text(GTK_ENTRY(passEntrySpecial)), (size_t) MAX_PASSLEN);	/* no length-check, cause it's done with entrys max-length */
	if (DEBUG)
		printf("New Password: %s\n", passWord);
	if ((passWarn == 1) && (passWord[0] != (uintptr_t) NULL)) {
		passWarn = 0;
		quick_message(str2str
			      (_
			       ("You have chosen to store your password on the disk.\nThis is not secure, so I recommend against doing so.\nYou have been warned.")),
			      4);
	}

	return (0);
}

void PassFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *vbox, *hbox1, *hbox2, *labelLogin, *labelPass;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	passFrame = gtk_frame_new(str2str(_("Password")));
	gtk_box_pack_start(GTK_BOX(mainHBox), passFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* vbox */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(passFrame), vbox);
	gtk_widget_show(vbox);

	/* Login Box */
	hbox1 = gtk_hbox_new(FALSE, 0 * INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(hbox1);

	/* Password Box */
	hbox2 = gtk_hbox_new(FALSE, 0 * INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_show(hbox2);

	/* Login */
	labelLogin = gtk_label_new(str2str(_("Login:")));
	gtk_misc_set_alignment(GTK_MISC(labelLogin), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelLogin),
				    LOGINLABELFIELD_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox1), labelLogin, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(labelLogin);

	loginEntrySpecial = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(loginEntrySpecial), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(loginEntrySpecial), MAX_USERNAME);
	gtk_entry_set_visibility(GTK_ENTRY(loginEntrySpecial), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox1), loginEntrySpecial, FALSE, FALSE, 0);
	gtk_widget_show(loginEntrySpecial);

	/* Password */
	labelPass = gtk_label_new(str2str(_("Password:")));
	gtk_misc_set_alignment(GTK_MISC(labelPass), 0, 0.5);
	gtk_widget_set_size_request(GTK_WIDGET(labelPass),
				    LOGINLABELFIELD_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox2), labelPass, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(labelPass);

	passEntrySpecial = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(passEntrySpecial), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(passEntrySpecial), MAX_PASSLEN);
	gtk_entry_set_visibility(GTK_ENTRY(passEntrySpecial), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox2), passEntrySpecial, FALSE, FALSE, 0);
	gtk_widget_show(passEntrySpecial);

	gtk_signal_connect(GTK_OBJECT(loginEntrySpecial), "focus-out-event",
			   GTK_SIGNAL_FUNC(loginEntrySpecialNew), NULL);
	gtk_signal_connect(GTK_OBJECT(passEntrySpecial), "focus-out-event",
			   GTK_SIGNAL_FUNC(passEntrySpecialNew), NULL);
}

void JobNameChanged(GtkWidget * widget, gpointer data)
{
	strncpy(JobName,
		gtk_editable_get_chars(GTK_EDITABLE(jobNameField), 0, -1),
		(size_t) MAX_JOB_NAME_LENGTH);
	if (DEBUG)
		printf("Job Name changed: %s\n", JobName);
}

void JobNameFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	jobNameFrame = gtk_frame_new(str2str(_("Job Name")));
	gtk_box_pack_start(GTK_BOX(mainHBox), jobNameFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* vbox */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(jobNameFrame), vbox);
	gtk_widget_show(vbox);

	/* hbox */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	/* TextBox */
	jobNameField = gtk_entry_new_with_max_length(MAX_JOB_NAME_LENGTH);
	gtk_widget_set_size_request(jobNameField, JOBNAME_FIELD_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY(jobNameField), "");
	gtk_box_pack_start(GTK_BOX(hbox), jobNameField, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(jobNameField);

	/* Signals */
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(jobNameField)), "changed",
			   GTK_SIGNAL_FUNC(JobNameChanged), NULL);
}

void specialCheckChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("specialChanged: %s\n", (char *)data);
	if (strcmp((char *)data, "Raw") == 0) {
		if (wantraw == 1)
			wantraw = 0;
		else
			wantraw = 1;
		return;
	}
	if (DEBUG)
		printf("PROG_ERROR: unknown special button !\n");
	emergency();

}

void SpecialOptFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	specialOptFrame = gtk_frame_new(str2str(_("Options")));
	gtk_box_pack_start(GTK_BOX(mainHBox), specialOptFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(specialOptFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	/* Buttons */
	specialRawCheck =
	    gtk_check_button_new_with_label(str2str(_("Raw output")));
	gtk_box_pack_start(GTK_BOX(vbox), specialRawCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(specialRawCheck), "pressed",
			   GTK_SIGNAL_FUNC(specialCheckChanged), "Raw");
	gtk_widget_show(specialRawCheck);

}

void extraOptChanged(GtkWidget * widget, gpointer data)
{
	strncpy(ExtraOpt,
		gtk_editable_get_chars(GTK_EDITABLE(extraOptField), 0, -1),
		(size_t) MAX_EXTRAOPT);
	if (DEBUG)
		printf("Extra Options changed: %s\n", ExtraOpt);
}

void ExtraOptFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	extraOptFrame = gtk_frame_new(str2str(_("Extra Options")));
	gtk_box_pack_start(GTK_BOX(mainHBox), extraOptFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* vbox */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(extraOptFrame), vbox);
	gtk_widget_show(vbox);

	/* hbox */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	/* TextBox */
	extraOptField = gtk_entry_new_with_max_length(MAX_EXTRAOPT);
	gtk_widget_set_size_request(extraOptField, EXTRAOPT_FIELD_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY(extraOptField), "");
	gtk_box_pack_start(GTK_BOX(hbox), extraOptField, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(extraOptField);

	/* Signals */
	gtk_signal_connect(GTK_OBJECT(GTK_ENTRY(extraOptField)), "changed",
			   GTK_SIGNAL_FUNC(extraOptChanged), NULL);
}

void SpecialTab(void)
{
	specialTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(specialTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), specialTab,
				 gtk_label_new(str2str(_("Special"))));

	BannerFrame(specialTab);
	JobNameFrame(specialTab);
	SpecialOptFrame(specialTab);
	PassFrame(specialTab);
	ExtraOptFrame(specialTab);
}
