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

#include "gtklpq.h"

#include <stdio.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/language.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <X11/Xlib.h>
#include <libgen.h>
#include <locale.h>

#include <gettext.h>
#include <defaults.h>
#include <libgtklp.h>

#include "gtklpq_functions.h"
#include "printer.h"

#include "gtklpq_exit.xpm"
#include "gtklpq_hold.xpm"
#include "gtklpq_move.xpm"
#include "gtklpq_prio.xpm"
#include "gtklpq_release.xpm"
#include "gtklpq_start_printer.xpm"
#include "gtklpq_stop_job.xpm"
#include "gtklpq_stop_jobs.xpm"
#include "gtklpq_stop_printer.xpm"
#include "gtklpq_accept_job.xpm"
#include "gtklpq_reject_job.xpm"

int hasAskedPWD;
int cancelmode = 0;

GtkWidget *tabs, *statbar;
GtkWidget *buttonPrio, *buttonHold, *buttonMove, *buttonCancel,
    *buttonCancelAll, *holdLabel, *buttonHoldPrinter, *holdPrinterLabel,
    *buttonRejectJob;
GtkWidget *holdimage, *printerimage, *rejectjobimage;
GdkPixmap *gtklpq_hold, *gtklpq_release, *gtklpq_stop_printer,
    *gtklpq_start_printer, *gtklpq_reject_job, *gtklpq_accept_job;
GdkBitmap *mgtklpq_hold, *mgtklpq_release, *mgtklpq_stop_printer,
    *mgtklpq_start_printer, *mgtklpq_reject_job, *mgtklpq_accept_job;
GtkStyle *style;
GtkTooltips *tooltips;

GtkWidget *dialog;

int m_id_upd, c_id_upd, m_id_state, c_id_state;

char PrinterNames[MAX_PRT][DEF_PRN_LEN + 1];
char PrinterInfos[MAX_PRT][MAXLINE + 1];
char PrinterLocations[MAX_PRT][MAXLINE + 1];
int PrinterNum;
int PrinterChoice;
char PrinterBannerStart[MAX_PRT][MAXLINE + 1];
char PrinterBannerEnd[MAX_PRT][MAXLINE + 1];

unsigned int selectedJob;

int lastPrinterChoice;

http_t *cupsHttp;
cups_lang_t *cupsLanguage;

