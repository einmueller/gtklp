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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <dirent.h>

#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/http.h>
#include <cups/language.h>
#include <X11/Xlib.h>

#include <gettext.h>
#include <defaults.h>

#include "libgtklp.h"

#if GTK_MAJOR_VERSION == 1
#include "info.xpm"
#include "error.xpm"
#include "warn.xpm"
#endif

#include "myfault.xpm"

#if GTK_MAJOR_VERSION != 1
#include "icons/16x16.xpm"
#include "icons/24x24.xpm"
#include "icons/32x32.xpm"
#include "icons/48x48.xpm"
#include "icons/64x64.xpm"
#endif

#define GTKLPACCEPTDIR "accept"
#define GTKLPDENYDIR "deny"

int DEBUG;
int nox;

int PrinterChoice;
int PrinterNum;
int lastPrinterChoice;
int passActive = 0;
char UserName[MAX_USERNAME + 1];
char passWord[MAX_PASSLEN + 1];
char ServerName[MAX_SERVER_NAME + 1];
char PortString[MAX_PORT + 11];
char ServerAddr[MAX_SERVER_NAME + MAX_PORT + 1];
char DEF_PRN[DEF_PRN_LEN + 1];
char PrinterBannerStart[MAX_PRT][MAXLINE + 1];
char PrinterBannerEnd[MAX_PRT][MAXLINE + 1];
char PrinterInfos[MAX_PRT][MAXLINE + 1];
char PrinterLocations[MAX_PRT][MAXLINE + 1];
char PrinterNames[MAX_PRT][DEF_PRN_LEN + 1];
char PrinterFavNames[MAX_PRT][DEF_PRN_LEN + 1];

#if GTK_MAJOR_VERSION != 1
GtkWidget *passwin;
#endif
GtkWidget *mainWindow;
GtkWidget *passEntry;
GtkWidget *loginEntry;

#if GTK_MAJOR_VERSION != 1
GtkWidget *progressBar = (GtkWidget *) NULL;
#endif

cups_lang_t *cupsLanguage;
http_t *cupsHttp;

int maxPassTry = 0;

extern int hasAskedPWD;
extern GtkWidget *dialog;

void quick_message(gchar * message, int which)
{
	GtkWidget *label, *pixmapwid, *vbox, *hbox;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *okay_button;
#endif

	if (nox == 1) {
		g_print("%s\n", message);
		return;
	}
	while (gtk_events_pending())
		gtk_main_iteration();

#if GTK_MAJOR_VERSION == 1
	dialog = gtk_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_activate_default(GTK_WINDOW(dialog));

	gtk_window_set_transient_for(GTK_WINDOW(dialog),
				     GTK_WINDOW(mainWindow));

	okay_button = gtk_button_new_with_label(str2str(_("Ok")));
	gtk_signal_connect_object(GTK_OBJECT(okay_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  (gpointer) dialog);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->action_area),
			  okay_button);

	gtk_widget_grab_focus(GTK_WIDGET(okay_button));
#else
	dialog =
	    gtk_dialog_new_with_buttons("", GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK, 0, NULL);
#endif
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,
			   MESSAGE_SPACING_V);

	switch (which) {
	case 1:		/*--- Info ---*/
		gtk_window_set_title(GTK_WINDOW(dialog), str2str(_("Message")));
#if GTK_MAJOR_VERSION == 1
		style = gtk_widget_get_style(dialog);
		pixmap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_info);
		pixmapwid = gtk_pixmap_new(pixmap, mask);
#else
		pixmapwid =
		    gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO,
					     GTK_ICON_SIZE_DIALOG);
#endif
		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE,
				   MESSAGE_SPACING_H);
		break;
	case 2:		/*--- Error ---*/
		gtk_window_set_title(GTK_WINDOW(dialog), str2str(_("Error")));
