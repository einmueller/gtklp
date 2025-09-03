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

#include "gtklptab.h"

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include <gettext.h>
#include <defaults.h>

#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"
#include "hpgl2.h"
#include "image.h"
#include "output.h"
#include "ppd.h"
#include "special.h"
#include "text.h"

#include "logo_k.xpm"

GtkWidget *gtklpTab;
GtkWidget *gtklpViewable, *gtklpViewOut, *gtklpViewText, *gtklpViewImage,
    *gtklpViewHpgl2, *gtklpViewSpecial, *gtklpViewPPD;
GtkWidget *prefsFrame, *remPrinterCheck, *remTabCheck, *saveExitCheck,
    *jobIDOnExitCheck, *wantconstCheck, *saveAnzCheck;
#if GTK_MAJOR_VERSION != 1
GtkWidget *saveSizePosCheck;
#endif
GtkWidget *pathesFrame;
GtkWidget *helpFrame;
GtkWidget *aboutWin;
GtkWidget *clearFileListCheck, *exitOnPrintCheck;
GtkWidget *browserCom, *helpURL, *gtklpqCom;
GtkWidget *buttonCons;

void gtklpTabSetDefaults(void)
{
	if (DEBUG)
		printf("Get gtklpDefaults\n");

	/* Viewable sections */
	if (viewable[0])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewOut),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewOut),
					     0);
	if (viewable[1])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewText),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewText),
					     0);

	if (viewable[2])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewImage),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewImage),
					     0);

	if (viewable[3])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewHpgl2),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewHpgl2),
					     0);

	if (viewable[4])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtklpViewSpecial), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtklpViewSpecial), 0);

	if (viewable[5])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewPPD),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewPPD),
					     0);
	if (printerPPD == NULL) {
		gtk_widget_set_sensitive(gtklpViewPPD, FALSE);
		gtk_widget_hide(ppdTab);
		gtk_widget_set_sensitive(buttonCons, FALSE);
		gtk_widget_set_sensitive(wantconstCheck, FALSE);
		wantconst = 0;
	} else {
		gtk_widget_set_sensitive(gtklpViewPPD, TRUE);
		gtk_widget_set_sensitive(buttonCons, TRUE);
		gtk_widget_set_sensitive(wantconstCheck, TRUE);
		gtk_widget_show(ppdTab);
	}

	/* Prefs */
	if (saveanz)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveAnzCheck),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveAnzCheck),
					     0);
	if (saveonexit)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveExitCheck),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveExitCheck),
					     0);
	if (rememberprinter)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remPrinterCheck),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remPrinterCheck),
					     0);
	if (remembertab)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remTabCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remTabCheck), 0);
	if (clearfilelist)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (clearFileListCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (clearFileListCheck), 0);
	if (exitonprint)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (exitOnPrintCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (exitOnPrintCheck), 0);
	if (jobidonexit)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (jobIDOnExitCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (jobIDOnExitCheck), 0);
	if (wantconst)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wantconstCheck),
					     1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wantconstCheck),
					     0);
#if GTK_MAJOR_VERSION != 1
	if (wantSaveSizePos)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (saveSizePosCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (saveSizePosCheck), 0);
#endif

	if (needFileSelection == 0) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (exitOnPrintCheck), 1);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (clearFileListCheck), 1);
		gtk_widget_set_sensitive(GTK_WIDGET(clearFileListCheck), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(exitOnPrintCheck), FALSE);
	}

	if (saveonexit == 0) {
		gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck), FALSE);
	}

	/* Paths */
	gtk_entry_set_text(GTK_ENTRY(browserCom), str2str(BROWSER));
	gtk_entry_set_text(GTK_ENTRY(helpURL), str2str(HELPURL));
	gtk_entry_set_text(GTK_ENTRY(gtklpqCom), str2str(GTKLPQCOM));
}

void gtklpViewChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("Gtklp view Changed: %s\n", (char *)data);
	if (strcmp((char *)data, "Out") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[0] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(outputTab);
			gtk_widget_show(tabs);

		} else {
			viewable[0] = 0;
			gtk_widget_hide(outputTab);
		}
		return;
	}
	if (strcmp((char *)data, "Text") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[1] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(textTab);
			gtk_widget_show(tabs);
		} else {
			viewable[1] = 0;
			gtk_widget_hide(textTab);
		}
		return;
	}
	if (strcmp((char *)data, "Image") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[2] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(imageTab);
			gtk_widget_show(tabs);
		} else {
			viewable[2] = 0;
			gtk_widget_hide(imageTab);
		}
		return;
	}
	if (strcmp((char *)data, "Hpgl2") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[3] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(hpgl2Tab);
			gtk_widget_show(tabs);
		} else {
			viewable[3] = 0;
			gtk_widget_hide(hpgl2Tab);
		}
		return;
	}
	if (strcmp((char *)data, "Special") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[4] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(specialTab);
			gtk_widget_show(tabs);
		} else {
			viewable[4] = 0;
			gtk_widget_hide(specialTab);
		}
		return;
	}
	if (strcmp((char *)data, "PPD") == 0) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			viewable[5] = 1;
			gtk_widget_hide(tabs);
			gtk_widget_show(ppdTab);
			gtk_widget_show(tabs);
		} else {
			viewable[5] = 0;
			gtk_widget_hide(ppdTab);
		}
		return;
	}

	if (DEBUG)
		printf("Unknown View-Change !\n");
	emergency();
}

void GtklpViewable(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	gtklpViewable = gtk_frame_new(str2str(_("Look")));
	gtk_box_pack_start(GTK_BOX(mainHBox), gtklpViewable, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(gtklpViewable), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	/* Buttons */
	gtklpViewOut =
	    gtk_check_button_new_with_label(str2str(_("Show Output")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewOut, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewOut), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "Out");
	gtk_widget_show(gtklpViewOut);

	gtklpViewText =
	    gtk_check_button_new_with_label(str2str(_("Show Text")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewText, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewText), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "Text");
	gtk_widget_show(gtklpViewText);

	gtklpViewImage =
	    gtk_check_button_new_with_label(str2str(_("Show Image")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewImage, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewImage), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "Image");
	gtk_widget_show(gtklpViewImage);

	gtklpViewHpgl2 =
	    gtk_check_button_new_with_label(str2str(_("Show HP-GL/2")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewHpgl2, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewHpgl2), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "Hpgl2");
	gtk_widget_show(gtklpViewHpgl2);

	gtklpViewSpecial =
	    gtk_check_button_new_with_label(str2str(_("Show Special")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewSpecial, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewSpecial), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "Special");
	gtk_widget_show(gtklpViewSpecial);

	gtklpViewPPD = gtk_check_button_new_with_label(str2str(_("Show PPD")));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewPPD, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(gtklpViewPPD), "toggled",
			   GTK_SIGNAL_FUNC(gtklpViewChanged), "PPD");
	gtk_widget_show(gtklpViewPPD);

}

void prefsChanged(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("prefsChanged: %s\n", (char *)data);
	if (strcmp((char *)data, "Rem") == 0) {
		if (rememberprinter == 0)
			rememberprinter = 1;
		else
			rememberprinter = 0;
		return;
	}
	if (strcmp((char *)data, "Tab") == 0) {
		if (remembertab == 0)
			remembertab = 1;
		else
			remembertab = 0;
		return;
	}
	if (strcmp((char *)data, "Save") == 0) {
		if (saveonexit == 0) {
			saveonexit = 1;
			gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck),
						 TRUE);
		} else {
			saveonexit = 0;
			gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck),
						 FALSE);
		}
		return;
	}

	if (strcmp((char *)data, "CFL") == 0) {
		if (clearfilelist == 0)
			clearfilelist = 1;
		else
			clearfilelist = 0;
		return;
	}

	if (strcmp((char *)data, "EOP") == 0) {
		if (exitonprint == 0)
			exitonprint = 1;
		else
			exitonprint = 0;
		return;
	}

	if (strcmp((char *)data, "JOE") == 0) {
		if (jobidonexit == 0)
			jobidonexit = 1;
		else
			jobidonexit = 0;
		return;
	}

	if (strcmp((char *)data, "CONS") == 0) {
		if (wantconst == 0)
			wantconst = 1;
		else
			wantconst = 0;
		return;
	}

	if (strcmp((char *)data, "Anz") == 0) {
		if (saveanz == 0)
			saveanz = 1;
		else
			saveanz = 0;
		return;
	}
