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

#include "gtklp.h"

#include <stdio.h>
#include <cups/cups.h>
#include <cups/language.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <libgtklp.h>

#include <defaults.h>
#include <gettext.h>
#include <locale.h>

#include <libgen.h>

#include "file.h"
#include "general.h"
#include "gtklp_functions.h"
#include "gtklptab.h"
#include "hpgl2.h"
#include "image.h"
#include "output.h"
#include "ppd.h"
#include "special.h"
#include "text.h"

GtkWidget *tabs;
char Range[MAXRANGEENTRY + 1];
char JobName[MAX_JOB_NAME_LENGTH + 1];
char CustomPageSize[2 * MAX_CUSTOM_SIZE_LENGTH + 2];
char ExtraOpt[MAX_EXTRAOPT + 1];
int ignore_stdin;
int clearfilelist, exitonprint;
int saveonexit, rememberprinter, remembertab, tabtoopen;
int viewable[6];
int hpgl2black, hpgl2fit;
unsigned hpgl2pen;
int wantraw;
int needFileSelection;
int noFileSelection;
int ImagePos[2];
int imageScalingType;
double imageScalingScalingValue;
double imageScalingNaturalValue;
double imageScalingPPIValue;
int mirroroutput;
int sheetsType;
int builtin;
int wantRange;
int wantReverseOutputOrder;
int rangeType;
int wantLandscape;
int wantCustomPageSize;
int ReallyPrint;
int NumberOfCopies;
int collate;
int prettyprint;
int imagescaling;
int wantimagescaling;
int imageppi;
int wantimageppi;
int imagenatural;
int wantimagenatural;
double hue;
double sat;
int cmdNumCopies;
int jobidonexit;
int wantconst;
int saveanz;
int oldnumcopies;
double brightness;
double ggamma;
unsigned int cppvalue, lpivalue, cpivalue;
unsigned int topmargin, bottommargin, leftmargin, rightmargin;
int MediaSizeSelected, MediaTypeSelected, MediaSourceSelected;
char filesToPrint[MAXPRINTFILES][MAXPATH + 1];
char filesToPrintAnz;
char fileOpenPath[MAXPATH + 1];
char confdir[MAXPATH + 1];
char GTKLPRC[MAXPATH + 1], HELP_HOME[MAXPATH + 1];
char BROWSER[MAXPATH + 1];
char HELPURL[MAXPATH + 1];
char GTKLPQCOM[MAXPATH + 1];
int textwrap;
int hasAskedPWD;
ppd_file_t *printerPPD;
int PPDopen;
char *PPDfilename;
int conflict_active;
int nox;
int num_commandline_opts;
cups_option_t *commandline_opts;
char nupLayout[5];
char borderType[13];
#if GTK_MAJOR_VERSION != 1
int mainWindowHeight, mainWindowWidth, mainWindowX, mainWindowY,
    wantSaveSizePos;
#endif
GtkWidget *dialog;