#if GTK_MAJOR_VERSION == 1
		style = gtk_widget_get_style(dialog);
		pixmap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_error);
		pixmapwid = gtk_pixmap_new(pixmap, mask);
#else
		pixmapwid =
		    gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR,
					     GTK_ICON_SIZE_DIALOG);
#endif
		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE,
				   MESSAGE_SPACING_H);
		break;
	case 3:    /*--- MyFault ---*/
		gtk_window_set_title(GTK_WINDOW(dialog),
				     str2str(_("Fehlerteufel")));
		style = gtk_widget_get_style(dialog);
		pixmap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_myfault);
		pixmapwid = gtk_pixmap_new(pixmap, mask);
		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE,
				   MESSAGE_SPACING_H);
		break;
	case 4:    /*--- Warning ---*/
		gtk_window_set_title(GTK_WINDOW(dialog),
				     str2str(_("Warning!")));
#if GTK_MAJOR_VERSION == 1
		style = gtk_widget_get_style(dialog);
		pixmap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_warn);
		pixmapwid = gtk_pixmap_new(pixmap, mask);
#else
		pixmapwid =
		    gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING,
					     GTK_ICON_SIZE_DIALOG);
#endif
		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE,
				   MESSAGE_SPACING_H);
		break;
	default:
		if (DEBUG)
			printf("Unknown Quick-message !\n");
		emergency();
		break;
	}

	label = gtk_label_new(message);
	gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, MESSAGE_SPACING_H);

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), vbox);

	gtk_widget_show_all(dialog);
#if GTK_MAJOR_VERSION != 1
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
#endif
}

void checkFavorites(void)
{
	int i1, i2, found;

	/* Check list of favorites, remove no longer existing entries */
	if (DEBUG)
		printf("Checking favorites\n");
	i1 = 0;
	while (PrinterFavNames[i1][0] != (uintptr_t) NULL) {
		i2 = 0;
		found = 0;
		do {
			if (strcmp
			    (str2str(PrinterNames[i2]),
			     str2str(PrinterFavNames[i1])) == 0) {
				found = 1;
				break;
			}
			i2++;
		}
		while (PrinterNames[i2][0] != (uintptr_t) NULL);

		if (!found) {
			/* find last entry and move here */
			unsigned i3;

			if (DEBUG)
				printf("Remove %s from favorites\n",
				       PrinterFavNames[i1]);
			i3 = i1 + 1;
			while (PrinterFavNames[i3][0] != (uintptr_t) NULL)
				i3++;
			i3--;
			if (i3 != i1) {
				strcpy(PrinterFavNames[i1],
				       PrinterFavNames[i3]);
			} else {
				i1++;
			}
			PrinterFavNames[i3][0] = 0;
		} else {
			i1++;
		}
	}
	if (DEBUG) {
		printf("Favorites now:\n");
		i1 = 0;
		while (PrinterFavNames[i1][0] != (uintptr_t) NULL) {
			printf("%u: %s\n", i1, PrinterFavNames[i1]);
			i1++;
		}
	}
}