#if GTK_MAJOR_VERSION != 1
	if (strcmp((char *)data, "SSP") == 0) {
		if (wantSaveSizePos == 0)
			wantSaveSizePos = 1;
		else
			wantSaveSizePos = 0;

		return;
	}
#endif

	if (DEBUG)
		printf("PROG-ERROR: Unknown prefsChange !\n");
	emergency();
}

void constButton(GtkWidget * widget, gpointer data)
{
	if (wantconst == 0) {
		wantconst = 1;
		constraints(ppdConflicts(printerPPD));
		wantconst = 0;
	} else {
		constraints(ppdConflicts(printerPPD));
	}
}

void PrefsFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *constbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	prefsFrame = gtk_frame_new(str2str(_("Feel")));
	gtk_box_pack_start(GTK_BOX(mainHBox), prefsFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(prefsFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	/* Buttons */
	remPrinterCheck =
	    gtk_check_button_new_with_label(str2str
					    (_("Remember last printer")));
	gtk_box_pack_start(GTK_BOX(vbox), remPrinterCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(remPrinterCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "Rem");
	gtk_widget_show(remPrinterCheck);

	remTabCheck =
	    gtk_check_button_new_with_label(str2str(_("Remember last Tab")));
	gtk_box_pack_start(GTK_BOX(vbox), remTabCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(remTabCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "Tab");
	gtk_widget_show(remTabCheck);

	saveExitCheck =
	    gtk_check_button_new_with_label(str2str
					    (_
					     ("Save all preferences on exit")));
	gtk_box_pack_start(GTK_BOX(vbox), saveExitCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(saveExitCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "Save");
	gtk_widget_show(saveExitCheck);

	saveAnzCheck =
	    gtk_check_button_new_with_label(str2str
					    (_
					     ("Save number of copies on exit")));
	gtk_box_pack_start(GTK_BOX(vbox), saveAnzCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(saveAnzCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "Anz");
	gtk_widget_show(saveAnzCheck);

	clearFileListCheck =
	    gtk_check_button_new_with_label(str2str
					    (_("Clear file list on print")));
	gtk_box_pack_start(GTK_BOX(vbox), clearFileListCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(clearFileListCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "CFL");
	gtk_widget_show(clearFileListCheck);

	exitOnPrintCheck =
	    gtk_check_button_new_with_label(str2str(_("Exit on print")));
	gtk_box_pack_start(GTK_BOX(vbox), exitOnPrintCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(exitOnPrintCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "EOP");
	gtk_widget_show(exitOnPrintCheck);

	jobIDOnExitCheck =
	    gtk_check_button_new_with_label(str2str(_("Show JobIDs")));
	gtk_box_pack_start(GTK_BOX(vbox), jobIDOnExitCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(jobIDOnExitCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "JOE");
	gtk_widget_show(jobIDOnExitCheck);

	constbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), constbox, TRUE, TRUE, 0);
	gtk_widget_show(constbox);

	wantconstCheck =
	    gtk_check_button_new_with_label(str2str
					    (_("Enable Constraints check")));
	gtk_box_pack_start(GTK_BOX(constbox), wantconstCheck, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(wantconstCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "CONS");
	gtk_widget_show(wantconstCheck);

	buttonCons = gtk_button_new_with_label(str2str(_("Check now")));
	button_pad(buttonCons);
	gtk_box_pack_end(GTK_BOX(constbox), buttonCons, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(buttonCons), "pressed",
			   GTK_SIGNAL_FUNC(constButton), NULL);
	gtk_widget_show(buttonCons);

#if GTK_MAJOR_VERSION != 1
	saveSizePosCheck =
	    gtk_check_button_new_with_label(str2str
					    (_("Remember size and position")));
	gtk_box_pack_start(GTK_BOX(vbox), saveSizePosCheck, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(saveSizePosCheck), "pressed",
			   GTK_SIGNAL_FUNC(prefsChanged), "SSP");
	gtk_widget_show(saveSizePosCheck);
#endif

}

void visitIt(char *what)
{
	char tmp[2 * MAXPATH + 1];
	char tmpcmd[MAXPATH + 1];
	char *z1;
	int ret;

	strncpy(tmpcmd, BROWSER, (size_t) (MAXPATH));

	z1 = strstr(tmpcmd, "$0");

	if (z1 != NULL) {
		*z1 = '%';
		z1++;
		*z1 = 's';
		snprintf(tmp, (size_t) (2 * MAXPATH), tmpcmd, what);
	} else {
		snprintf(tmp, (size_t) (2 * MAXPATH), "%s %s", tmpcmd, what);
	}

	if (DEBUG)
		printf("Command to execute: %s\n", tmp);

	ret = system(tmp);
	if (ret != 0) {
		if (DEBUG)
			printf("PROG-ERROR: Cannot call browser !\n");
	}
}

void visitMyHome(GtkWidget * widget, gpointer data)
{
	visitIt(MY_HOME);
}

void visitCupsHome(GtkWidget * widget, gpointer data)
{
	visitIt(CUPS_HOME);
}

void destroyAbout(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("About-Window destroyed !\n");
	gtk_window_set_modal(GTK_WINDOW(aboutWin), FALSE);
	gtk_window_set_modal(GTK_WINDOW(mainWindow), TRUE);

	gtk_widget_destroy(aboutWin);
}

void showAbout(GtkWidget * widget, gpointer data)
{
#if GTK_MAJOR_VERSION == 1
	GtkWidget *mainVBox, *sep3, *hbox8;
#endif
	GtkWidget *mainHBox, *vbox, *pixmapwid;
	GtkWidget *label1, *label2, *label3, *label4, *label5, *label6, *label7,
	    *label8, *labelBox1, *labelBox3, *sep1;
	GtkWidget *hbox3, *hbox5, *hbox6;
	GtkWidget *button1, *button2, *button3;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;
	gchar *version_name;

	if (DEBUG)
		printf("Showing About-Dialog\n");
#if GTK_MAJOR_VERSION == 1
	aboutWin = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_position(GTK_WINDOW(aboutWin), GTK_WIN_POS_CENTER);
	gtk_window_set_policy(GTK_WINDOW(aboutWin), 0, 0, 0);
	gtk_window_set_modal(GTK_WINDOW(aboutWin), TRUE);
	gtk_window_set_modal(GTK_WINDOW(mainWindow), FALSE);
	gtk_signal_connect(GTK_OBJECT(aboutWin), "delete_event",
			   GTK_SIGNAL_FUNC(destroyAbout), NULL);
#else
	aboutWin =
	    gtk_dialog_new_with_buttons(str2str(PROGNAME),
					GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
	button3 = gtk_dialog_add_button(GTK_DIALOG(aboutWin), GTK_STOCK_OK, 0);
#endif

#if GTK_MAJOR_VERSION == 1
	mainVBox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(aboutWin), mainVBox);
	gtk_widget_show(mainVBox);

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mainVBox), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
#else
	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(aboutWin)->vbox), mainHBox);
	gtk_widget_show(mainHBox);
#endif

	/* Box */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(mainHBox), vbox, FALSE, FALSE,
			 FRAME_SPACING_H);
	gtk_widget_show(vbox);

	/* Pixmap */
	style = gtk_widget_get_style(aboutWin);
	pixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) logo_k);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_box_pack_start(GTK_BOX(mainHBox), pixmapwid, FALSE, FALSE,
			   MESSAGE_SPACING_H);
	gtk_widget_show(pixmapwid);

	/* Titel */
	labelBox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), labelBox1, TRUE, TRUE, 0);
	gtk_widget_show(labelBox1);
	label1 = gtk_label_new(PROGNAME);
	gtk_box_pack_start(GTK_BOX(labelBox1), label1, TRUE, TRUE, 0);
	gtk_widget_show(label1);

	version_name = g_strdup_printf("%s %s", str2str(_("version")), VERSION);
	label2 = gtk_label_new(version_name);
	g_free(version_name);
	gtk_box_pack_start(GTK_BOX(labelBox1), label2, TRUE, TRUE, 0);
	gtk_widget_show(label2);

	/* ruler */
	sep1 = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep1, FALSE, FALSE, 0);
	gtk_widget_show(sep1);

	/* from */
	label3 = gtk_label_new(str2str(_("written by")));
	gtk_box_pack_start(GTK_BOX(vbox), label3, FALSE, FALSE, 0);
	gtk_widget_show(label3);
	labelBox3 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), labelBox3, FALSE, FALSE, 0);
	gtk_widget_show(labelBox3);
	label4 = gtk_label_new(MY_NAME);
	gtk_box_pack_start(GTK_BOX(vbox), label4, FALSE, FALSE, 0);
	gtk_widget_show(label4);

	label5 = gtk_label_new(MY_MAIL);
	gtk_box_pack_start(GTK_BOX(vbox), label5, FALSE, FALSE, 0);
	gtk_widget_show(label5);
	hbox3 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	gtk_widget_show(hbox3);
	hbox6 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox6, FALSE, FALSE, 0);
	gtk_widget_show(hbox6);
	button1 = gtk_button_new_with_label(str2str(MY_HOME));
	button_pad(button1);
	gtk_box_pack_start(GTK_BOX(hbox6), button1, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button1), "pressed",
			   GTK_SIGNAL_FUNC(visitMyHome), NULL);
	gtk_widget_show(button1);

	/* for */
	label7 = gtk_label_new(str2str(_("written for cups")));
	gtk_box_pack_start(GTK_BOX(vbox), label7, FALSE, FALSE, 0);
	gtk_widget_show(label7);
	hbox5 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox5, FALSE, FALSE, 0);
	gtk_widget_show(hbox5);

	button2 = gtk_button_new_with_label(str2str(CUPS_HOME));
	button_pad(button2);
	gtk_box_pack_start(GTK_BOX(vbox), button2, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(visitCupsHome), NULL);
	gtk_widget_show(button2);

	label6 = gtk_label_new(str2str(ABOUT_CRIGHT));
	gtk_box_pack_start(GTK_BOX(vbox), label6, FALSE, FALSE, 0);
	gtk_widget_show(label6);

	label8 = gtk_label_new(str2str(ABOUT_LICENSE));
	gtk_box_pack_start(GTK_BOX(vbox), label8, FALSE, FALSE, 0);
	gtk_widget_show(label8);

	/* ruler */