int main(int argc, char *argv[])
{
	GtkWidget *mainVBox = NULL, *waitLabel = NULL;
	GtkWidget *buttonBox, *buttonPrint, *buttonAbbruch, *buttonReset,
	    *buttonSave;
	int i1;
	int calledGtkLP;
	int calledLpr;
	int calledLp;
	int forceX;
	char *z1;
	char tmpenv[MAX_SERVER_NAME + 13];
	struct stat snstat;

#if GTK_MAJOR_VERSION != 1
	progressBar = (GtkWidget *) NULL;

	mainWindowHeight = 0;
	mainWindowWidth = 0;
	mainWindowX = 0;
	mainWindowY = 0;
	wantSaveSizePos = 0;
#endif

	dialog = (GtkWidget *) NULL;

	JobName[0] = (uintptr_t) NULL;

	ignore_stdin = 0;
	printerPPD = NULL;
	PPDopen = 0;
	PPDfilename = NULL;

	conflict_active = 0;

	BROWSER[0] = (uintptr_t) NULL;
	HELPURL[0] = (uintptr_t) NULL;
	GTKLPQCOM[0] = (uintptr_t) NULL;

	nupLayout[0] = (uintptr_t) NULL;
	borderType[0] = (uintptr_t) NULL;

	textwrap = 0;

	nox = noX();

	num_commandline_opts = 0;
	commandline_opts = NULL;

	/*-- Force X ? --*/
	forceX = 0;
	if (getenv("GTKLP_FORCEX") != (char *)NULL) {
		if (strcmp(getenv("GTKLP_FORCEX"), "1") == 0) {
			forceX = 1;
			printf("Force X !\n");
		}
	}

	/*-- Use X ? --*/
	nox = noX();

	/*-- See how we are called */
	calledGtkLP = 1;
	calledLpr = 0;
	calledLp = 0;
	if (strcmp(basename(argv[0]), "lpr") == 0) {
		calledLpr = 1;
		calledGtkLP = 0;
		if (forceX == 0)	/* Default: No X when called as lpr */
			nox = 1;
		if (DEBUG)
			printf("Called as LPR\n");
	}
	if (strcmp(basename(argv[0]), "lp") == 0) {
		calledLp = 1;
		calledGtkLP = 0;
		if (forceX == 0)	/* Default: No X when called as lp */
			nox = 1;
		if (DEBUG)
			printf("Called as LP\n");
	}

	/*--- Get Programm Options ---*/
	num_commandline_opts = 0;
	commandline_opts = NULL;
	DEF_PRN[0] = (uintptr_t) NULL;
	confdir[0] = (uintptr_t) NULL;
	ServerName[0] = (uintptr_t) NULL;
	PortString[0] = (uintptr_t) NULL;
	ServerAddr[0] = (uintptr_t) NULL;
	UserName[0] = (uintptr_t) NULL;
	ReallyPrint = 1;
	needFileSelection = 1;
	cmdNumCopies = -1;
	builtin = 0;
	tabtoopen = 1;
	noFileSelection = 0;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
#if GTK_MAJOR_VERSION != 1
	bind_textdomain_codeset(PACKAGE, "UTF-8");
#endif
	textdomain(PACKAGE);

	/*--- GUI creation ---*/
	if (nox == 0) {
		setlocale(LC_ALL, "");
		gtk_init(&argc, &argv);

#if GTK_MAJOR_VERSION != 1
		/* Default Icons */
		setWindowDefaultIcons();
#endif

		/* HauptFenster */
		mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(mainWindow), PROGNAME);
		gtk_window_set_position(GTK_WINDOW(mainWindow),
					GTK_WIN_POS_CENTER);
		gtk_widget_set_size_request(mainWindow, WAIT_WIN_SIZE, -1);
		gtk_signal_connect(GTK_OBJECT(mainWindow), "delete_event",
				   GTK_SIGNAL_FUNC(destroy), NULL);
		gtk_signal_connect(GTK_OBJECT(mainWindow), "destroy",
				   GTK_SIGNAL_FUNC(destroy), NULL);
		gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 10);
		gtk_widget_realize(mainWindow);

		/* Vertikaler Container im HauptFenster */
		mainVBox = gtk_vbox_new(FALSE, BUTTON_SPACING_H);
		gtk_container_add(GTK_CONTAINER(mainWindow), mainVBox);
		gtk_widget_show(mainVBox);

		waitLabel = gtk_label_new(str2str(_("Please wait!")));
		gtk_box_pack_start(GTK_BOX(mainVBox), waitLabel, TRUE, TRUE, 0);
		gtk_widget_show(waitLabel);

#if GTK_MAJOR_VERSION != 1
		progressBar = gtk_progress_bar_new();
		gtk_box_pack_start(GTK_BOX(mainVBox), progressBar, TRUE, TRUE,
				   0);
		gtk_widget_show(progressBar);