void getPrinters(int loop)
{
	int i1;
	cups_dest_t *dests;
	ipp_t *request, *response;
	ipp_attribute_t *attr;
	char URI[MAX_URI + 1];
	char tmpprn[DEF_PRN_LEN + 1];
	unsigned droppedPrinters;
	char TMPPATH[MAXPATH + 1];
	DIR *testdir;
	FILE *testfile;
	char allowdenyexists;
	char ad;
#if GTK_MAJOR_VERSION != 1
	double progFrac = 1.0, progress = 1.0;
#endif
	droppedPrinters = 0;
	allowdenyexists = -1;	/* -1=undef, 0=allow dir exists, 1=only deny dir exists, 2=mechanismus deactive */
	ad = 1;			/* 1=printer is allowed, -1=printer is denied */

	PrinterChoice = -1;
	lastPrinterChoice = -1;

	if (DEBUG)
		printf("Maximum number of supported printers: %u\n", MAX_PRT);

	PrinterNum = cupsGetDests(&dests);
	if (PrinterNum > MAX_PRT) {
		if (DEBUG)
			printf("PROG-ERROR: Too many printers: %u !\n",
			       PrinterNum);
/*		exitOnError(str2str(_("You have installed too many printers or templates !")),"",-3); */
		PrinterNum = MAX_PRT;
	}
	PrinterNum--;
	if (DEBUG)
		printf("Number of Printers: %i\n", PrinterNum + 1);

	snprintf(TMPPATH, (size_t) MAXPATH, "%s/%s/", DEF_GTKLPRC,
		 GTKLPACCEPTDIR);
	testdir = opendir(TMPPATH);
	if (testdir != (DIR *) NULL) {	/* system/allow exists */
		allowdenyexists = 0;
		ad = -1;	/* default is deny all, but... */
		closedir(testdir);
	} else {		/* system/allow does not exist */
		snprintf(TMPPATH, (size_t) MAXPATH, "%s/%s/", DEF_GTKLPRC,
			 GTKLPDENYDIR);
		testdir = opendir(TMPPATH);
		if (testdir != (DIR *) NULL) {	/* no system/allow but system/deny */
			allowdenyexists = 1;
			ad = 1;	/* default is allow all, but... */
			closedir(testdir);
		} else {
			allowdenyexists = 2;	/* no system/allow, no system/deny */
			ad = 1;	/* all printers are allowed (ok, stay allowed...) */
		}
	}

#if GTK_MAJOR_VERSION != 1
	progFrac = 0.8 / PrinterNum;
	progress = 0.1;
#endif

	for (i1 = 0; i1 <= PrinterNum; i1++) {
		if (DEBUG) {
			if (dests[i1].instance == (char *)NULL)
				printf("Name: %s - (NULL) (%i)\n",
				       dests[i1].name, dests[i1].is_default);
			else
				printf("Name: %s - %s (%i)\n", dests[i1].name,
				       dests[i1].instance,
				       dests[i1].is_default);
		}
#if GTK_MAJOR_VERSION != 1
		if (progressBar != (GtkWidget *) NULL) {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR
						      (progressBar), progress);
			while (gtk_events_pending())
				gtk_main_iteration();
			progress += progFrac;
		}
#endif
		if (dests[i1].instance == (char *)NULL)
			snprintf(PrinterNames[i1 - droppedPrinters],
				 (size_t) DEF_PRN_LEN, "%s", dests[i1].name);
		else
			snprintf(PrinterNames[i1 - droppedPrinters],
				 (size_t) DEF_PRN_LEN, "%s/%s", dests[i1].name,
				 dests[i1].instance);

		if (allowdenyexists == 0) {
			snprintf(TMPPATH, (size_t) MAXPATH, "%s/%s/%s",
				 DEF_GTKLPRC, GTKLPACCEPTDIR,
				 PrinterNames[i1 - droppedPrinters]);
			testfile = fopen(TMPPATH, "r");
			if (testfile == (FILE *) NULL) {	/* printer is not in system/allow */
				ad = -1;	/* printer is denied */
			} else {	/* printer stays allowed */
				fclose(testfile);
				snprintf(TMPPATH, (size_t) MAXPATH,
					 "%s/%s/%s/%s", getenv("HOME"),
					 GTKLPRC_USER, GTKLPDENYDIR,
					 PrinterNames[i1 - droppedPrinters]);
				testfile = fopen(TMPPATH, "r");
				if (testfile != (FILE *) NULL) {	/* printer denied by user */
					fclose(testfile);
					ad = -1;
				} else {	/* printer allowed */
					ad = 1;
				}
			}
		}

		if (allowdenyexists == 1) {
			snprintf(TMPPATH, (size_t) MAXPATH, "%s/%s/%s",
				 DEF_GTKLPRC, GTKLPDENYDIR,
				 PrinterNames[i1 - droppedPrinters]);
			testfile = fopen(TMPPATH, "r");
			if (testfile != (FILE *) NULL) {	/* printer is denied by system */
				fclose(testfile);
				ad = -1;
			} else {
				snprintf(TMPPATH, (size_t) MAXPATH,
					 "%s/%s/%s/%s", getenv("HOME"),
					 GTKLPRC_USER, GTKLPDENYDIR,
					 PrinterNames[i1 - droppedPrinters]);
				testfile = fopen(TMPPATH, "r");
				if (testfile != (FILE *) NULL) {	/* printer denied by user */
					fclose(testfile);
					ad = -1;
				} else {	/* printer allowed */
					ad = 1;
				}
			}
		}

		if (ad == -1) {	/* printer is denied */
			droppedPrinters++;
		} else {
			if ((dests[i1].is_default == 1) & (DEF_PRN[0] ==
							   (uintptr_t) NULL)) {
				if (DEBUG)
					printf("---->Default-Printer\n");
				if (dests[i1].instance == (char *)NULL) {
					strncpy(DEF_PRN, dests[i1].name,
						(size_t) DEF_PRN_LEN);
				} else {
					snprintf(DEF_PRN, (size_t) DEF_PRN_LEN,
						 "%s/%s", dests[i1].name,
						 dests[i1].instance);
				}
			}
			if (DEF_PRN[0] != (uintptr_t) NULL) {
				if (dests[i1].instance != NULL)
					snprintf(tmpprn, (size_t) DEF_PRN_LEN,
						 "%s/%s", dests[i1].name,
						 dests[i1].instance);
				else
					snprintf(tmpprn, (size_t) DEF_PRN_LEN,
						 "%s", dests[i1].name);
				if (strcmp(DEF_PRN, tmpprn) == 0)
					PrinterChoice = i1 - droppedPrinters;
			}
			snprintf(URI, (size_t) MAX_URI, "ipp://%s/printers/%s",
				 ServerAddr, dests[i1].name);
			request = ippNew();
			ippSetOperation(request, IPP_GET_PRINTER_ATTRIBUTES);
			ippSetRequestId(request, 1);
			ippAddString(request, IPP_TAG_OPERATION,
				     IPP_TAG_CHARSET, "attributes-charset",
				     NULL, cupsLangEncoding(cupsLanguage));
			ippAddString(request, IPP_TAG_OPERATION,
				     IPP_TAG_LANGUAGE,
				     "attributes-natural-language", NULL,
				     cupsLanguage->language);
			ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
				     "printer-uri", NULL, URI);
			if ((response =
			     cupsDoRequest(cupsHttp, request, "/")) == NULL) {
				if (DEBUG)
					printf
					    ("PROG-ERROR: unable for doing cupsDoRequest !\n");
				emergency();
			}
			if ((attr =
			     ippFindAttribute(response, "job-sheets-default",
					      IPP_TAG_ZERO)) != NULL) {
				if (DEBUG)
					printf
					    ("Job-Sheets-Default for %s\n  Start: %s\n  Stop: %s\n",
					     dests[i1].name, ippGetString(attr,
									  0,
									  NULL),
					     ippGetString(attr, 1, NULL));
				strncpy(PrinterBannerStart
					[i1 - droppedPrinters],
					ippGetString(attr, 0, NULL),
					(size_t) MAXLINE);
				strncpy(PrinterBannerEnd[i1 - droppedPrinters],
					ippGetString(attr, 1, NULL),
					(size_t) MAXLINE);

			} else {
				if (DEBUG)
					printf
					    ("No Default Banners found for %s\n",
					     dests[i1].name);
				strncpy(PrinterBannerStart
					[i1 - droppedPrinters], "none",
					(size_t) MAXLINE);
				strncpy(PrinterBannerEnd[i1 - droppedPrinters],
					"none", (size_t) MAXLINE);
			}
			if ((attr =
			     ippFindAttribute(response, "printer-location",
					      IPP_TAG_ZERO)) != NULL) {
				if (DEBUG)
					printf("Location: %s\n",
					       ippGetString(attr, 0, NULL));
				strncpy(PrinterLocations[i1 - droppedPrinters],
					ippGetString(attr, 0, NULL),
					(size_t) MAXLINE);
			} else {
				if (DEBUG)
					printf("Location not found !\n");
			}
			if ((attr =
			     ippFindAttribute(response, "printer-info",
					      IPP_TAG_ZERO)) != NULL) {
				if (DEBUG)
					printf("Info: %s\n",
					       ippGetString(attr, 0, NULL));
				strncpy(PrinterInfos[i1 - droppedPrinters],
					ippGetString(attr, 0, NULL),
					(size_t) MAXLINE);
			} else {
				if (DEBUG)
					printf("Info not found !\n");
			}
			ippDelete(response);
		}

	}

	if (PrinterNum < 0) {
		if (DEBUG)
			printf("PROG-ERROR: No Printers found !\n");
		exitOnError(str2str(_("No printers found !")), "", -2);
	}
	if (DEF_PRN[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf("No Default Printer yet, using %s\n",
			       PrinterNames[0]);
		strncpy(DEF_PRN, PrinterNames[0], (size_t) DEF_PRN_LEN);
		PrinterChoice = 0;
	}
	if (PrinterChoice == -1) {
		if (loop == 1) {	/* Called this function recursively */
			gtklp_fatal_end(str2str
					(_
					 ("The given printer does not exist!")));
		}
		quick_message(str2str(_("The given printer does not exist!")),
			      2);
		DEF_PRN[0] = (uintptr_t) NULL;
		getPrinters(loop + 1);
	}

	/* decrease for hidden printers */
	PrinterNum -= droppedPrinters;

	/* any visible printers left? */
	if (PrinterNum < 0) {
		if (DEBUG)
			printf("PROG-ERROR: No Printers found !\n");
		exitOnError(str2str(_("No printers found !")), "", -2);
	}
	checkFavorites();
}