#if GTK_MAJOR_VERSION == 1
	sep3 = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep3, FALSE, FALSE, 0);
	gtk_widget_show(sep3);
#endif

	/* OK */
#if GTK_MAJOR_VERSION == 1
	hbox8 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox8, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(hbox8);

	button3 = gtk_button_new_with_label(str2str(_("Ok")));
	button_pad(button3);
	gtk_box_pack_start(GTK_BOX(hbox8), button3, TRUE, TRUE,
			   INFRAME_SPACING_H);
	gtk_signal_connect(GTK_OBJECT(button3), "clicked",
			   GTK_SIGNAL_FUNC(destroyAbout), NULL);
	gtk_widget_show(button3);

	gtk_widget_grab_focus(GTK_WIDGET(button3));

	gtk_widget_show(aboutWin);
#else
	gtk_widget_grab_focus(GTK_WIDGET(button3));
	gtk_dialog_set_default_response(GTK_DIALOG(aboutWin), 0);
	gtk_dialog_run(GTK_DIALOG(aboutWin));
	gtk_widget_destroy(GTK_WIDGET(aboutWin));
#endif

}

void showHTMLHelp(GtkWidget * widget, gpointer data)
{
	visitIt(HELPURL);
}

void HelpFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *button1, *button2;
	gchar *buf;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	helpFrame = gtk_frame_new(str2str(_("Help")));
	gtk_box_pack_start(GTK_BOX(mainHBox), helpFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(helpFrame), hbox);
	gtk_widget_show(hbox);

	/* Buttons */
	button1 = gtk_button_new_with_label(str2str(_("Help on Options")));
	button_pad(button1);
	gtk_box_pack_start(GTK_BOX(hbox), button1, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button1), "pressed",
			   GTK_SIGNAL_FUNC(showHTMLHelp), NULL);
	gtk_widget_show(button1);

	buf = g_strdup_printf(str2str(_("About %s")), PROGNAME);
	button2 = gtk_button_new_with_label(buf);
	button_pad(button2);
	g_free(buf);
	gtk_box_pack_start(GTK_BOX(hbox), button2, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button2), "pressed",
			   GTK_SIGNAL_FUNC(showAbout), NULL);
	gtk_widget_show(button2);
}