#endif

		while (gtk_events_pending())
			gtk_main_iteration();

	}

	i1 = getopt(argc, argv, ProgOpts);
	while (i1 != -1) {
		switch (i1) {
		case 'H':
			if (calledLp) {
				if (optarg == (char *)NULL) {
					g_print("-H: %s\n",
						str2str(_
							("This option requires an value !")));
					break;
				}
				if (strcmp(optarg, "hold") == 0) {
					num_commandline_opts =
					    cupsAddOption("job-hold-until",
							  "indefinite",
							  num_commandline_opts,
							  &commandline_opts);
					break;
				}
				if ((strcmp(optarg, "resume") == 0)
				    || (strcmp(optarg, "release") == 0)) {
					num_commandline_opts =
					    cupsAddOption("job-hold-until",
							  "no-hold",
							  num_commandline_opts,
							  &commandline_opts);
					break;
				}
				if (strcmp(optarg, "immediate") == 0) {
					num_commandline_opts =
					    cupsAddOption("job-priority", "100",
							  num_commandline_opts,
							  &commandline_opts);
					break;
				}
				num_commandline_opts =
				    cupsAddOption("job-hold-until", optarg,
						  num_commandline_opts,
						  &commandline_opts);
				break;	/* break, cause this means help if not lp */
			}
		case '?':
			if (calledGtkLP) {
				g_print("%s",
					str2str(_
						("Usage: gtklp [-P|-d printer] [-c configdir] [-S server] [-U user] [-p port] [-l] [-D] [-V] [-b] [-i] [-# n] [-C] [-H] [-E] [-J jobname] [-q priority] [-o option=value ...] [file(s)]\n")));
			}
			return (0);
			break;
		case 'i':
			if (calledGtkLP) {
				if (DEBUG)
					printf("Ignoring Data on stdin !\n");
				ignore_stdin = 1;
			}
			if (calledLp) {
				g_print("lp -i: %s \n",
					str2str(_
						("Function is not supported for now !")));
				exit(-1);
				break;
			}
			if (calledLpr)
				break;
			break;
		case 'b':
			if (DEBUG)
				printf("Using builtin defaults !\n");
			builtin = 1;
			break;
		case 'p':
			if (calledLpr) {
				num_commandline_opts =
				    cupsParseOptions("prettyprint=true",
						     num_commandline_opts,
						     &commandline_opts);
				break;
			}
			if (calledLp)
				break;
			if (calledGtkLP) {
				if (optarg == (char *)NULL)
					break;
				if (strlen(optarg) > MAX_PORT) {
					if (DEBUG)
						printf("Port too long !\n");
					exitOnError(str2str
						    (_("Invalid Portnumber !")),
						    "", -1);
				} else {
					if (DEBUG)
						printf("Using port: %s\n",
						       optarg);
					snprintf(PortString,
						 (size_t) MAX_PORT + 10,
						 "IPP_PORT=%s", optarg);
					putenv(PortString);
				}
			}
			break;
		case 'P':
			if (calledLp) {
				num_commandline_opts =
				    cupsAddOption("page-ranges", optarg,
						  num_commandline_opts,
						  &commandline_opts);
				break;	/* break, cause P is special for lp */
			}
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
		case 'S':
			if (calledLp)
				break;
		case 'h':
			if (calledLpr)
				break;
			if (optarg == (char *)NULL)
				break;
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
		case 'l':
			if (calledGtkLP) {
				ReallyPrint = 0;
				needFileSelection = 0;
			}
			if (calledLpr) {
				num_commandline_opts =
				    cupsParseOptions("raw=true",
						     num_commandline_opts,
						     &commandline_opts);
			}
			break;
		case 'D':
			DEBUG = 1;
			printf("DEBUG-MODE on !\n");
			break;
		case 'c':
			if (calledGtkLP) {
				strncpy(confdir, optarg, (size_t) MAXPATH);
			}
			break;
		case 'C':
			if (calledGtkLP) {
				nox = 1;
				break;	/* break, cause C is console-mode for gtklp, but Title for lpr */
			}
			/* No break, cause C is the same as J,T for lpr, but not for lp.... */
		case 'J':
		case 'T':
		case 't':
			strncpy(JobName, optarg, MAX_JOB_NAME_LENGTH);
			break;
		case '#':
		case 'n':
			cmdNumCopies = atoi(optarg);
			break;
		case 'V':
			g_print("\n%s %s %s\n", PROGNAME, str2str(_("version")),
				VERSION);
			g_print("%s\n\n", MY_HOME);
			return (0);
			break;
		case 'o':
			num_commandline_opts =
			    cupsParseOptions(optarg, num_commandline_opts,
					     &commandline_opts);
			break;
		case 'm':
			break;
		case 'q':
			if (atoi(optarg) < 0) {
				num_commandline_opts =
				    cupsAddOption("job-priority", "0",
						  num_commandline_opts,
						  &commandline_opts);
				break;
			}
			if (atoi(optarg) > 100) {
				num_commandline_opts =
				    cupsAddOption("job-priority", "100",
						  num_commandline_opts,
						  &commandline_opts);
				break;
			}
			num_commandline_opts =
			    cupsAddOption("job-priority", optarg,
					  num_commandline_opts,
					  &commandline_opts);
			break;
		case 's':
			if (calledLp) {
				if (DEBUG)
					printf
					    ("Silent mode not necessary yet. I AM silent...\n");
				break;
			}
			if (calledLpr)
				break;
			break;
		case 'r':
			if (calledLpr) {
				g_print("lp -t not supported !\n");
			}
			if (calledLp)
				break;
			break;
		case 'f':
			break;
		case 'w':
			break;
		case 'y':
			break;
		case '1':
		case '2':
		case '3':
		case '4':
			break;
		case 'X':
			nox = noX();
			break;
		case 'E':
#ifdef HAVE_LIBSSL
			cupsSetEncryption(HTTP_ENCRYPT_REQUIRED);
#else
			exitOnError(str2str(_("-E: %s\n")),
				    str2str(_
					    ("Sorry, this function is not compiled in !")),
				    -2);
#endif
			break;
		case 'F':
			noFileSelection = 1;
			break;
		default:
			if (DEBUG)
				printf
				    ("PROG-ERROR: Unknown, but allowed switch: %c\n",
				     i1);
			emergency();
			break;
		}
		i1 = getopt(argc, argv, ProgOpts);
	}