void gtklp_fatal_end(char *text)
{
	cupsLangFlush();
	cupsLanguage = NULL;
	httpClose(cupsHttp);
	cupsHttp = NULL;
	printf("\n%s\n\n", text);
	exit(-3);
}

void emergency(void)
{
	g_print("===========================\n");
	g_print("An Programm-Error occured !\n");
	g_print("===========================\n");
	g_print("Try it again, if the problem persists,\n");
	g_print
	    ("please ask your Administrator, or if this is you, try DEBUG-Mode !\n");
	g_print("If you have can`t find whats wrong, mail me at\n");
	g_print(MY_MAIL);
	g_print("\nor visit my Homepage:\n");
	g_print(MY_HOME);
	g_print("\n\nHave a nice day, exiting...\n");

	exitOnError(str2str(_("Internal program error !")), "", -3);
}

void passDestroy(void)
{
	passActive = 0;
}

void passNO(GtkWidget * dialog)
{
	gtk_entry_set_text(GTK_ENTRY(passEntry), "");
	gtk_widget_destroy(dialog);
}

void passChanged(GtkEditable * editable, gpointer data)
{
	strncpy(passWord, (char *)gtk_entry_get_text(GTK_ENTRY(passEntry)),
		(size_t) MAX_USERNAME);
}