void PathesEntryChanged(GtkWidget * widget, gpointer data)
{
	if (strcmp((char *)data, "BROWSER") == 0) {
		strncpy(BROWSER, (char *)gtk_entry_get_text(GTK_ENTRY(widget)),
			MAXPATH);
	}

	if (strcmp((char *)data, "HELP") == 0) {
		strncpy(HELPURL, (char *)gtk_entry_get_text(GTK_ENTRY(widget)),
			MAXPATH);
	}

	if (strcmp((char *)data, "GTKLPQ") == 0) {
		if (strlen(GTKLPQCOM) > 0)
			gtk_widget_hide(gtklpqFrame);
		strncpy(GTKLPQCOM,
			(char *)gtk_entry_get_text(GTK_ENTRY(widget)), MAXPATH);
		if (strlen(GTKLPQCOM) > 0)
			gtk_widget_show(gtklpqFrame);
	}
}

void PathesFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *label1, *label2, *label3;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	pathesFrame = gtk_frame_new(str2str(_("Commands and Paths")));
	gtk_box_pack_start(GTK_BOX(mainHBox), pathesFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(pathesFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, BUTTON_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
	gtk_widget_show(vbox);

	label1 = gtk_label_new(str2str(_("Browser Command")));
	gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(vbox), label1, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(label1);

	browserCom = gtk_entry_new_with_max_length(MAXPATH);
	gtk_entry_set_editable(GTK_ENTRY(browserCom), TRUE);
	gtk_entry_set_text(GTK_ENTRY(browserCom), str2str(DEF_BROWSER_CMD));
	gtk_box_pack_start(GTK_BOX(vbox), browserCom, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(browserCom);

	label2 = gtk_label_new(str2str(_("Help URL")));
	gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(vbox), label2, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(label2);

	helpURL = gtk_entry_new_with_max_length(MAXPATH);
	gtk_entry_set_editable(GTK_ENTRY(helpURL), TRUE);
	gtk_entry_set_text(GTK_ENTRY(helpURL), str2str(DEF_HELP_HOME));
	gtk_box_pack_start(GTK_BOX(vbox), helpURL, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(helpURL);

	label3 = gtk_label_new(str2str(_("Gtklpq Command")));
	gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(vbox), label3, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(label3);

	gtklpqCom = gtk_entry_new_with_max_length(MAXPATH);
	gtk_entry_set_editable(GTK_ENTRY(gtklpqCom), TRUE);
	gtk_entry_set_text(GTK_ENTRY(gtklpqCom), str2str(DEF_HELP_HOME));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpqCom, TRUE, TRUE,
			   0 * INFRAME_SPACING_H);
	gtk_widget_show(gtklpqCom);

#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect(GTK_OBJECT(browserCom), "changed",
			   PathesEntryChanged, "BROWSER");
	gtk_signal_connect(GTK_OBJECT(helpURL), "changed", PathesEntryChanged,
			   "HELP");
	gtk_signal_connect(GTK_OBJECT(gtklpqCom), "changed", PathesEntryChanged,
			   "GTKLPQ");
#else
	gtk_signal_connect(GTK_OBJECT(browserCom), "changed",
			   GTK_SIGNAL_FUNC(PathesEntryChanged), "BROWSER");
	gtk_signal_connect(GTK_OBJECT(helpURL), "changed",
			   GTK_SIGNAL_FUNC(PathesEntryChanged), "HELP");
	gtk_signal_connect(GTK_OBJECT(gtklpqCom), "changed",
			   GTK_SIGNAL_FUNC(PathesEntryChanged), "GTKLPQ");
#endif

}

GtkWidget *GtklpTabFrame(GtkWidget * container)
{
	GtkWidget *frameScrolled;
	GtkWidget *hbox, *vbox;

	frameScrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frameScrolled),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(container), frameScrolled, TRUE, TRUE, 0);
	gtk_widget_show(frameScrolled);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW
					      (frameScrolled), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(vbox);

	return (vbox);
}

void GtklpTabButtons(GtkWidget * container)
{
	GtkWidget *button, *hbox;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(hbox);

#if GTK_MAJOR_VERSION != 1
	button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
#else
	button = gtk_button_new_with_label(str2str(_("Save")));
#endif
	button_pad(button);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
			   GTK_SIGNAL_FUNC(SaveGtkLPPrefs), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(button);
}

void GtklpTab(void)
{
	GtkWidget *frame;

	gtklpTab = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), gtklpTab,
				 gtk_label_new(PROGNAME));

	frame = GtklpTabFrame(gtklpTab);
	GtklpViewable(frame);
	PrefsFrame(frame);
	PathesFrame(frame);
	HelpFrame(frame);
	GtklpTabButtons(gtklpTab);
}