#if GTK_MAJOR_VERSION != 1
	if (progressBar != (GtkWidget *) NULL)
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),
					      0.025);
#endif
	filesToPrintAnz = 0;
	if (optind < argc) {
		while (optind < argc) {
			if (filesToPrintAnz < MAXPRINTFILES) {
				strncpy(filesToPrint
					[(unsigned char)filesToPrintAnz++],
					argv[optind++], (size_t) MAXPATH);
			} else {
				if (DEBUG)
					printf
					    ("Prog-Error: MAXPRINTFILES to short !\n");
				exitOnError(str2str
					    (_("Too many files to print !")),
					    "", -3);
			}
		}
	}
	if (filesToPrintAnz > 0) {
		ignore_stdin = 1;
		if (noFileSelection == 1) {
			needFileSelection = 0;
		}
	}
#if GTK_MAJOR_VERSION != 1
	if (progressBar != (GtkWidget *) NULL)
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),
					      0.05);
#endif

	if (DEBUG) {
		printf("Files to print: \n");
		for (i1 = 0; i1 < filesToPrintAnz; i1++)
			printf("  %s\n", filesToPrint[i1]);

	}
	if (DEBUG) {
		if (optind < argc) {
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ", argv[optind++]);
			printf("\n");
		}
	}

  /*--- GUI creation ---*/
	if (nox == 0) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	hasAskedPWD = 0;	/* cups has not asked for pwd */

	if (nox == 0)
		cupsSetPasswordCB(getPass);
  /*-- GUI Init ende ---*/

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
	/* Set server location */
	if ((stat(ServerName, &snstat) == 0) && S_ISSOCK(snstat.st_mode)) {
		/* Unix socket */
		if (PortString[0]) {	/* port specified */
			sprintf(ServerAddr, "127.0.0.1:%s", PortString + 9);
		} else {
			sprintf(ServerAddr, "127.0.0.1:%d", ippPort());
		}
	} else {
		if (PortString[0]) {	/* port specified */
			sprintf(ServerAddr, "%s:%s", ServerName,
				PortString + 9);
		} else {
			strcpy(ServerAddr, ServerName);
		}
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
		snprintf(tmpenv, (size_t) MAX_SERVER_NAME, "CUPS_SERVER=%s",
			 ServerName);
		cupsSetServer(ServerName);
		putenv(tmpenv);
	}
	cupsLanguage = cupsLangDefault();