void loginChanged(GtkEditable * editable, gpointer data)
{
	strncpy(UserName, (char *)gtk_entry_get_text(GTK_ENTRY(loginEntry)),
		(size_t) MAX_PASSLEN);
}

#if GTK_MAJOR_VERSION != 1
void passwin_response_ok(GtkEditable * editable, gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(passwin), 1);
}
#endif

const char *getPass(const char *prompt)
{
	GtkWidget *vbox, *hbox, *hbox1, *vbox1, *vbox2, *labell, *labelp, *yes,
	    *no;
	gint gi1;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *sep, *passwin;
#endif

	hasAskedPWD = 1;

	if (passWord[0] != (uintptr_t) NULL) {
		if (DEBUG)
			printf("Password already set to %s\n", passWord);
		maxPassTry++;
		if (DEBUG)
			printf("%i try by Cups-Server !\n", maxPassTry);
		if (maxPassTry < MAX_PASSTRY) {
			return ((const char *)passWord);
		} else {
			if (DEBUG)
				printf
				    ("Too many Password-requests from Server. Maybe the User entered it wrong !\n");
			maxPassTry = 0;
		}

	}

	while (gtk_events_pending())
		gtk_main_iteration();

#if GTK_MAJOR_VERSION == 1
	passwin = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(passwin),
			     str2str(_("Password verification")));
	gtk_window_set_policy(GTK_WINDOW(passwin), FALSE, FALSE, TRUE);
	gtk_widget_show(passwin);
	gtk_window_set_transient_for(GTK_WINDOW(passwin),
				     GTK_WINDOW(mainWindow));
	gtk_widget_grab_focus(passwin);
	gtk_window_set_position(GTK_WINDOW(passwin), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(passwin), TRUE);