int main(int argc, char *argv[])
{
	GtkWidget *mainVBox;
	GtkWidget *buttonBox, *buttonAbbruch;
	GdkPixmap *gtklpq_exit, *gtklpq_move, *gtklpq_prio, *gtklpq_stop_job,
	    *gtklpq_stop_jobs;
	GdkBitmap *mgtklpq_exit, *mgtklpq_move, *mgtklpq_prio,
	    *mgtklpq_stop_job, *mgtklpq_stop_jobs;
	GtkWidget *image;
	int i1;
	char *z1;
	int queryIntervall = 0;
	int xpos, ypos;
	char tmpenv[MAX_SERVER_NAME + 13];
	int forceX;
	int calledGtkLPQ;
	int calledLprm;
	int calledEnable;
	int calledAccept;
	int calledDisable;
	int calledDisableCancel;
	int calledReject;
	struct stat snstat;

#if GTK_MAJOR_VERSION != 1
	progressBar = (GtkWidget *) NULL;
#endif
	dialog = (GtkWidget *) NULL;
	reason[0] = (uintptr_t) NULL;

	hasAskedPWD = 0;
	selectedJob = 0;

	xpos = -1;
	ypos = -1;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
#if GTK_MAJOR_VERSION != 1
	bind_textdomain_codeset(PACKAGE, "UTF-8");
#endif
	textdomain(PACKAGE);

	/*-- Force X ? --*/
	forceX = 0;
	if (getenv("GTKLP_FORCEX") != (char *)NULL) {
		if (strcmp(getenv("GTKLP_FORCEX"), "1") == 0) {
			forceX = 1;
		}
	}

	/*-- Use X ? --*/
	nox = noX();

	/*-- See how we are called */
	calledGtkLPQ = 1;
	calledLprm = 0;
	calledEnable = 0;
	calledAccept = 0;
	calledDisable = 0;
	calledDisableCancel = 0;
	calledReject = 0;
	if (strcmp(basename(argv[0]), "lpq") == 0) {
		calledGtkLPQ = 0;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as LPQ\n");
	}
	if (strcmp(basename(argv[0]), "lprm") == 0) {
		calledGtkLPQ = 0;
		calledLprm = 1;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as LPRM\n");
	}

	if (strcmp(basename(argv[0]), "enable") == 0) {
		calledGtkLPQ = 0;
		calledEnable = 1;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as ENABLE\n");
	}

	if (strcmp(basename(argv[0]), "accept") == 0) {
		calledGtkLPQ = 0;
		calledAccept = 1;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as ACCEPT\n");
	}

	if (strcmp(basename(argv[0]), "disable") == 0) {
		calledGtkLPQ = 0;
		calledDisable = 1;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as DISABLE\n");
	}

	if (strcmp(basename(argv[0]), "reject") == 0) {
		calledGtkLPQ = 0;
		calledDisable = 1;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as REJECT\n");
	}

	if (nox == 0) {
		gtk_init(&argc, &argv);
		setlocale(LC_ALL, "");

#if GTK_MAJOR_VERSION != 1
		/* Default Icons */
		setWindowDefaultIcons();
#endif

		/* HauptFenster */
		mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(mainWindow),
				     str2str(_("GtkLPQ")));
		gtk_widget_set_usize(mainWindow, GTKLPQ_MAINSIZEH,
				     GTKLPQ_MAINSIZEV);
		if ((xpos < 0) || (ypos < 0))
			gtk_window_set_position(GTK_WINDOW(mainWindow),
						GTK_WIN_POS_CENTER);
		else
			gtk_widget_set_uposition(mainWindow, (gint) xpos,
						 (gint) ypos);

		gtk_signal_connect(GTK_OBJECT(mainWindow), "delete_event",
				   GTK_SIGNAL_FUNC(gtklpq_end), NULL);
		gtk_signal_connect(GTK_OBJECT(mainWindow), "destroy",
				   GTK_SIGNAL_FUNC(gtklpq_end), NULL);
		gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 10);
		gtk_widget_show(mainWindow);
	}

	/*--- Get Programm Options ---*/
	DEF_PRN[0] = (uintptr_t) NULL;
	ServerName[0] = (uintptr_t) NULL;
	PortString[0] = (uintptr_t) NULL;
	ServerAddr[0] = (uintptr_t) NULL;
	UserName[0] = (uintptr_t) NULL;
	passWord[0] = (uintptr_t) NULL;
	i1 = getopt(argc, argv, GTKLPQ_ProgOpts);
	while (i1 != -1) {
		switch (i1) {
		case 'r':
			strncpy(reason, optarg, (size_t) MAXLINE);
			break;
		case 'c':
			calledDisableCancel = 1;
			break;
		case 'p':
			if (strlen(optarg) > MAX_PORT) {
				if (DEBUG)
					printf("Port too long !\n");
				exitOnError(str2str(_("Invalid Portnumber !")),
					    "", -1);
			} else {
				if (DEBUG)
					printf("Using port: %s\n", optarg);
				snprintf(PortString, (size_t) MAX_PORT + 10,
					 "IPP_PORT=%s", optarg);
				putenv(PortString);
				snprintf(PortString, (size_t) MAX_PORT + 10,
					 "%s", optarg);
			}
			break;
		case 'P':
		case 'd':
			if (strlen(optarg) < DEF_PRN_LEN) {
				strncpy(DEF_PRN, optarg, (size_t) DEF_PRN_LEN);
			} else {
				if (DEBUG)
					printf
					    ("PROG-ERROR: Printername too long !\n ");
				exitOnError(str2str(_("Invalid Printername !")),
					    "", -1);
			}
			break;
		case 't':
			queryIntervall = atoi(optarg);
			if (queryIntervall < 0)
				queryIntervall = 0;
			if (DEBUG)
				printf("Timeout: %i\n", queryIntervall);
			break;
		case '?':
			if (!calledGtkLPQ)
				break;
		case 'h':
			if (calledGtkLPQ) {
				g_print("%s\n",
					str2str(_
						("Usage: gtklpq [-P|-d Printer] [-S server] [-p port] [-D] [-V] [-U user] [-t timeout] [-g geometry] [-C] [-h] [-E]")));
				return (0);
			}
		case 'S':
			if (optarg == (char *) NULL) {
				exitOnError(str2str
					    (_
					     ("This option requires an value !")),
					    "", -1);
				break;
			}
			if (strlen(optarg) < MAX_SERVER_NAME) {
				strncpy(ServerName, optarg,
					(size_t) MAX_SERVER_NAME);
			} else {
				if (DEBUG)
					printf
					    ("PROG-ERROR: Servername too long !\n ");
				exitOnError(str2str(_("Invalid Servername !")),
					    "", -1);
			}
			break;
		case 'U':
			if (strlen(optarg) < MAX_USERNAME) {
				strncpy(UserName, optarg,
					(size_t) MAX_USERNAME);
			} else {
				if (DEBUG)
					printf
					    ("PROG-ERROR: Username too long !\n ");
				exitOnError(str2str(_("Invalid Username !")),
					    "", -1);
			}
			break;
		case 'D':
			DEBUG = 1;
			printf("DEBUG-MODE on !\n");
			break;
		case 'V':
			g_print("\n%s %s ", PROGNAME, str2str(_("version")));
			g_print("%s\n", VERSION);
			g_print("%s\n\n", MY_HOME);
			return (0);
			break;
		case 'g':
			if (strchr(optarg, 'x') != NULL) {
				z1 = strchr(optarg, 'x');
				*z1 = (uintptr_t) NULL;
				z1++;
				xpos = atoi(optarg);
				ypos = atoi(z1);
			}
			break;
		case 'C':
			nox = 1;
			break;
		case 'X':
			nox = noX();
			break;
		case 'E':
#ifdef HAVE_LIBSSL
			cupsSetEncryption(HTTP_ENCRYPT_REQUIRED);
#else
			g_print("-E: %s\n",
				str2str(_
					("Sorry, this function is not compiled in !")));
#endif
			break;

		default:
			if (DEBUG)
				printf
				    ("PROG-ERROR: Unknown, but allowed switch: %c\n",
				     i1);
			emergency();
			break;
		}
		i1 = getopt(argc, argv, GTKLPQ_ProgOpts);
	}

	/* Try to get connected to Server */
	if (ServerName[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf("No Servername given, searching...\n");
		z1 = (char *)cupsServer();
		if (z1 == (char *)NULL) {
			if (DEBUG)
				printf("PROG-ERROR: No Cups-Server found !\n");
			exitOnError(str2str(_("No Cups-Server found !")), "",
				    -2);
		}
		strncpy(ServerName, z1, (size_t) MAX_SERVER_NAME);
	}
	if (DEBUG)
		printf("ServerName: %s\nTry to connect...\n", ServerName);
	cupsHttp = httpConnect(ServerName, ippPort());
	if (cupsHttp == NULL) {
		if (DEBUG)
			printf("PROG-ERROR: Cannot connect to Server %s!\n",
			       ServerName);
		exitOnError(str2str(_("Unable to connect to Server %s !")),
			    ServerName, -2);
	} else {
		if (DEBUG)
			printf("connected\n");

		if ((stat(ServerName, &snstat) == 0)
		    && S_ISSOCK(snstat.st_mode)) {
			/* Unix socket */
			if (DEBUG)
				printf("Socket found: %s\n", ServerName);
			strncpy(ServerName, "127.0.0.1",
				(size_t) MAX_SERVER_NAME);
		} else {
			if (DEBUG)
				printf("Not a socket: %s\n", ServerName);
		}

		if (PortString[0]) {	/* port specified */
			sprintf(ServerAddr, "%s:%s", ServerName,
				PortString + 9);
		} else {
			strcpy(ServerAddr, ServerName);
		}

		snprintf(tmpenv, (size_t) MAX_SERVER_NAME + 12,
			 "CUPS_SERVER=%s", ServerName);
		putenv(tmpenv);
		cupsSetServer((const char *)ServerName);

	}
	cupsLanguage = cupsLangDefault();

	/* Setting Username */
	cupsSetUser((const char *)UserName);
	if (DEBUG)
		printf("Username is set to: %s\n", cupsUser());

	/* Printer */
	if (DEBUG)
		printf("Default-Printer(1): %s\n", DEF_PRN);

	getPrinters(0);

	if (DEBUG)
		printf("Default-Printer(2): %s\n", DEF_PRN);

	if (DEF_PRN[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf
			    ("PROG-ERROR: No Printer found to use for default !\n");
		exitOnError(str2str(_("No Printer found to use as default !")),
			    "", -2);
	}

	if (nox == 0) {
		/* Vertikaler Container im HauptFenster */
		mainVBox = gtk_vbox_new(FALSE, BUTTON_SPACING_H);
		gtk_container_add(GTK_CONTAINER(mainWindow), mainVBox);
		gtk_widget_show(mainVBox);

		PrinterFrame(mainVBox, 0);
		printerSetDefaults(0);
		gtk_widget_show(printerFrame[0]);

		printQueueInit(mainVBox);
		gtk_widget_show(queueFrame);

		statbar = gtk_statusbar_new();
		gtk_box_pack_end(GTK_BOX(mainVBox), statbar, FALSE, FALSE, 0);
		c_id_upd =
		    gtk_statusbar_get_context_id((GtkStatusbar *) statbar,
						 "Update Queue");
		c_id_state =
		    gtk_statusbar_get_context_id((GtkStatusbar *) statbar,
						 "Printer State");
		gtk_widget_show(statbar);

		buttonBox = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_end(GTK_BOX(mainVBox), buttonBox, FALSE, FALSE, 0);
		gtk_widget_show(buttonBox);

		tooltips = gtk_tooltips_new();
		gtk_tooltips_set_delay(tooltips, TOOLTIP_DELAY);
		gtk_tooltips_enable(tooltips);

		style = gtk_widget_get_style(mainVBox);
		gtklpq_exit =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_exit,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_gtklpq_exit);
		gtklpq_hold =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_hold,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_gtklpq_hold);
		gtklpq_move =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_move,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_gtklpq_move);
		gtklpq_prio =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_prio,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **) pixmap_gtklpq_prio);
		gtklpq_release =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_release,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_release);
		gtklpq_start_printer =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_start_printer,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_start_printer);
		gtklpq_stop_job =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_stop_job,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_stop_job);
		gtklpq_stop_jobs =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_stop_jobs,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_stop_jobs);
		gtklpq_stop_printer =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_stop_printer,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_stop_printer);
		gtklpq_reject_job =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_reject_job,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_reject_job);
		gtklpq_accept_job =
		    gdk_pixmap_create_from_xpm_d(mainWindow->window,
						 &mgtklpq_accept_job,
						 &style->bg[GTK_STATE_NORMAL],
						 (gchar **)
						 pixmap_gtklpq_accept_job);

		image = gtk_pixmap_new(gtklpq_stop_job, mgtklpq_stop_job);
		gtk_widget_show(image);
		buttonCancel = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonCancel), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "CANCEL");
		gtk_container_add(GTK_CONTAINER(buttonCancel), image);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonCancel, FALSE,
				   FALSE, BUTTON_SPACING_V);
		gtk_widget_show(buttonCancel);

		gtk_tooltips_set_tip(tooltips, buttonCancel,
				     str2str(_("Cancel Job")),
				     str2str(_("Cancel Job")));

		image = gtk_pixmap_new(gtklpq_stop_jobs, mgtklpq_stop_jobs);
		gtk_widget_show(image);
		buttonCancelAll = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonCancelAll), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "CANCELALL");
		gtk_container_add(GTK_CONTAINER(buttonCancelAll), image);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonCancelAll, FALSE,
				   FALSE, BUTTON_SPACING_V);
		gtk_widget_show(buttonCancelAll);

		gtk_tooltips_set_tip(tooltips, buttonCancelAll,
				     str2str(_("Cancel All")),
				     str2str(_("Cancel All")));

		holdimage = gtk_pixmap_new(gtklpq_hold, mgtklpq_hold);
		gtk_widget_show(holdimage);
		buttonHold = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonHold), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "HOLD");
		gtk_container_add(GTK_CONTAINER(buttonHold), holdimage);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonHold, FALSE, FALSE,
				   BUTTON_SPACING_V);
		gtk_widget_show(buttonHold);

		gtk_tooltips_set_tip(tooltips, buttonHold,
				     str2str(_("Hold Job")),
				     str2str(_("Hold Job")));

		image = gtk_pixmap_new(gtklpq_move, mgtklpq_move);
		gtk_widget_show(image);
		buttonMove = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonMove), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "MOVE");
		gtk_container_add(GTK_CONTAINER(buttonMove), image);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonMove, FALSE, FALSE,
				   BUTTON_SPACING_V);
		gtk_widget_show(buttonMove);

		gtk_tooltips_set_tip(tooltips, buttonMove,
				     str2str(_("Move Job")),
				     str2str(_("Move Job")));

		rejectjobimage =
		    gtk_pixmap_new(gtklpq_reject_job, mgtklpq_reject_job);
		gtk_widget_show(rejectjobimage);
		buttonRejectJob = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonRejectJob), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "REJECTJOB");
		gtk_container_add(GTK_CONTAINER(buttonRejectJob),
				  rejectjobimage);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonRejectJob, FALSE,
				   FALSE, BUTTON_SPACING_V);
		gtk_widget_show(buttonRejectJob);

		gtk_tooltips_set_tip(tooltips, buttonRejectJob,
				     str2str(_("Reject Jobs")),
				     str2str(_("Reject Jobs")));

		printerimage =
		    gtk_pixmap_new(gtklpq_stop_printer, mgtklpq_stop_printer);
		gtk_widget_show(printerimage);
		buttonHoldPrinter = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonHoldPrinter), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "HOLDPRINTER");
		gtk_container_add(GTK_CONTAINER(buttonHoldPrinter),
				  printerimage);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonHoldPrinter, FALSE,
				   FALSE, BUTTON_SPACING_V);
		gtk_widget_show(buttonHoldPrinter);

		gtk_tooltips_set_tip(tooltips, buttonHoldPrinter,
				     str2str(_("Stop Printer")),
				     str2str(_("Stop Printer")));

		image = gtk_pixmap_new(gtklpq_prio, mgtklpq_prio);
		gtk_widget_show(image);
		buttonPrio = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonPrio), "clicked",
				   GTK_SIGNAL_FUNC(queueFunc), "PRIORITY");
		gtk_container_add(GTK_CONTAINER(buttonPrio), image);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonPrio, FALSE, FALSE,
				   BUTTON_SPACING_V);
		gtk_widget_show(buttonPrio);

		gtk_tooltips_set_tip(tooltips, buttonPrio,
				     str2str(_("Priority")),
				     str2str(_("Priority")));

		gtk_widget_set_sensitive((GtkWidget *) buttonHold, FALSE);
		gtk_widget_set_sensitive((GtkWidget *) buttonMove, FALSE);
		gtk_widget_set_sensitive((GtkWidget *) buttonCancel, FALSE);
		gtk_widget_set_sensitive((GtkWidget *) buttonCancelAll, FALSE);
		gtk_widget_set_sensitive((GtkWidget *) buttonHoldPrinter, TRUE);
		gtk_widget_set_sensitive((GtkWidget *) buttonPrio, TRUE);

		image = gtk_pixmap_new(gtklpq_exit, mgtklpq_exit);
		gtk_widget_show(image);
		buttonAbbruch = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(buttonAbbruch), "clicked",
				   GTK_SIGNAL_FUNC(gtklpq_end), NULL);
		gtk_container_add(GTK_CONTAINER(buttonAbbruch), image);
		gtk_box_pack_end(GTK_BOX(buttonBox), buttonAbbruch, FALSE,
				 FALSE, BUTTON_SPACING_V);
		gtk_widget_show(buttonAbbruch);

		gtk_tooltips_set_tip(tooltips, buttonAbbruch,
				     str2str(_("Exit")), str2str(_("Exit")));

		cupsSetPasswordCB(getPass);

		gtk_widget_grab_focus(buttonAbbruch);
	}

	if (nox == 0) {
		calledGtkLPQ = 1;
		calledLprm = 0;
		calledEnable = 0;
		calledAccept = 0;
		calledDisable = 0;
		calledReject = 0;
	}

	if ((calledEnable == 1) || (calledAccept == 1) || (calledDisable == 1)
	    || (calledReject == 1)) {
		if (calledEnable)
			cancelmode = 6;
		if (calledAccept)
			cancelmode = 9;
		if (calledDisable)
			cancelmode = 5;
		if (calledReject)
			cancelmode = 8;

		selectedJob = 0;

		if (optind < argc) {
			while (optind < argc) {
				i1 = 0;
				while (PrinterNames[i1][0] != (uintptr_t) NULL) {
					if (strcmp
					    (PrinterNames[i1],
					     argv[optind]) == 0) {
						PrinterChoice = i1;
						if ((calledDisable)
						    && (calledDisableCancel)) {
							cancelmode = 1;
							cancelJob((GtkWidget *)
								  NULL);
							cancelmode = 5;
						}
						cancelJob((GtkWidget *) NULL);
					}
					i1++;
				}
				optind++;
			}
		} else {
			cancelJob((GtkWidget *) NULL);
		}
		return (0);
	}

	if (calledLprm == 1) {

		if (optind < argc) {
			if (argv[optind][0] == '-') {
				cancelmode = 1;	/* ALL */
				selectedJob = 0;
				cancelJob((GtkWidget *) NULL);
			} else {
				cancelmode = 0;
				while (optind < argc) {
					selectedJob = atoi(argv[optind++]);
					if (selectedJob > 0) {
						cancelJob((GtkWidget *) NULL);
					}
				}
			}
		} else {
			cancelmode = 0;
			selectedJob = 0;
			cancelJob((GtkWidget *) NULL);
		}
	} else {
		if (nox == 1) {
			if (queryIntervall != 0) {
				while (1) {
					printQueueUpdate(NULL, NULL);
					g_print("\n");
					sleep(queryIntervall);
				}
			}
		}
		printQueueUpdate(NULL, NULL);
	}
	if (nox == 0) {
		if (queryIntervall == 0)
			queryIntervall = QUERY_INTERVALL;
		gtk_timeout_add(queryIntervall * 1000,
				(GtkFunction) printQueueUpdate, NULL);
		gtk_main();
	}
	return (0);
}