#if GTK_MAJOR_VERSION != 1
	if (progressBar != (GtkWidget *) NULL)
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),
					      0.075);
#endif

	/* Setting Username */
	cupsSetUser((const char *)UserName);
	if (DEBUG)
		printf("Username is set to: %s\n", cupsUser());

	/* Create non command-line options */

	/* confdir */
	if (DEBUG)
		printf("Search for Confdir.\n");
	getPaths();
	if (DEBUG) {
		printf("Using:GTKLPRC=%s\nconfdir=%s\n", GTKLPRC, confdir);
	}

	/* CWD */
	fileOpenPath[0] = (uintptr_t) NULL;
	getcwd(fileOpenPath, MAXPATH);
	strncat(fileOpenPath, "/", (size_t) MAXPATH);
	if (DEBUG)
		printf("PWD=%s\n", fileOpenPath);

	/*--- get Global Prog-Options ---*/
	getOptions(DEF_PRN, 2);

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
	/*--- get Options for this Printer ---*/
	if (getPPDOpts(DEF_PRN) < 0) {	/* exit if password wrong */
		cupsLangFlush();
		cupsLanguage = NULL;
		httpClose(cupsHttp);
		cupsHttp = NULL;
		exit(-2);
	}
	/* Collate might need getPPDOpts to have been called */
	getOptions(DEF_PRN, -1);	/* Get only important options -> Passwort !!!!! */
	/*--- Den Rest der GUI bauen ---*/

	if (nox == 0) {
		/* Tab Environment basteln */
		tabs = gtk_notebook_new();
		gtk_box_pack_start(GTK_BOX(mainVBox), tabs, TRUE, TRUE, 0);
		gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_TOP);
		//gtk_widget_show(tabs);

		/* Buttons basteln */
		buttonBox = gtk_hbox_new(TRUE, BUTTON_SPACING_H);
		gtk_box_pack_end(GTK_BOX(mainVBox), buttonBox, FALSE, FALSE, 0);

#if GTK_MAJOR_VERSION != 1
		buttonPrint = gtk_button_new_from_stock(GTK_STOCK_PRINT);
#else
		buttonPrint = gtk_button_new_with_label(str2str(_("Print")));
#endif
		button_pad(buttonPrint);
		gtk_signal_connect(GTK_OBJECT(buttonPrint), "clicked",
				   GTK_SIGNAL_FUNC(PrintFunc), NULL);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonPrint, FALSE, TRUE,
				   0);
		gtk_widget_show(buttonPrint);

#if GTK_MAJOR_VERSION != 1
		buttonReset =
		    gtk_button_new_from_stock(GTK_STOCK_REVERT_TO_SAVED);
#else
		buttonReset =
		    gtk_button_new_with_label(str2str(_("Reset All")));