#else
	passwin =
	    gtk_dialog_new_with_buttons(str2str(_("Password verification")),
					GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK, 1, GTK_STOCK_CANCEL, 0,
					NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(passwin), 1);
#endif

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
#if GTK_MAJOR_VERSION == 1
	gtk_container_add(GTK_CONTAINER(passwin), vbox);
#else
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(passwin)->vbox), vbox);
#endif
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(hbox);

	vbox1 = gtk_vbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox1);

	vbox2 = gtk_vbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox2);

	labell = gtk_label_new(str2str(_("Login:")));
	gtk_box_pack_start(GTK_BOX(vbox1), labell, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_show(labell);

	labelp = gtk_label_new(str2str(_("Password:")));
	gtk_box_pack_start(GTK_BOX(vbox1), labelp, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_show(labelp);

	loginEntry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(loginEntry), (char *)cupsUser());
	gtk_entry_set_max_length(GTK_ENTRY(loginEntry), MAX_USERNAME);
	gtk_entry_set_visibility(GTK_ENTRY(loginEntry), TRUE);
	gtk_box_pack_start(GTK_BOX(vbox2), loginEntry, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_show(loginEntry);

	passEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(passEntry), MAX_PASSLEN);
	gtk_entry_set_visibility(GTK_ENTRY(passEntry), FALSE);
	gtk_box_pack_start(GTK_BOX(vbox2), passEntry, FALSE, FALSE,
			   0 * INFRAME_SPACING_V);
	gtk_widget_grab_focus(passEntry);
	gtk_widget_show(passEntry);

#if GTK_MAJOR_VERSION == 1
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);
	gtk_widget_show(sep);
#endif

#if GTK_MAJOR_VERSION == 1
	hbox1 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_set_homogeneous(GTK_BOX(hbox1), TRUE);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, INFRAME_SPACING_V);
	gtk_widget_show(hbox1);

	yes = gtk_button_new_with_label(str2str(_("Ok")));
	gtk_box_pack_start(GTK_BOX(hbox1), yes, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_show(yes);

	no = gtk_button_new_with_label(str2str(_("Cancel")));
	gtk_box_pack_start(GTK_BOX(hbox1), no, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_show(no);

	gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(passwin));
	gtk_signal_connect_object(GTK_OBJECT(no), "clicked",
				  GTK_SIGNAL_FUNC(passNO), GTK_OBJECT(passwin));
#endif

	gtk_signal_connect(GTK_OBJECT(passwin), "destroy",
			   GTK_SIGNAL_FUNC(passDestroy), NULL);
	gtk_signal_connect(GTK_OBJECT(passEntry), "changed",
			   GTK_SIGNAL_FUNC(passChanged), NULL);
	gtk_signal_connect(GTK_OBJECT(loginEntry), "changed",
			   GTK_SIGNAL_FUNC(loginChanged), NULL);

#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect_object(GTK_OBJECT(passEntry), "activate",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(passwin));
#else
	gtk_signal_connect_object(GTK_OBJECT(passEntry), "activate",
				  GTK_SIGNAL_FUNC(passwin_response_ok), NULL);
#endif

	passActive = 1;
#if GTK_MAJOR_VERSION == 1
	while (passActive == 1) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}
#else
	gi1 = gtk_dialog_run(GTK_DIALOG(passwin));

	strncpy(passWord, (char *)gtk_entry_get_text(GTK_ENTRY(passEntry)),
		(size_t) MAX_USERNAME);
	strncpy(UserName, (char *)gtk_entry_get_text(GTK_ENTRY(loginEntry)),
		(size_t) MAX_PASSLEN);

	if (gi1 != 1) {
		passNO(GTK_WIDGET(passwin));
	} else {
		gtk_widget_destroy(passwin);
	}
#endif

	if (DEBUG)
		printf("Passwort set to:%s\n", passWord);

	if (UserName[0] != (uintptr_t) NULL)
		cupsSetUser(UserName);

	return (passWord);

}

int noX(void)
{
	Display *display;
	int ret;

	ret = 0;

	if (getenv("DISPLAY") == NULL) {
		ret = 1;
	} else {
		display = XOpenDisplay(getenv("DISPLAY"));
		if (display == NULL) {
			ret = 1;
		} else {
			XCloseDisplay(display);
		}
	}

	if (DEBUG)
		printf("No-X: %i\n", ret);

	return (ret);
}

int max(int a, int b)
{
	if (a > b)
		return (a);
	else
		return (b);
}

void button_pad(GtkWidget * widget)
{
	GtkWidget *lab = GTK_BIN(widget)->child;
	if (GTK_IS_LABEL(lab))
		gtk_misc_set_padding(GTK_MISC(lab), BUTTON_PAD, 0);
}

/* clean all strings and convert to utf8 */
gchar *str2str(gchar * text)
{
#if GTK_MAJOR_VERSION == 1
	return (text);
#else
	gchar *result;

	if (!text)
		return NULL;

	if (g_utf8_validate(text, -1, NULL)) {
		return g_strdup(text);
	}

	result = g_locale_to_utf8(text, -1, NULL, NULL, NULL);
	if (!result) {
		gchar *p;

		result = p = g_strdup(text);
		while (!g_utf8_validate(p, -1, (const gchar **)&p))
			*p = '?';
	}
	return result;
#endif
}

void exitOnErrorExit(GtkWidget * widget, GtkObject * object)
{
	exit(GPOINTER_TO_INT(object));
}

void exitOnError(char *gerror1, char *gerror2, int exitcode)
{
	GtkWidget *exitErrorDialog;
	GtkWidget *hbox, *label, *pixmapwid;
	char tmplabel[MAXLINE + 1];
	GtkStyle *style;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *button, *vbox, *sep, *bbox;
#endif

	if (nox == 0) {
#if GTK_MAJOR_VERSION == 1
		exitErrorDialog = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_window_set_title(GTK_WINDOW(exitErrorDialog),
				     str2str(_("Error")));
		gtk_window_set_policy(GTK_WINDOW(exitErrorDialog), FALSE, FALSE,
				      TRUE);
		gtk_widget_show(exitErrorDialog);
		gtk_window_set_transient_for(GTK_WINDOW(exitErrorDialog),
					     GTK_WINDOW(mainWindow));
		gtk_window_set_position(GTK_WINDOW(exitErrorDialog),
					GTK_WIN_POS_CENTER);
		gtk_window_set_modal(GTK_WINDOW(exitErrorDialog), TRUE);
		gtk_widget_grab_focus(exitErrorDialog);
		vbox = gtk_vbox_new(0, INFRAME_SPACING_V);
#else
		exitErrorDialog =
		    gtk_dialog_new_with_buttons(str2str(_("Error")),
						GTK_WINDOW(mainWindow),
						GTK_DIALOG_MODAL |
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_OK, 0, NULL);
#endif

		hbox = gtk_hbox_new(0, INFRAME_SPACING_H);

#if GTK_MAJOR_VERSION == 1
		gtk_container_add(GTK_CONTAINER(exitErrorDialog), vbox);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,
				   FRAME_SPACING_V);
		gtk_widget_show(vbox);
#else
		gtk_container_add(GTK_CONTAINER
				  (GTK_DIALOG(exitErrorDialog)->vbox), hbox);
#endif

		gtk_widget_show(hbox);

#if GTK_MAJOR_VERSION == 1
		style = gtk_widget_get_style(exitErrorDialog);
		pixmap =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_error);
		pixmapwid = gtk_pixmap_new(pixmap, mask);