#endif
		button_pad(buttonReset);
		gtk_signal_connect(GTK_OBJECT(buttonReset), "clicked",
				   GTK_SIGNAL_FUNC(ResetFunc), NULL);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonReset, FALSE, TRUE,
				   0);
		gtk_widget_show(buttonReset);

#if GTK_MAJOR_VERSION != 1
		buttonSave = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
#else
		buttonSave = gtk_button_new_with_label(str2str(_("Templates")));
#endif
		button_pad(buttonSave);
		gtk_signal_connect(GTK_OBJECT(buttonSave), "clicked",
				   GTK_SIGNAL_FUNC(instWin), NULL);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonSave, FALSE, TRUE,
				   0);
		gtk_widget_show(buttonSave);

#if GTK_MAJOR_VERSION != 1
		buttonAbbruch = gtk_button_new_from_stock(GTK_STOCK_QUIT);
#else
		buttonAbbruch = gtk_button_new_with_label(str2str(_("Close")));
#endif
		button_pad(buttonAbbruch);
		gtk_signal_connect(GTK_OBJECT(buttonAbbruch), "clicked",
				   GTK_SIGNAL_FUNC(AbbruchFunc), NULL);
		gtk_box_pack_start(GTK_BOX(buttonBox), buttonAbbruch, FALSE,
				   TRUE, 0);
		gtk_widget_show(buttonAbbruch);

#if GTK_MAJOR_VERSION != 1
		if (progressBar != (GtkWidget *) NULL) {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR
						      (progressBar), 0.95);
			while (gtk_events_pending())
				gtk_main_iteration();
		}
#endif

		/* Tabs basteln */
		CreateWidgets();

#if GTK_MAJOR_VERSION != 1
		if (progressBar != (GtkWidget *) NULL) {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR
						      (progressBar), 0.99);
			while (gtk_events_pending())
				gtk_main_iteration();
		}
#endif

		/* Fenster anzeigen */
		// XXX gtk_widget_hide(mainWindow);
		gtk_widget_hide(waitLabel);
#if GTK_MAJOR_VERSION != 1
		gtk_widget_hide(progressBar);
#endif
		ShowWidgets();
		if (DEBUG)
			printf("Switching to Tab %i\n", tabtoopen);

#if GTK_MAJOR_VERSION == 1
		gtk_notebook_set_page(GTK_NOTEBOOK(tabs), tabtoopen);
#else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), tabtoopen);
#endif

		gtk_widget_show(buttonBox);

#if GTK_MAJOR_VERSION != 1
		/* Restore Position */
		if (nox == 0) {
			if (wantSaveSizePos == 1) {
				if (mainWindowHeight > 0 && mainWindowWidth > 0) {

					gtk_widget_set_uposition(GTK_WIDGET
								 (mainWindow),
								 mainWindowX,
								 mainWindowY);
					while (gtk_events_pending())
						gtk_main_iteration();
					gtk_window_resize(GTK_WINDOW
							  (mainWindow),
							  mainWindowWidth,
							  mainWindowHeight);
					while (gtk_events_pending())
						gtk_main_iteration();
				}
			}
		}
#endif
		if (nox == 0)
			gtk_widget_show(mainWindow);

		if (GTK_IS_WIDGET(dialog) == 1) {	/* Error Windows back to foreground */
			gtk_widget_hide(dialog);
			gtk_window_set_transient_for(GTK_WINDOW(dialog),
						     GTK_WINDOW(mainWindow));
			gtk_widget_show(dialog);
		}

		gtk_widget_grab_focus(buttonPrint);

	}

	conflict_active = 1;
	if (nox == 0) {
		if (printerPPD != NULL) {
			while (gtk_events_pending())
				gtk_main_iteration();
			constraints(ppdConflicts(printerPPD));
		}
		gtk_main();
	} else {
		PrintFunc(NULL, NULL);
	}
	return (0);
}