#else
		pixmapwid =
		    gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR,
					     GTK_ICON_SIZE_DIALOG);
#endif

		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE,
				   MESSAGE_SPACING_H);
		gtk_widget_show(pixmapwid);

		if (strlen(gerror2) == 0)
			snprintf(tmplabel, (size_t) MAXLINE, gerror1);
		else
			snprintf(tmplabel, (size_t) MAXLINE, gerror1, gerror2);
		label = gtk_label_new(tmplabel);
		gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE,
				 FRAME_SPACING_V);
		gtk_widget_show(label);

#if GTK_MAJOR_VERSION == 1
		sep = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);
		gtk_widget_show(sep);
		bbox = gtk_hbox_new(1, 0);
		gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE,
				   BUTTON_SPACING_V);
		gtk_widget_show(bbox);
		button = gtk_button_new_with_label(str2str(_("Ok")));
		button_pad(button);
		gtk_box_pack_end(GTK_BOX(bbox), button, FALSE, FALSE, 0);
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(exitOnErrorExit), GINT_TO_POINTER(exitcode));
		gtk_signal_connect(GTK_OBJECT(exitErrorDialog), "destroy",
				   GTK_SIGNAL_FUNC(exitOnErrorExit), GINT_TO_POINTER(exitcode));
		gtk_widget_show(button);
		gtk_widget_show(exitErrorDialog);

		for (;;)
			gtk_main_iteration();

#else
		gtk_dialog_run(GTK_DIALOG(exitErrorDialog));
#endif
	} else {
		if (strlen(gerror2) == 0)
			g_warning(gerror1);
		else
			g_warning(gerror1, gerror2);
	}
	exit(exitcode);
}

#if GTK_MAJOR_VERSION != 1
void setWindowDefaultIcons(void)
{
	GList *iconlist = (GList *) NULL;
	GdkPixbuf *icon;

	icon = gdk_pixbuf_new_from_xpm_data((gchar const **)gtklp_16x16_xpm);
	iconlist = g_list_append(iconlist, icon);
	icon = gdk_pixbuf_new_from_xpm_data((gchar const **)gtklp_24x24_xpm);
	iconlist = g_list_append(iconlist, icon);
	icon = gdk_pixbuf_new_from_xpm_data((gchar const **)gtklp_32x32_xpm);
	iconlist = g_list_append(iconlist, icon);
	icon = gdk_pixbuf_new_from_xpm_data((gchar const **)gtklp_48x48_xpm);
	iconlist = g_list_append(iconlist, icon);
	icon = gdk_pixbuf_new_from_xpm_data((gchar const **)gtklp_64x64_xpm);
	iconlist = g_list_append(iconlist, icon);

	gtk_window_set_default_icon_list(iconlist);

}
#endif

void hello(GtkWidget * widget, gpointer data)
{
	g_print("Hello World\n");
}
