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

#include "gtklp_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/http.h>
#include <cups/language.h>
#include <cups/ppd.h>

#include <gettext.h>
#include <defaults.h>
#include <libgtklp.h>

#include "file.h"
#include "general.h"
#include "gtklp.h"
#include "gtklptab.h"
#include "hpgl2.h"
#include "image.h"
#include "output.h"
#include "ppd.h"
#include "special.h"
#include "text.h"

#include "conflict.xpm"
#include "rarrow.xpm"

unsigned topmargin, bottommargin, leftmargin, rightmargin;
int imageScalingType;
double imageScalingScalingValue;
double imageScalingNaturalValue;
double imageScalingPPIValue;
int hasAskedPWD;

ppd_file_t *printerPPD;

GtkWidget *mainWindow;

#if GTK_MAJOR_VERSION == 1
GtkWidget *button_inst_save_label;
#else
GtkWidget *button_inst_save;
#endif
GtkWidget *button_inst_remove;
GtkWidget *instwin;

int teststdin()
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	int ret;

	if (ignore_stdin)	/* Ignore stdin, needed for acroread */
		return (0);

	/* Achte auf stdin (fd 0), um zu sehen, wenn es
	 * Eingaben gibt.
	 */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	/* Warte bis zu fünf Sekunden. */
	tv.tv_sec = 0;
	tv.tv_usec = WAITFORSTDIN;

	retval = select(1, &rfds, NULL, NULL, &tv);
	/* Verlaß Dich jetzt bloß nicht auf den Wert von tv! */

	if (retval) {
		if (DEBUG)
			printf("Data on stdin\n");
		ret = 1;
	} else {
		if (DEBUG)
			printf("No Data on stdin at last %u u-secs.\n",
			       WAITFORSTDIN);
		ret = 0;
	}
	return (ret);
}

void getPaths(void)
{
	char *z1;

	GTKLPRC[0] = (uintptr_t) NULL;

	if (confdir[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf("No conffile given at command-line !\n");

		if (getenv("HOME") == (char *)NULL) {
			if (DEBUG)
				printf("ERROR: No Home-Variable set !\n");
		} else {
			snprintf((char *)confdir, (size_t) MAXPATH, "%s/%s",
				 getenv("HOME"), GTKLPRC_USER);
		}
	} else {
		z1 = (char *)confdir;
		z1 += strlen((char *)confdir);
		z1--;
		if (*z1 == '/')
			*z1 = (uintptr_t) NULL;
	}

	snprintf((char *)GTKLPRC, (size_t) MAXPATH, "%s/%s", confdir,
		 GTKLPRC_GLOBAL);

}

void getBannerList(char *printername)
{
	int i1;
	ipp_t *request, *response;
	ipp_attribute_t *attr;
	char URI[MAX_URI + 1];
	char *z1;

	bannerList = NULL;

	if (DEBUG)
		printf("Search for Banners for %s\n", printername);

	z1 = strchr(printername, '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;

	snprintf(URI, (size_t) MAX_URI, "ipp://%s/printers/%s", ServerAddr,
		 printername);

	if (z1 != NULL)
		*z1 = '/';

	request = ippNew();
	ippSetOperation(request, IPP_GET_PRINTER_ATTRIBUTES);
	ippSetRequestId(request, 1);
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL,
		     cupsLangEncoding(cupsLanguage));
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     cupsLanguage->language);
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
		     NULL, URI);
	if ((response = cupsDoRequest(cupsHttp, request, "/")) == NULL) {
		if (DEBUG)
			printf
			    ("PROG-ERROR: unable for doing cupsDoRequest !\n");
		emergency();
	}
	if ((attr =
	     ippFindAttribute(response, "job-sheets-supported",
			      IPP_TAG_ZERO)) == NULL) {
		if (DEBUG)
			printf("No sheets found !\n");
		bannerAnz = 1;
		strncpy(Banners[0], "none", (size_t) MAXLINE);
		bannerList =
		    g_list_append(bannerList, (gchar *) str2str(Banners[0]));
	} else {
		bannerAnz = ippGetCount(attr);
		if (bannerAnz > MAX_BANNER) {
			if (DEBUG)
				printf("Too many banners found for %s: %u\n",
				       printername, bannerAnz);
			emergency();
		}
		if (DEBUG)
			printf("Found %i banners\n", bannerAnz);
		for (i1 = 0; i1 < bannerAnz; i1++) {
			if (DEBUG)
				printf("  %s\n", ippGetString(attr, i1, NULL));
			strncpy(Banners[i1],
				(char *)ippGetString(attr, i1, NULL),
				(size_t) MAXLINE);
			bannerList =
			    g_list_append(bannerList,
					  (gchar *) str2str(Banners[i1]));
		}
	}
	ippDelete(response);
}

void getOptions(char *printername, int which)
{
	int i1, hasrc, nohome;
	int needNew = 0;
	int whichfile = 0;
	char printpath[MAXPATH + 1], globalpath[MAXPATH + 1];
	char printpath_g[MAXPATH + 1], globalpath_g[MAXPATH + 1];
	char tmp[MAXLINE + 1];
	char iprintername[DEF_PRN_LEN + 1];
	FILE *file = NULL;
	int num_printer;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	cups_option_t *option;
	char *z1;
#if GTK_MAJOR_VERSION != 1
	int screenHeight, screenWidth;
#endif
	num_printer = 0;
	all_printer = NULL;
	act_printer = NULL;
	instance = NULL;

	if (DEBUG)
		printf("Getting Defaults: %i\n", which);

	z1 = strchr(printername, '/');
	if (z1 != NULL) {
		*z1 = (uintptr_t) NULL;
		snprintf(iprintername, (size_t) DEF_PRN_LEN, "%s\\%s",
			 printername, z1 + 1);
		*z1 = '/';
		z1++;
		z1 = strchr(z1, '/');
		if (z1 != NULL) {
			if (DEBUG)
				printf("ERROR: Wrong printername !\n");
			emergency();
		}
	} else {
		strncpy(iprintername, printername, (size_t) DEF_PRN_LEN);
	}

	/* Check for user-settings */
	if (!builtin) {
		nohome = 0;
		printpath[0] = (uintptr_t) NULL;
		globalpath[0] = (uintptr_t) NULL;
		printpath_g[0] = (uintptr_t) NULL;
		globalpath_g[0] = (uintptr_t) NULL;

		if (getenv("HOME") == (char *)NULL) {
			if (DEBUG)
				printf("ERROR: No Home-Variable set !\n");
			nohome = 1;
		}

		strncpy(printpath_g, DEF_GTKLPRC, (size_t) MAXPATH);
		strncat(printpath_g, "/", (size_t) MAXPATH);
		strncat(printpath_g, iprintername, (size_t) MAXPATH);
		strncpy(globalpath_g, DEF_GTKLPRC, (size_t) MAXPATH);
		strncat(globalpath_g, "/", (size_t) MAXPATH);
		strncat(globalpath_g, GTKLPRC_GLOBAL, (size_t) MAXPATH);

	} else {
		if (DEBUG)
			printf("Using builtin defaults !\n");
		nohome = 1;
	}
	if (nohome == 0) {
		snprintf(printpath, (size_t) MAXPATH, "%s/%s", confdir,
			 iprintername);
		snprintf(globalpath, (size_t) MAXPATH, "%s/%s", confdir,
			 GTKLPRC_GLOBAL);
	} else {
		strncpy(printpath, printpath_g, (size_t) MAXPATH);
		strncpy(globalpath, globalpath_g, (size_t) MAXPATH);
	}

	/* Printer dependent settings */
	if (which <= 1) {
		if (DEBUG)
			printf("Getting Defaults for %s\n", printername);
		strncpy(nupLayout, "lrtb", (size_t) 4);
		strncpy(borderType, "none", (size_t) 12);
		NumberOfCopies = 1;
		wantLandscape = 0;
		wantCustomPageSize = 0;
		wantRange = 0;
		wantReverseOutputOrder = 0;
		Range[0] = (uintptr_t) NULL;
		rangeType = 2;
		mirroroutput = 0;
		sheetsType = 1;
		brightness = 100;
		ggamma = 1000;
		cpivalue = 10;
		lpivalue = 6;
		cppvalue = 1;
		topmargin = 0;
		bottommargin = 0;
		leftmargin = 0;
		rightmargin = 0;
		prettyprint = 0;
		collate = 0;
		textwrap = 0;
		imageScalingType = 0;
		imageScalingScalingValue = 100;
		imageScalingNaturalValue = 100;
		imageScalingPPIValue = 300;

		hue = 0;
		sat = 100;
		ImagePos[0] = 1;
		ImagePos[1] = 1;
		hpgl2black = 0;
		hpgl2fit = 0;
		hpgl2pen = 1000;
		wantraw = 0;
		ExtraOpt[0] = (uintptr_t) NULL;
		viewable[0] = 1;
		viewable[1] = 1;
		viewable[2] = 1;
		viewable[3] = 1;
		viewable[4] = 1;
		viewable[5] = 1;
		if (UserName[0] == (uintptr_t) NULL) {
			if ((strlen((char *)cupsUser()) < MAX_USERNAME))
				strncpy(UserName, (char *)cupsUser(),
					(size_t) MAX_USERNAME);
			else
				UserName[0] = (uintptr_t) NULL;
		}
		passWord[0] = (uintptr_t) NULL;
		hasAskedPWD = 0;

		hasrc = 0;
		if (!builtin) {
			if (DEBUG)
				printf("Trying to open %s\n", printpath);
			file = fopen(printpath, "r");
			if (file == (FILE *) NULL) {
				if (DEBUG)
					printf("Can't open users rc-file !\n");
				if (DEBUG)
					printf("Trying to open %s\n",
					       printpath_g);
				file = fopen(printpath_g, "r");
				if (file == (FILE *) NULL) {
					if (DEBUG)
						printf
						    ("Can't open global rc-file !\n");
				} else {
					if (DEBUG)
						printf
						    ("global rc-file found !\n");
					hasrc = 1;
					whichfile = 2;
				}
			} else {
				if (DEBUG)
					printf("users rc-file found !\n");
				hasrc = 1;
				whichfile = 1;
			}
		} else {
			if (DEBUG)
				printf("Using builtin defaults !\n");
		}

		num_printer = cupsGetDests(&all_printer);
		instance = strchr(printername, '/');
		if (instance != NULL) {
			*instance = (uintptr_t) NULL;
			if (DEBUG)
				printf("Try to get printer: %s/%s\n",
				       printername, instance + 1);
			act_printer =
			    cupsGetDest(printername, instance + 1, num_printer,
					all_printer);
			*instance = '/';
		} else {
			if (DEBUG)
				printf("Try to get printer: %s\n", printername);
			act_printer =
			    cupsGetDest(printername, NULL, num_printer,
					all_printer);
		}

		if (act_printer == NULL) {
			if (DEBUG)
				printf("ERROR: Dest unknown (1) !\n");
			emergency();
		}

		if (DEBUG) {
			printf("ALL OPTIONS: %s\n", act_printer->name);
			for (i1 = act_printer->num_options, option =
			     act_printer->options; i1 > 0; option++, i1--) {
				printf("%i: %s %s\n", i1, option->name,
				       option->value);
			}
		}

		snprintf(CustomPageSize,
			 (size_t) 2 * MAX_CUSTOM_SIZE_LENGTH + 1, "%s\n",
			 cupsGetOption("PageSize", act_printer->num_options,
				       act_printer->options));

		for (i1 = num_commandline_opts, option = commandline_opts;
		     i1 > 0; option++, i1--) {
			if (DEBUG)
				printf("CMDLine: %s %s\n", option->name,
				       option->value);
			act_printer->num_options =
			    cupsAddOption(option->name, option->value,
					  act_printer->num_options,
					  &act_printer->options);
		}

		if (cmdNumCopies > 0) {
			NumberOfCopies = cmdNumCopies;
		} else {
			if (cupsGetOption
			    ("copies", act_printer->num_options,
			     act_printer->options) != (char *)NULL)
				NumberOfCopies =
				    atoi(cupsGetOption
					 ("copies", act_printer->num_options,
					  act_printer->options));
			oldnumcopies = NumberOfCopies;
		}

		if (cupsGetOption
		    ("collate", act_printer->num_options,
		     act_printer->options) != NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("collate", act_printer->num_options,
			      act_printer->options), "true") == 0)
				collate = 1;
			else
				collate = 0;
		} else if (ppdFindMarkedChoice(printerPPD, "Collate") != NULL) {
			if (strcasecmp
			    (ppdFindMarkedChoice(printerPPD, "Collate")->choice,
			     "true") == 0)
				collate = 1;
			else
				collate = 0;
		}

		if (cupsGetOption
		    ("landscape", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("landscape", act_printer->num_options,
			      act_printer->options), "true") == 0)
				wantLandscape = 1;
		}

		if (cupsGetOption
		    ("page-ranges", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			wantRange = 1;
			strncpy(Range,
				cupsGetOption("page-ranges",
					      act_printer->num_options,
					      act_printer->options),
				(size_t) MAXRANGEENTRY);
		}

		if (cupsGetOption
		    ("outputorder", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("outputorder", act_printer->num_options,
			      act_printer->options), "normal") == 0)
				wantReverseOutputOrder = 0;
			if (strcasecmp
			    (cupsGetOption
			     ("outputorder", act_printer->num_options,
			      act_printer->options), "reverse") == 0)
				wantReverseOutputOrder = 1;
		}

		if (cupsGetOption
		    ("page-set", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("page-set", act_printer->num_options,
			      act_printer->options), "even") == 0)
				rangeType = 1;
			if (strcasecmp
			    (cupsGetOption
			     ("page-set", act_printer->num_options,
			      act_printer->options), "odd") == 0)
				rangeType = 0;
		}

		if (cupsGetOption
		    ("number-up", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			sheetsType =
			    atoi(cupsGetOption
				 ("number-up", act_printer->num_options,
				  act_printer->options));
			if ((sheetsType != 1) & (sheetsType !=
						 2) & (sheetsType !=
						       3) & (sheetsType !=
							     4) & (sheetsType !=
								   6) &
			    (sheetsType != 9) & (sheetsType != 16))
				sheetsType = 1;
		}

		if (cupsGetOption
		    ("number-up-layout", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			strncpy(nupLayout,
				cupsGetOption("number-up-layout",
					      act_printer->num_options,
					      act_printer->options),
				(size_t) 4);
		}

		if (cupsGetOption
		    ("mirror", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("mirror", act_printer->num_options,
			      act_printer->options), "true") == 0)
				mirroroutput = 1;
		}

		if (cupsGetOption
		    ("page-border", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			strncpy(borderType,
				cupsGetOption("page-border",
					      act_printer->num_options,
					      act_printer->options),
				(size_t) 12);
		}

		if (cupsGetOption
		    ("brightness", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			brightness =
			    atof(cupsGetOption
				 ("brightness", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("gamma", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			ggamma =
			    atof(cupsGetOption
				 ("gamma", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("cpi", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			cpivalue =
			    (unsigned)
			    atol(cupsGetOption
				 ("cpi", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("lpi", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			lpivalue =
			    (unsigned)
			    atol(cupsGetOption
				 ("lpi", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("columns", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			cppvalue =
			    (unsigned)
			    atol(cupsGetOption
				 ("columns", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("page-top", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			topmargin =
			    (unsigned)
			    atol(cupsGetOption
				 ("page-top", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("page-bottom", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			bottommargin =
			    (unsigned)
			    atol(cupsGetOption
				 ("page-bottom", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("page-left", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			leftmargin =
			    (unsigned)
			    atol(cupsGetOption
				 ("page-left", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("page-right", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			rightmargin =
			    (unsigned)
			    atol(cupsGetOption
				 ("page-right", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("prettyprint", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("prettyprint", act_printer->num_options,
			      act_printer->options), "true") == 0)
				prettyprint = 1;
		}

		if (cupsGetOption
		    ("wrap", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("wrap", act_printer->num_options,
			      act_printer->options), "true") == 0)
				textwrap = 1;
		}

		if (cupsGetOption
		    ("scaling", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			imageScalingScalingValue =
			    atof(cupsGetOption
				 ("scaling", act_printer->num_options,
				  act_printer->options));
			imageScalingType = 1;
		}

		if (cupsGetOption
		    ("natural-scaling", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			imageScalingNaturalValue =
			    atof(cupsGetOption
				 ("natural-scaling", act_printer->num_options,
				  act_printer->options));
			imageScalingType = 2;
		}

		if (cupsGetOption
		    ("ppi", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			imageScalingPPIValue =
			    atof(cupsGetOption
				 ("ppi", act_printer->num_options,
				  act_printer->options));
			imageScalingType = 3;
		}

		if (cupsGetOption
		    ("hue", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			hue =
			    atof(cupsGetOption
				 ("hue", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("saturation", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			sat =
			    atof(cupsGetOption
				 ("saturation", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("position", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "left") == 0) {
				ImagePos[0] = 0;
				ImagePos[1] = 1;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "right") == 0) {
				ImagePos[0] = 2;
				ImagePos[1] = 1;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "top") == 0) {
				ImagePos[0] = 1;
				ImagePos[1] = 0;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "top-left") == 0) {
				ImagePos[0] = 0;
				ImagePos[1] = 0;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "top-right") == 0) {
				ImagePos[0] = 2;
				ImagePos[1] = 0;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "bottom") == 0) {
				ImagePos[0] = 1;
				ImagePos[1] = 2;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "bottom-left") == 0) {
				ImagePos[0] = 0;
				ImagePos[1] = 2;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "bottom-right") == 0) {
				ImagePos[0] = 2;
				ImagePos[1] = 2;
			}
			if (strcasecmp
			    (cupsGetOption
			     ("position", act_printer->num_options,
			      act_printer->options), "center") == 0) {
				ImagePos[0] = 1;
				ImagePos[1] = 1;
			}
		}

		if (cupsGetOption
		    ("blackplot", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("blackplot", act_printer->num_options,
			      act_printer->options), "true") == 0)
				hpgl2black = 1;
		}

		if (cupsGetOption
		    ("fitplot", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("fitplot", act_printer->num_options,
			      act_printer->options), "true") == 0)
				hpgl2fit = 1;
		}

		if (cupsGetOption
		    ("penwidth", act_printer->num_options,
		     act_printer->options) != (char *)NULL)
			hpgl2pen =
			    (unsigned)
			    atol(cupsGetOption
				 ("penwidth", act_printer->num_options,
				  act_printer->options));

		if (cupsGetOption
		    ("job-sheets", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			strncpy(tmp,
				cupsGetOption("job-sheets",
					      act_printer->num_options,
					      act_printer->options),
				(size_t) MAXLINE);
			z1 = strchr(tmp, ',');
			if (z1 == (char *)NULL) {
				strncpy(PrinterBannerStart[PrinterChoice], tmp,
					(size_t) MAXLINE);
			} else {
				*z1 = (uintptr_t) NULL;
				z1++;
				strncpy(PrinterBannerStart[PrinterChoice], tmp,
					(size_t) MAXLINE);
				strncpy(PrinterBannerEnd[PrinterChoice], z1,
					(size_t) MAXLINE);
			}
		}

		if (cupsGetOption
		    ("raw", act_printer->num_options,
		     act_printer->options) != (char *)NULL) {
			if (strcasecmp
			    (cupsGetOption
			     ("raw", act_printer->num_options,
			      act_printer->options), "true") == 0)
				wantraw = 1;
		}

		if ((file != NULL) && (hasrc == 1)) {
			if (DEBUG)
				printf("Loading defaults for %s\n",
				       printername);

			tmp[0] = (uintptr_t) NULL;
			i1 = freadline(file, tmp, MAXLINE);
			if ((JobName[0] == (uintptr_t) NULL) && (tmp[0] != '-'))
				strncpy(JobName, str2str(tmp),
					(size_t) MAX_JOB_NAME_LENGTH);

			tmp[0] = (uintptr_t) NULL;
			i1 = freadline(file, tmp, MAXLINE);
			if (tmp[0] != '-')
				strncpy(ExtraOpt, str2str(tmp),
					(size_t) MAX_EXTRAOPT);
			else
				ExtraOpt[0] = (uintptr_t) NULL;

			fscanf(file, "%i %i %i %i %i %i\n", &viewable[0],
			       &viewable[1], &viewable[2], &viewable[3],
			       &viewable[4], &viewable[5]);

			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);
			if (tmp[0] != (uintptr_t) NULL) {
				if ((strlen(str2str(tmp)) < MAX_USERNAME)
				    && (tmp[0] != 255))
					strncpy(UserName, str2str(tmp),
						(size_t) MAX_USERNAME);
				else
					UserName[0] = (uintptr_t) NULL;
				cupsSetUser((const char *)UserName);
			}
			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);
			if ((strlen(str2str(tmp)) < MAX_PASSLEN)
			    && (tmp[0] != 255))
				strncpy(passWord, str2str(tmp),
					(size_t) MAX_PASSLEN);
			else
				passWord[0] = (uintptr_t) NULL;
			hasAskedPWD = 0;

			fclose(file);

			if (needNew == 1) {
				if (DEBUG)
					printf
					    ("Config-File has problems, I will delete it !\n");
				if (whichfile == 1) {
					if (DEBUG)
						printf("unlink %s\n",
						       printpath);
					i1 = unlink(printpath);
					if (i1 == -1) {
						if (DEBUG)
							printf("Fatal: %s\n",
							       strerror(errno));
					}
				}
				if (whichfile == 2) {
					if (DEBUG)
						printf("unlink %s\n",
						       printpath_g);
					i1 = unlink(printpath_g);
					if (i1 == -1) {
						if (DEBUG)
							printf("Fatal: %s\n",
							       strerror(errno));
					}
				}
				/* All wrong files deleted, reread and so ,get defaults */
				quick_message(str2str
					      (_
					       ("The PPD for this printer has changed!\nResetting...")),
					      2);
				PrinterChanged(NULL,
					       PrinterNames[PrinterChoice]);
				return;
			}
		}
	}
	hasrc = 0;
	/* Printer independent settings */

	if (which >= 1) {
		saveonexit = 1;
		saveanz = 1;
		rememberprinter = 0;
		remembertab = 0;
		clearfilelist = 1;
		exitonprint = 1;
		tabtoopen = 0;
		jobidonexit = 0;
		strncpy(BROWSER, DEF_BROWSER_CMD, (size_t) MAXPATH);
		strncpy(HELPURL, DEF_HELP_HOME, (size_t) MAXPATH);
		GTKLPQCOM[0] = (uintptr_t) NULL;
		wantconst = 0;
#if GTK_MAJOR_VERSION != 1
		mainWindowHeight = 0;
		mainWindowWidth = 0;
		mainWindowX = 0;
		mainWindowY = 0;
		wantSaveSizePos = 0;
#endif

		if (DEBUG)
			printf("Trying to open %s\n", globalpath);
		file = fopen(globalpath, "r");
		if (file == (FILE *) NULL) {
			if (DEBUG)
				printf("Can't find users globalrc-file !\n");
			if (DEBUG)
				printf("Trying to open %s\n", globalpath_g);
			file = fopen(globalpath_g, "r");
			if (file == (FILE *) NULL) {
				if (DEBUG)
					printf
					    ("Can't find global globalrc-file !\n");
			} else {
				if (DEBUG)
					printf
					    ("global globalrc-file found !\n");
				hasrc = 1;
			}
		} else {
			if (DEBUG)
				printf("users globalrc-file found !\n");
			hasrc = 1;
		}
		if (hasrc == 1) {
			if (DEBUG)
				printf("Loading global defaults\n");
			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);	/* last Printer */

			fscanf(file, "%i\n", &rememberprinter);
			if ((rememberprinter == 1)
			    && DEF_PRN[0] == (uintptr_t) NULL) {
				if (DEBUG)
					printf
					    ("set printer to remembered: %s\n",
					     tmp);
				strncpy(DEF_PRN, tmp, (size_t) DEF_PRN_LEN);
			}
			fscanf(file, "%i\n", &remembertab);
			fscanf(file, "%i\n", &tabtoopen);
			if (remembertab == 0)
				tabtoopen = 0;
			fscanf(file, "%i\n", &saveonexit);
			fscanf(file, "%i\n", &saveanz);
			fscanf(file, "%i\n", &clearfilelist);
			fscanf(file, "%i\n", &exitonprint);
			fscanf(file, "%i\n", &jobidonexit);
			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);
			strncpy(BROWSER, tmp, (size_t) MAXPATH);
			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);
			strncpy(HELPURL, tmp, (size_t) MAXPATH);
			tmp[0] = (uintptr_t) NULL;
			freadline(file, tmp, MAXLINE);
			strncpy(GTKLPQCOM, tmp, (size_t) MAXPATH);
			fscanf(file, "%i\n", &wantconst);
#if GTK_MAJOR_VERSION != 1
			if (fscanf(file, "%i %i %i %i %i\n", &wantSaveSizePos, &mainWindowX, &mainWindowY, &mainWindowWidth, &mainWindowHeight) == EOF) {	/* Old style conf */
				wantSaveSizePos = 0;
				mainWindowHeight = 0;
				mainWindowWidth = 0;
				mainWindowX = 0;
				mainWindowY = 0;
			} else {
				/* validate Size and Pos settings, ignore on too small screens */
				if (nox == 0) {
					screenHeight = gdk_screen_height();
					screenWidth = gdk_screen_width();

					if (mainWindowX < 0)
						mainWindowX = 0;
					if (mainWindowY < 0)
						mainWindowY = 0;

					if (mainWindowX + mainWindowWidth <
					    WINDOWINSCREENBORDER
					    || mainWindowY + mainWindowHeight <
					    WINDOWINSCREENBORDER
					    || screenWidth - mainWindowX <
					    WINDOWINSCREENBORDER
					    || screenHeight - mainWindowY <
					    WINDOWINSCREENBORDER) {
						mainWindowHeight = 0;
						mainWindowWidth = 0;
						mainWindowX = 0;
						mainWindowY = 0;
					}
				}

			}
#endif
			fclose(file);
		}
	}
}

int addAllOptionsHelp(ppd_group_t * group, cups_option_t ** optionsptr,
		      int num_options)
{
	int i1, i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for (i1 = group->num_options, option = group->options; i1 > 0;
	     i1--, option++) {
		for (i2 = option->num_choices, choice = option->choices; i2 > 0;
		     i2--, choice++) {
			if (choice->marked) {
				if (strncasecmp
				    (option->keyword, "copies",
				     (size_t) 6) != 0)
					num_options =
					    cupsAddOption(option->keyword,
							  choice->choice,
							  num_options,
							  optionsptr);
			}
		}
	}

	for (i1 = group->num_subgroups, subgroup = group->subgroups; i1 > 0;
	     i1--, subgroup++)
		num_options = addAllOptionsHelp(group, optionsptr, num_options);

	return (num_options);
}

int addAllOptions(cups_option_t ** optionsptr, int num_options)
{
	int i1;
	ppd_group_t *group;
	char tmp[MAXOPTLEN + 1];
	cups_option_t *option;

	option = NULL;

	/* === At first PPD-Options, because they should be overwritten  
	 *     by both command-line and interface entries === */
	if (printerPPD != NULL) {
		for (i1 = printerPPD->num_groups, group = printerPPD->groups;
		     i1 > 0; i1--, group++)
			num_options =
			    addAllOptionsHelp(group, optionsptr, num_options);
	}

	/* === Then Command-Line opts, cause they could be
	   overwritten by Interface-Entrys ====================== */

	for (i1 = num_commandline_opts, option = commandline_opts; i1 > 0;
	     option++, i1--) {
		if (DEBUG)
			printf("CMDLine: %s %s\n", option->name, option->value);
		num_options =
		    cupsAddOption(option->name, option->value, num_options,
				  optionsptr);
	}

	/* === Get Options from Interface === */

	/* General */
	if (NumberOfCopies > 0) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%i", NumberOfCopies);
		num_options =
		    cupsAddOption("copies", tmp, num_options, optionsptr);
	}

	if (collate == 1)
		num_options =
		    cupsAddOption("collate", "true", num_options, optionsptr);
	else
		num_options =
		    cupsAddOption("collate", "false", num_options, optionsptr);

	/* Output */
	if (wantRange == 1 && Range[0] != (uintptr_t) NULL)
		num_options =
		    cupsAddOption("page-ranges", Range, num_options,
				  optionsptr);
	if (wantReverseOutputOrder == 1)
		num_options =
		    cupsAddOption("outputorder", "reverse", num_options,
				  optionsptr);
	else
		num_options =
		    cupsAddOption("outputorder", "normal", num_options,
				  optionsptr);
	if (rangeType == 1)
		num_options =
		    cupsAddOption("page-set", "even", num_options, optionsptr);
	if (rangeType == 0)
		num_options =
		    cupsAddOption("page-set", "odd", num_options, optionsptr);
	if (sheetsType <= 16) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%i", sheetsType);
		num_options =
		    cupsAddOption("number-up", tmp, num_options, optionsptr);
	}
	if (strcmp(nupLayout, "lrtb") != 0)
		num_options =
		    cupsAddOption("number-up-layout", nupLayout, num_options,
				  optionsptr);

	if (mirroroutput == 1)
		num_options =
		    cupsParseOptions("mirror", num_options, optionsptr);

	if (strcmp(borderType, "none") != 0)
		num_options =
		    cupsAddOption("page-border", borderType, num_options,
				  optionsptr);
	if (wantLandscape == 1)
		num_options =
		    cupsParseOptions("landscape", num_options, optionsptr);

	if (wantCustomPageSize == 1) {
		snprintf(tmp, (size_t) MAXOPTLEN, "Custom.%sx%s%s",
			 (char *)
			 gtk_entry_get_text(GTK_ENTRY(customSizeFieldX)),
			 (char *)
			 gtk_entry_get_text(GTK_ENTRY(customSizeFieldY)),
			 (char *)
			 gtk_entry_get_text(GTK_ENTRY
					    (GTK_OBJECT
					     (GTK_COMBO(customSizeCombo)->
					      entry))));
		num_options =
		    cupsAddOption("PageSize", tmp, num_options, optionsptr);
	}

	if (nox == 0) {
		if (GTK_ADJUSTMENT(brightAdj)->value != 100) {
			snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
				 GTK_ADJUSTMENT(brightAdj)->value);
			num_options =
			    cupsAddOption("brightness", tmp, num_options,
					  optionsptr);
		}
		if (GTK_ADJUSTMENT(gammaAdj)->value != 1000) {
			snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
				 GTK_ADJUSTMENT(gammaAdj)->value);
			num_options =
			    cupsAddOption("gamma", tmp, num_options,
					  optionsptr);
		}
	} else {
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f", brightness);
		num_options =
		    cupsAddOption("brightness", tmp, num_options, optionsptr);
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f", ggamma);
		num_options =
		    cupsAddOption("gamma", tmp, num_options, optionsptr);
	}

	/* Text */
	if (cpivalue != 10) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", cpivalue);
		num_options =
		    cupsAddOption("cpi", tmp, num_options, optionsptr);
	}
	if (lpivalue != 6) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", lpivalue);
		num_options =
		    cupsAddOption("lpi", tmp, num_options, optionsptr);
	}
	if (cppvalue != 1) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", cppvalue);
		num_options =
		    cupsAddOption("columns", tmp, num_options, optionsptr);
	}
	if (topmargin != 0) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", topmargin);
		num_options =
		    cupsAddOption("page-top", tmp, num_options, optionsptr);
	}
	if (bottommargin != 0) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", bottommargin);
		num_options =
		    cupsAddOption("page-bottom", tmp, num_options, optionsptr);
	}
	if (leftmargin != 0) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", leftmargin);
		num_options =
		    cupsAddOption("page-left", tmp, num_options, optionsptr);
	}
	if (rightmargin != 0) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", rightmargin);
		num_options =
		    cupsAddOption("page-right", tmp, num_options, optionsptr);
	}
	if (prettyprint == 1)
		num_options =
		    cupsParseOptions("prettyprint", num_options, optionsptr);
	if (textwrap == 1)
		num_options =
		    cupsParseOptions("wrap=true", num_options, optionsptr);
	else
		num_options =
		    cupsParseOptions("wrap=false", num_options, optionsptr);

	/* Image */
	switch (ImagePos[1]) {
	case 1:
		switch (ImagePos[0]) {
		case 1:	/* Center (default) */
			num_options =
			    cupsAddOption("position", "center", num_options,
					  optionsptr);
			break;
		case 0:	/* Left */
			num_options =
			    cupsAddOption("position", "left", num_options,
					  optionsptr);
			break;
		case 2:	/* Right */
			num_options =
			    cupsAddOption("position", "right", num_options,
					  optionsptr);
			break;
		default:
			if (DEBUG)
				printf
				    ("PROG-ERROR: Unknown Image Position: x-0 !\n");
			emergency();
			break;
		}
		break;
	case 0:
		switch (ImagePos[0]) {
		case 1:	/* Top Center */
			num_options =
			    cupsAddOption("position", "top", num_options,
					  optionsptr);
			break;
		case 0:	/* Top Left */
			num_options =
			    cupsAddOption("position", "top-left", num_options,
					  optionsptr);
			break;
		case 2:	/* Top Right */
			num_options =
			    cupsAddOption("position", "top-right", num_options,
					  optionsptr);
			break;
		default:
			if (DEBUG)
				printf
				    ("PROG-ERROR: Unknown Image Position: x-1 !\n");
			emergency();
			break;
		}
		break;
	case 2:
		switch (ImagePos[0]) {
		case 1:	/* Bottom Center */
			num_options =
			    cupsAddOption("position", "bottom", num_options,
					  optionsptr);
			break;
		case 0:	/* Bottom Left */
			num_options =
			    cupsAddOption("position", "bottom-left",
					  num_options, optionsptr);
			break;
		case 2:	/* Bottom Right */
			num_options =
			    cupsAddOption("position", "bottom-right",
					  num_options, optionsptr);
			break;
		default:
			if (DEBUG)
				printf
				    ("PROG-ERROR: Unknown Image Position: x-2 !\n");
			emergency();
			break;
		}
		break;
	default:
		if (DEBUG)
			printf("PROG-ERROR: Unknown Image Position: y-x !\n");
		emergency();
		break;
	}
	switch (imageScalingType) {
	case 0:		/* None */
		break;
	case 1:		/* Sheet */
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
			 imageScalingScalingValue);
		num_options =
		    cupsAddOption("scaling", tmp, num_options, optionsptr);
		break;
	case 2:		/* Image */
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
			 imageScalingNaturalValue);
		num_options =
		    cupsAddOption("natural-scaling", tmp, num_options,
				  optionsptr);
		break;
	case 3:		/* PPI */
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f", imageScalingPPIValue);
		num_options =
		    cupsAddOption("ppi", tmp, num_options, optionsptr);
		break;
	}
	if (nox == 0) {
		if (GTK_ADJUSTMENT(imageHUEAdj)->value != 0) {
			snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
				 GTK_ADJUSTMENT(imageHUEAdj)->value);
			num_options =
			    cupsAddOption("hue", tmp, num_options, optionsptr);
		}
		if (GTK_ADJUSTMENT(imageSatAdj)->value != 100) {
			snprintf(tmp, (size_t) MAXOPTLEN, "%.0f",
				 GTK_ADJUSTMENT(imageSatAdj)->value);
			num_options =
			    cupsAddOption("saturation", tmp, num_options,
					  optionsptr);
		}
	} else {
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f", hue);
		num_options =
		    cupsAddOption("hue", tmp, num_options, optionsptr);
		snprintf(tmp, (size_t) MAXOPTLEN, "%.0f", sat);
		num_options =
		    cupsAddOption("saturation", tmp, num_options, optionsptr);
	}

	/* HPGL2 */
	if (hpgl2black == 1)
		num_options =
		    cupsParseOptions("blackplot", num_options, optionsptr);
	if (hpgl2fit == 1)
		num_options =
		    cupsParseOptions("fitplot", num_options, optionsptr);
	if (hpgl2pen != 1000) {
		snprintf(tmp, (size_t) MAXOPTLEN, "%u", hpgl2pen);
		num_options =
		    cupsAddOption("penwidth", tmp, num_options, optionsptr);
	}

	/* Special */
	if (nox == 0) {
		if (strcmp
		    ((char *)
		     gtk_entry_get_text(GTK_ENTRY
					((GTK_COMBO(bannerFrameStopCombo)->
					  entry))), "none") == 0) {
			if (strcmp
			    ((char *)
			     gtk_entry_get_text(GTK_ENTRY
						((GTK_COMBO
						  (bannerFrameStartCombo)->
						  entry))), "none") != 0) {
				snprintf(tmp, (size_t) MAXOPTLEN, "%s",
					 (char *)
					 gtk_entry_get_text(GTK_ENTRY
							    (GTK_COMBO
							     (bannerFrameStartCombo)->
							     entry)));
				num_options =
				    cupsAddOption("job-sheets", tmp,
						  num_options, optionsptr);
			}
		} else {
			snprintf(tmp, (size_t) MAXOPTLEN, "%s,%s",
				 (char *)
				 gtk_entry_get_text(GTK_ENTRY
						    (GTK_COMBO
						     (bannerFrameStartCombo)->
						     entry)),
				 (char *)
				 gtk_entry_get_text(GTK_ENTRY
						    (GTK_COMBO
						     (bannerFrameStopCombo)->
						     entry)));
			num_options =
			    cupsAddOption("job-sheets", tmp, num_options,
					  optionsptr);
		}
	} else {
		if (strcmp(PrinterBannerEnd[PrinterChoice], "none") == 0) {
			if (strcmp(PrinterBannerStart[PrinterChoice], "none") !=
			    0) {
				snprintf(tmp, (size_t) MAXOPTLEN, "%s",
					 PrinterBannerStart[PrinterChoice]);
				num_options =
				    cupsAddOption("job-sheets", tmp,
						  num_options, optionsptr);
			}
		} else {
			snprintf(tmp, (size_t) MAXOPTLEN, "%s,%s",
				 PrinterBannerStart[PrinterChoice],
				 PrinterBannerEnd[PrinterChoice]);
			num_options =
			    cupsAddOption("job-sheets", tmp, num_options,
					  optionsptr);
		}
	}

	if (wantraw == 1)
		num_options = cupsParseOptions("raw", num_options, optionsptr);
	if (ExtraOpt[0] != (uintptr_t) NULL)
		num_options =
		    cupsParseOptions(ExtraOpt, num_options, optionsptr);

	return (num_options);
}

void setPrinterAsDefault(char *printername)
{
	char *instance;
	cups_dest_t *dests;
	cups_dest_t *dest;
	int num_dests;
	int i1;

	num_dests = 0;
	dest = NULL;
	dests = NULL;

	instance = strchr(printername, '/');
	if (instance != NULL)
		*instance = (uintptr_t) NULL;

	num_dests = cupsGetDests(&dests);

	if (instance != 0)
		dest = cupsGetDest(printername, instance + 1, num_dests, dests);
	else
		dest = cupsGetDest(printername, NULL, num_dests, dests);

	if (dest != NULL) {
		for (i1 = 0; i1 < num_dests; i1++)
			dests[i1].is_default = 0;

		dest->is_default = 1;

		cupsSetDests(num_dests, dests);

	}

	if (instance != NULL)
		*instance = '/';

}

int saveOptions(char *printername, int onlyglobal)
{
	int i1;
	int num_options = 0;
	cups_option_t *options;
	cups_option_t **optionsptr = NULL;
	char printpath[MAXPATH + 1], globalpath[MAXPATH + 1];
	FILE *file;
	int num_printer;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	char iprintername[DEF_PRN_LEN + 1];
	char *z1;
	int newinstance = 0;
	char tmp[MAXOPTLEN + 1];

	if (nox == 1)
		return (0);

	if (onlyglobal == 0) {
		options = (cups_option_t *) 0;

		num_printer = 0;
		all_printer = NULL;
		act_printer = NULL;
		instance = NULL;

		optionsptr = &options;
	}

	if (builtin) {
		if (DEBUG)
			printf("Started with -b, don't saving anything !\n");
		return (0);
	}

	if (onlyglobal == 0) {
		z1 = strchr(printername, '/');
		if (z1 != NULL) {
			*z1 = (uintptr_t) NULL;
			snprintf(iprintername, (size_t) DEF_PRN_LEN, "%s\\%s",
				 printername, z1 + 1);
			*z1 = '/';
			z1++;
			z1 = strchr(z1, '/');
			if (z1 != NULL) {
				if (DEBUG)
					printf("ERROR: Wrong printername !\n");
				emergency();
			}
		} else {
			strncpy(iprintername, printername,
				(size_t) DEF_PRN_LEN);
		}
	}

	printpath[0] = (uintptr_t) NULL;
	globalpath[0] = (uintptr_t) NULL;

	if (getenv("HOME") == (char *)NULL) {
		if (DEBUG)
			printf("ERROR: No Home-Variable set !\n");
		quick_message(str2str(_("Error saving user-settings!")), 2);
		return (-1);
	}

	snprintf(printpath, (size_t) MAXPATH, "%s/", confdir);

	/* try to mkdir this */
	i1 = mkdir(printpath, S_IREAD | S_IWRITE | S_IEXEC);
	if (DEBUG & (i1 != 0))
		printf("Creating dir error: %s\n", strerror(errno));

	strncpy(globalpath, printpath, (size_t) MAXPATH);
	strncat(globalpath, GTKLPRC_GLOBAL, (size_t) MAXPATH);

	if (onlyglobal == 0) {
		strncat(printpath, iprintername, (size_t) MAXPATH);

		/* Printer dependent settings */
		if (DEBUG)
			printf("Trying to open %s\n", printpath);
		file = fopen(printpath, "w");
		if (file == (FILE *) NULL) {
			if (DEBUG)
				printf("ERROR: Can't open users rc-file !\n");
			quick_message(str2str(_("Error saving user-settings!")),
				      2);
			return (-1);
		}
		if (DEBUG)
			printf("Saving Defaults for %s\n", printername);
		if (JobName[0] == (uintptr_t) NULL)
			fprintf(file, "-\n");
		else
			fprintf(file, "%s\n", JobName);
		if (ExtraOpt[0] == (uintptr_t) NULL)
			fprintf(file, "-\n");
		else
			fprintf(file, "%s\n", ExtraOpt);
		fprintf(file, "%i %i %i %i %i %i\n", viewable[0], viewable[1],
			viewable[2], viewable[3], viewable[4], viewable[5]);
		if (strlen(UserName) == 0)
			fprintf(file, "%c\n", 255);
		else
			fprintf(file, "%s\n", UserName);
		if (strlen(passWord) == 0)
			fprintf(file, "%c\n", 255);
		else
			fprintf(file, "%s\n", passWord);
		fclose(file);

		num_printer = cupsGetDests(&all_printer);
		instance = strchr(printername, '/');
		if (instance != NULL) {
			*instance = (uintptr_t) NULL;
			if (DEBUG)
				printf("SAVE: Try to get printer: %s/%s\n",
				       printername, instance + 1);
			act_printer =
			    cupsGetDest(printername, instance + 1, num_printer,
					all_printer);
			*instance = '/';
		} else {
			if (DEBUG)
				printf("SAVE: Try to get printer: %s\n",
				       printername);
			act_printer =
			    cupsGetDest(printername, NULL, num_printer,
					all_printer);
		}

		if (act_printer == NULL) {
			if (instance == NULL) {
				if (DEBUG)
					printf
					    ("SAVE: ERROR: Dest unknown (2) !\n");
				emergency();
			} else {	/* New Instance */
				*instance = (uintptr_t) NULL;
				num_printer =
				    cupsAddDest(printername, instance + 1,
						num_printer, &all_printer);
				if (num_printer >= MAX_PRT) {
					quick_message(str2str
						      (_("Too many printers!")),
						      2);
					*instance = '/';
					return (-1);
				}
				act_printer =
				    cupsGetDest(printername, instance + 1,
						num_printer, all_printer);
				if (act_printer == NULL) {
					quick_message(str2str
						      (_
						       ("Unable to add instance!")),
						      2);
					*instance = '/';
					return (-1);
				}
				*instance = '/';
				newinstance = 1;
			}
		}

		num_options = addAllOptions(optionsptr, num_options);

		/* But if we don`t want to save the number of copies on exit... */
		if (saveanz == 0) {
			if (NumberOfCopies > 0) {
				snprintf(tmp, (size_t) MAXOPTLEN, "%i",
					 oldnumcopies);
				num_options =
				    cupsAddOption("copies", tmp, num_options,
						  optionsptr);
			}
		}

		act_printer->num_options = num_options;
		act_printer->options = options;

		cupsSetDests(num_printer, all_printer);

		cupsFreeOptions(num_options, options);

		/* Set Permissions to read/write only by user */
		if (chmod(printpath, S_IRUSR | S_IWUSR) == -1) {
			if (DEBUG)
				printf("Unable to chmod %s: %s\n", printpath,
				       strerror(errno));
		}
	}

	/* Global settings */
	if (DEBUG)
		printf("Trying to open %s\n", globalpath);
	file = fopen(globalpath, "w");
	if (file == (FILE *) NULL) {
		if (DEBUG)
			printf("ERROR: Can't open users globalrc-file !\n");
		quick_message(str2str(_("Error saving user-settings!")), 2);
		return (-1);
	}
	if (DEBUG)
		printf("Saving Global Defaults\n");
	fprintf(file, "%s\n", printername);
	fprintf(file, "%i\n", rememberprinter);
	fprintf(file, "%i\n", remembertab);
	fprintf(file, "%i\n",
		gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)));
	fprintf(file, "%i\n", saveonexit);
	fprintf(file, "%i\n", saveanz);
	fprintf(file, "%i\n", clearfilelist);
	fprintf(file, "%i\n", exitonprint);
	fprintf(file, "%i\n", jobidonexit);
	fprintf(file, "%s\n", BROWSER);
	fprintf(file, "%s\n", HELPURL);
	fprintf(file, "%s\n", GTKLPQCOM);
	fprintf(file, "%i\n", wantconst);

#if GTK_MAJOR_VERSION != 1
	/* save window state if wished */
	gtk_window_get_position(GTK_WINDOW(mainWindow), &mainWindowX,
				&mainWindowY);
	gtk_window_get_size(GTK_WINDOW(mainWindow), &mainWindowWidth,
			    &mainWindowHeight);

	fprintf(file, "%i %i %i %i %i\n", wantSaveSizePos, mainWindowX,
		mainWindowY, mainWindowWidth, mainWindowHeight);
#endif
	fclose(file);

	/* Set Permissions to read/write only by user */
	if (chmod(globalpath, S_IRUSR | S_IWUSR) == -1) {
		if (DEBUG)
			printf("Unable to chmod %s: %s\n", globalpath,
			       strerror(errno));
	}

	if (rememberprinter == 1)
		setPrinterAsDefault(printername);

	return (newinstance);
}

void setDefaults(void)
{
	if (teststdin() == 1)
		needFileSelection = 0;
	fileTabSetDefaults();
	generalTabSetDefaults();
	outputTabSetDefaults();
	textTabSetDefaults();
	imageTabSetDefaults();
	hpgl2TabSetDefaults();
	specialTabSetDefaults();
	gtklpTabSetDefaults();
	ppdTabSetDefaults();
}

int freadline(FILE * file, char *String, unsigned howmuch)
{
	int i1, i2;
	unsigned ct;

	i1 = 0;
	i2 = 0;
	ct = 0;
	while (TRUE) {
		i1 = fgetc(file);
		if ((i1 != EOF) && (i1 != 10)) {
			*String++ = (char)i1;
		} else {
			*String = (uintptr_t) NULL;
			break;
		}
		i2++;
		if (ct < howmuch) {
			ct++;
		} else {
			*String = (uintptr_t) NULL;
			return (i2);
		}

	}
	if (i1 == EOF)
		i2 = EOF;
	return (i2);

}

int getPPDOpts(char *printername)
{
	int num_printer, i1;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	cups_option_t *option;

	all_printer = NULL;
	act_printer = NULL;
	num_printer = 0;

	if (PPDopen == 1) {
		unlink(PPDfilename);
		ppdClose(printerPPD);
		PPDopen = 0;
		printerPPD = NULL;
	}

	num_printer = cupsGetDests(&all_printer);
	instance = strchr(printername, '/');
	if (instance != NULL) {
		*instance = (uintptr_t) NULL;
		if (DEBUG)
			printf("Try to get printer: %s/%s\n", printername,
			       instance + 1);
		act_printer =
		    cupsGetDest(printername, instance + 1, num_printer,
				all_printer);
	} else {
		if (DEBUG)
			printf("Try to get printer: %s\n", printername);
		act_printer =
		    cupsGetDest(printername, NULL, num_printer, all_printer);
	}

	if (act_printer == NULL) {
		if (DEBUG)
			printf("ERROR: Dest unknown (3) !\n");
		emergency();
	}

	if (DEBUG)
		printf("Get PPD-Options for %s\n", printername);

	PPDfilename = (char *)cupsGetPPD(printername);
	if (DEBUG)
		printf("GetPPD for %s from Server %s!\n", printername,
		       PPDfilename);
	if (PPDfilename == (char *)NULL) {
		if (DEBUG)
			printf("PROG-ERROR: Can't get PPD from Server !\n");
		if (hasAskedPWD == 1) {
			if (DEBUG)
				printf("Authorization failed !\n");
			if (instance != NULL)
				*instance = '/';
			return (-2);
		}
		return (0);
	}

	if (DEBUG)
		printf("Get Options from PPD\n");
	printerPPD = ppdOpenFile(PPDfilename);
	if (printerPPD == NULL) {
		if (DEBUG)
			printf("PROG: Error opening PPD-File !\n");
		unlink(PPDfilename);
		if (instance != NULL)
			*instance = '/';
		return (0);
	}

	ppdMarkDefaults(printerPPD);

	for (i1 = num_commandline_opts, option = commandline_opts; i1 > 0;
	     option++, i1--) {
		act_printer->num_options =
		    cupsAddOption(option->name, option->value,
				  act_printer->num_options,
				  &act_printer->options);
	}

	cupsMarkOptions(printerPPD, act_printer->num_options,
			act_printer->options);

	if (DEBUG)
		printf("Number of Groups for this printer: %i\n",
		       printerPPD->num_groups);

	PPDopen = 1;

	if (instance != NULL)
		*instance = '/';
	return (0);
}

void resetButton(GtkWidget * widget, gpointer data)
{
	char tmp[MAX_CPI_DIGITS + MAX_LPI_DIGITS + MAX_CPP_DIGITS + 1];

	if (DEBUG)
		printf("Reset: %s\n", (char *)data);

	if (strcmp((char *)data, "Brightness") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), brightness);
		return;
	}

	if (strcmp((char *)data, "BrightnessDefault") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), 100);
		return;
	}

	if (strcmp((char *)data, "Gamma") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), ggamma);
		return;
	}

	if (strcmp((char *)data, "GammaDefault") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), 1000);
		return;
	}

	if (strcmp((char *)data, "CPI") == 0) {
		snprintf(tmp,
			 (size_t) MAX_CPI_DIGITS + MAX_LPI_DIGITS +
			 MAX_CPP_DIGITS, "%u", cpivalue);
		gtk_entry_set_text(GTK_ENTRY(textCPIField), tmp);
		return;
	}

	if (strcmp((char *)data, "CPIDefault") == 0) {
		gtk_entry_set_text(GTK_ENTRY(textCPIField), "10");
		return;
	}

	if (strcmp((char *)data, "LPI") == 0) {
		snprintf(tmp,
			 (size_t) MAX_CPI_DIGITS + MAX_LPI_DIGITS +
			 MAX_CPP_DIGITS, "%u", lpivalue);
		gtk_entry_set_text(GTK_ENTRY(textLPIField), tmp);
		return;
	}

	if (strcmp((char *)data, "LPIDefault") == 0) {
		gtk_entry_set_text(GTK_ENTRY(textLPIField), "6");
		return;
	}

	if (strcmp((char *)data, "CPP") == 0) {
		snprintf(tmp,
			 (size_t) MAX_CPI_DIGITS + MAX_LPI_DIGITS +
			 MAX_CPP_DIGITS, "%u", cppvalue);
		gtk_entry_set_text(GTK_ENTRY(textCPPField), tmp);
		return;
	}

	if (strcmp((char *)data, "CPPDefault") == 0) {
		gtk_entry_set_text(GTK_ENTRY(textCPPField), "1");
		return;
	}

	if (strcmp((char *)data, "HUE") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(imageHUEAdj), hue);
		return;
	}

	if (strcmp((char *)data, "HUEDefault") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(imageHUEAdj), 0);
		return;
	}

	if (strcmp((char *)data, "Sat") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(imageSatAdj), sat);
		return;
	}

	if (strcmp((char *)data, "SatDefault") == 0) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(imageSatAdj), 100);
		return;
	}

	if (strcmp((char *)data, "HPGL2PenDefault") == 0) {
		gtk_entry_set_text(GTK_ENTRY(hpgl2PenField), "1000");
		return;
	}

	if (DEBUG) {
		printf("PROG-ERROR: Unknown Reset Request: %s !\n",
		       (char *)data);
		emergency();
	}
}

void CreateWidgets(void)
{
	FileTab();
	gtk_widget_show(fileTab);
	GeneralTab();
	gtk_widget_show(generalTab);
	OutputTab();
	gtk_widget_show(outputTab);
	TextTab();
	gtk_widget_show(textTab);
	ImageTab();
	gtk_widget_show(imageTab);
	Hpgl2Tab();
	gtk_widget_show(hpgl2Tab);
	SpecialTab();
	gtk_widget_show(specialTab);
	PPDTab();
	gtk_widget_show(ppdTab);
	GtklpTab();
	gtk_widget_show(gtklpTab);
}

void ShowWidgets(void)
{
	sleep(SLEEPBEFORE);
	if (teststdin() == 1)
		needFileSelection = 0;
	setDefaults();

	/* Tabs */
	if (printerPPD == NULL)
		viewable[5] = 0;
	if (!needFileSelection)
		gtk_widget_hide(fileTab);
	if (!viewable[0])
		gtk_widget_hide(outputTab);
	if (!viewable[1])
		gtk_widget_hide(textTab);
	if (!viewable[2])
		gtk_widget_hide(imageTab);
	if (!viewable[3])
		gtk_widget_hide(hpgl2Tab);
	if (!viewable[4])
		gtk_widget_hide(specialTab);
	if (!viewable[5])
		gtk_widget_hide(ppdTab);

	/* Frames in general */
	gtk_widget_show(printerFrame);
	gtk_widget_show(printerNumCopiesFrame);
	if ((ppdFindOption(printerPPD, "PageSize") != NULL)
	    || (ppdFindOption(printerPPD, "MediaType") != NULL)
	    || (ppdFindOption(printerPPD, "InputSlot") != NULL)
	    || (ppdFindOption(printerPPD, "PageRegion") != NULL))
		gtk_widget_show(printerMediaFrame);
	if (ppdFindOption(printerPPD, "Duplex") != NULL)
		gtk_widget_show(printerDuplexFrame);
	if (strlen(GTKLPQCOM) > 0)
		gtk_widget_show(gtklpqFrame);

	/* Frames in output */
	gtk_widget_show(rangesFrame);
	gtk_widget_show(sheetsFrame);
	gtk_widget_show(brightFrame);
	gtk_widget_show(gammaFrame);

	/* Frames in text */
	gtk_widget_show(textSizesFrame);
	gtk_widget_show(textMarginsFrame);
	gtk_widget_show(textPrettyFrame);

	/* Frames in image */
	gtk_widget_show(imagePosFrame);
	gtk_widget_show(imageScalingAllFrame);
	gtk_widget_show(imageHUEFrame);
	gtk_widget_show(imageSatFrame);

	/* Frames in hpgl2 */
	gtk_widget_show(hpgl2OptFrame);
	gtk_widget_show(hpgl2PenFrame);

	/* Frames in special */
	gtk_widget_show(bannerFrame);
	gtk_widget_show(jobNameFrame);
	gtk_widget_show(specialOptFrame);
	gtk_widget_show(extraOptFrame);
	gtk_widget_show(passFrame);

	/* Frames in gtklp */
	gtk_widget_show(gtklpViewable);
	gtk_widget_show(prefsFrame);
	gtk_widget_show(pathesFrame);
	gtk_widget_show(helpFrame);

	/* Frames in file */
	gtk_widget_show(fileFrame);

	/* Frames in ppd */

	/* Main */
	gtk_widget_hide(mainWindow);
	gtk_widget_set_size_request(mainWindow, -1, -1);
	gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
	while (gtk_events_pending())
		gtk_main_iteration();

	gtk_widget_queue_draw(GTK_WIDGET(mainWindow));
	//gtk_widget_show (mainWindow);

}

void gtklp_end(void)
{
	char tmp[1];
	if (teststdin()) {
		while (fread(tmp, 1, sizeof(tmp), stdin) > 0) ;
	}

	if (saveonexit) {
		saveOptions(PrinterNames[PrinterChoice], 0);
	} else {
		if (rememberprinter == 1)
			saveOptions(PrinterNames[PrinterChoice], 1);
	}

	cupsLangFlush();
	cupsLanguage = NULL;
	httpClose(cupsHttp);
	cupsHttp = NULL;
	if (nox == 0)
		gtk_main_quit();
}

void AbbruchFunc(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		g_print("Abort !\n");
	gtklp_end();
}

void destroy(GtkWidget * widget, gpointer data)
{
	gtklp_end();
}

void SaveFunc(GtkWidget * widget, gpointer instance)
{
	char tmp[DEF_PRN_LEN + 1];
	char *z1;
	int i1, so;
	GList *printerList = NULL;
	int merk;

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;

	if (strlen((char *)gtk_entry_get_text(GTK_ENTRY(instance))) != 0) {
		snprintf(tmp, (size_t) DEF_PRN_LEN, "%s/%s",
			 PrinterNames[PrinterChoice],
			 (char *)gtk_entry_get_text(GTK_ENTRY(instance)));
		if (z1 != NULL)
			*z1 = '/';

		so = saveOptions(tmp, 0);
		if (so == 1) {
			getPrinters(0);
			merk = 0;
			for (i1 = 0; i1 <= PrinterNum; i1++) {
				printerList =
				    g_list_append(printerList,
						  str2str(PrinterNames[i1]));
				if (strcmp(PrinterNames[i1], tmp) == 0)
					merk = i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO
						      (printerFrameCombo),
						      printerList);
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(printerFrameCombo)->
					      list), merk);
		}
		if (so >= 0) {
			quick_message(str2str(_("Options saved!")), 1);
			gtk_widget_destroy(instwin);
		}
	} else {
		if (saveOptions(PrinterNames[PrinterChoice], 0) >= 0) {
			quick_message(str2str(_("Options saved!")), 1);
			gtk_widget_destroy(instwin);
		}
		if (z1 != NULL)
			*z1 = '/';
	}

}

void DelInstFunc(GtkWidget * widget, gpointer instance)
{
	char *z1;
	int num_dests, i1, merk;
	cups_dest_t *dests;
	cups_dest_t *dest;
	GList *printerList = NULL;
	char tmp[DEF_PRN_LEN + 1];
	char printpath[MAXPATH + 1];

	num_dests = 0;
	dests = NULL;
	dest = NULL;

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;
	strncpy(tmp, PrinterNames[PrinterChoice], (size_t) DEF_PRN_LEN);
	if (z1 != NULL)
		*z1 = '/';

	num_dests = cupsGetDests(&dests);
	dest =
	    cupsGetDest(tmp, (char *)gtk_entry_get_text(GTK_ENTRY(instance)),
			num_dests, dests);

	if (dest != NULL) {
		cupsFreeOptions(dest->num_options, dest->options);
		num_dests--;

		i1 = dest - dests;
		if (i1 < num_dests)
			memcpy(dest, dest + 1,
			       (num_dests - i1) * sizeof(cups_dest_t));
		cupsSetDests(num_dests, dests);
		dest = NULL;

		getPrinters(0);
		merk = 0;
		for (i1 = 0; i1 <= PrinterNum; i1++) {
			printerList =
			    g_list_append(printerList,
					  str2str(PrinterNames[i1]));
			if (strcmp(PrinterNames[i1], tmp) == 0)
				merk = i1;
		}
		gtk_combo_set_popdown_strings(GTK_COMBO(printerFrameCombo),
					      printerList);
		gtk_list_select_item(GTK_LIST
				     (GTK_COMBO(printerFrameCombo)->list),
				     merk);

		/* unlink $HOME/.gtklp/printer */
		if (strlen((char *)gtk_entry_get_text(GTK_ENTRY(instance))) != 0) {	/* Only Instances could be deleted */
			snprintf(printpath, (size_t) MAXPATH, "%s/%s/%s\\%s",
				 getenv("HOME"), GTKLPRC_USER, tmp,
				 (char *)
				 gtk_entry_get_text(GTK_ENTRY(instance)));
			unlink(printpath);
		}

		quick_message(str2str(_("Instance removed!")), 1);
		gtk_widget_destroy(instwin);
	}
}

void ResetFunc(GtkWidget * widget, gpointer data)
{
	getOptions(PrinterNames[PrinterChoice], 2);
	PrinterChanged(NULL, PrinterNames[PrinterChoice]);
	quick_message(str2str(_("Saved Defaults loaded!")), 1);
}

void clearFileList(void)
{
	if (DEBUG)
		printf("Clear file list !\n");
	filesToPrintAnz = 0;
	filesToPrint[0][0] = (uintptr_t) NULL;
	if (nox == 0)
		gtk_clist_clear(GTK_CLIST(fileList));
}

void PrintFunc(GtkWidget * widget, gpointer data)
{
	FILE *spool;
	char tmp[MAXOPTLEN + 1];
	char tmppath[MAXPATH + 1];
	char *z1;
	int i1;
	int num_options, jobid;
	cups_option_t *options;
	cups_option_t **optionsptr;
	const char *files[MAXPRINTFILES];
	char tmpprn[DEF_PRN_LEN + 1];
	char *spoolfile;

	num_options = 0;
	options = (cups_option_t *) 0;
	optionsptr = &options;

	num_options = addAllOptions(optionsptr, num_options);

	snprintf(tmpprn, (size_t) DEF_PRN_LEN, "%s",
		 PrinterNames[PrinterChoice]);
	z1 = strchr(tmpprn, '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;

	if ((ReallyPrint == 1) && (teststdin() == 0)) {
		if (filesToPrintAnz > 0) {
			for (i1 = 0; i1 < filesToPrintAnz; i1++)
				files[i1] =
				    (char *)g_filename_to_utf8((gchar *)
							       filesToPrint[i1],
							       -1, NULL, NULL,
							       NULL);

			if (DEBUG)
				printf
				    ("NumOpts-final(printing from file): %i\n",
				     num_options);
			if (JobName[0] != (uintptr_t) NULL) {
				jobid =
				    cupsPrintFiles(tmpprn, filesToPrintAnz,
						   files, JobName, num_options,
						   options);
			} else {
				for (i1 = 0; i1 < filesToPrintAnz; i1++) {
					z1 = strrchr(files[i1], '/');
					if (z1 == (char *)NULL)
						z1 = (char *)files[i1];
					else
						z1++;
					jobid =
					    cupsPrintFile(tmpprn, files[i1], z1,
							  num_options, options);

					if (jobid <= 0)
						g_print("\n%s\n\n",
							str2str(_
								("Unable to print,\nunknown file format!")));
					if (DEBUG)
						printf("Job-ID: %i\n", jobid);
					if (jobidonexit)
						g_print("%s: %i\n",
							str2str(_("JobID")),
							jobid);
				}
			}
			cupsFreeOptions(num_options, options);
			for (i1 = 0; i1 < filesToPrintAnz; i1++)
				g_free((char *)files[i1]);
		} else {
			quick_message(str2str(_("Dont know what to print!")),
				      2);
			return;
		}
		if (exitonprint || (needFileSelection == 0))
			gtklp_end();
		if (clearfilelist)
			clearFileList();
	}

	if ((ReallyPrint == 1) && (teststdin() == 1)) {
		if (DEBUG)
			printf("NumOpts-final(printing from stdin): %i\n",
			       num_options);

		spoolfile = (char *)cupsTempFile2(tmppath, sizeof(tmppath));

		if (spoolfile == (char *)NULL) {
			if (DEBUG)
				printf("Cannot create temporary spool file !");
			g_print("\n%s\n\n", str2str(_("Unable to print!")));
		} else {
			if (DEBUG)
				printf("Spoolfile: %s\n", tmppath);
			spool = fopen(tmppath, "w");
			if (spool == (FILE *) NULL) {
				if (DEBUG)
					printf("Unable to open Spoolfile !\n");
				g_print("\n%s\n\n",
					str2str(_("Unable to print!")));
			} else {
				while ((i1 =
					fread(tmp, 1, sizeof(tmp), stdin)) > 0)
					fwrite(tmp, 1, i1, spool);
				i1 = ftell(spool);
				fclose(spool);
				if (i1 == 0) {
					if (DEBUG)
						printf
						    ("PROG-ERROR: Data on stdin, but empty...\n");
					emergency();
				}

				if (JobName[0] != (uintptr_t) NULL)
					jobid =
					    cupsPrintFile(tmpprn, tmppath,
							  JobName, num_options,
							  options);
				else
					jobid =
					    cupsPrintFile(tmpprn, tmppath,
							  str2str(_("(stdin)")),
							  num_options, options);
				if (jobid <= 0)
					g_print("\n%s\n\n",
						str2str(_("Unable to print!")));
				if (DEBUG)
					printf("Job-ID: %i\n", jobid);
				if (jobidonexit)
					g_print("%s: %i\n", str2str(_("JobID")),
						jobid);

				cupsFreeOptions(num_options, options);
				unlink(tmppath);

				if (exitonprint || (needFileSelection == 0))
					gtklp_end();
				if (clearfilelist)
					clearFileList();
			}
		}
	}

	if (ReallyPrint == 0) {	/* Not really Print, only list ! */
		for (i1 = num_options - 1; i1 >= 0; i1--) {
			g_print(" -o %s=%s", options[i1].name,
				options[i1].value);
		}
		g_print(" -P%s", PrinterNames[PrinterChoice]);
		if (JobName[0] != (uintptr_t) NULL)
			g_print(" -J \"%s\"", JobName);
		if (filesToPrintAnz > 0) {
			for (i1 = 0; i1 < filesToPrintAnz; i1++)
				g_print(" %s", filesToPrint[i1]);
		}
		cupsFreeOptions(num_options, options);
		if (exitonprint || (needFileSelection == 0))
			gtklp_end();
		if (clearfilelist)
			clearFileList();
	}
}

void printPPDoptsHelp(ppd_group_t * group)
{
	int i1, i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for (i1 = group->num_options, option = group->options; i1 > 0;
	     i1--, option++) {
		for (i2 = option->num_choices, choice = option->choices; i2 > 0;
		     i2--, choice++) {
			if (choice->marked) {
				g_print("[%s=%s] ", option->keyword,
					choice->choice);
			} else {
				g_print("%s ", choice->choice);
			}
			if (option->conflicted)
				g_print("CONFLICT: %s %s \n", option->text,
					choice->text);
		}
		g_print("\n");
	}

	for (i1 = group->num_subgroups, subgroup = group->subgroups; i1 > 0;
	     i1--, subgroup++)
		printPPDoptsHelp(subgroup);

}

void printPPDopts(void)
{
	int i1;
	ppd_group_t *group;

	for (i1 = printerPPD->num_groups, group = printerPPD->groups; i1 > 0;
	     i1--, group++)
		printPPDoptsHelp(group);

	g_print("\n");

}

void constraints(int prob)
{
	int i, j, k, count;
	ppd_const_t *c;
	ppd_group_t *g, *sg;
	ppd_option_t *o1, *o2;
	ppd_choice_t *c1, *c2;
	GtkWidget *constwin;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *conflictimage;
	GtkWidget *rightimage;
	GdkPixmap *conflictpixmap;
	GdkPixmap *rightpixmap;
	GdkBitmap *mask;
	GtkStyle *style;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *ruler;
	GtkWidget *buttonbox;
#endif
	if ((prob <= 0) || (printerPPD == NULL) || (conflict_active == 0)
	    || (wantconst == 0))
		return;

	count = 0;

#if GTK_MAJOR_VERSION == 1
	constwin = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(constwin), str2str(_("Conflicts")));
	gtk_window_set_policy(GTK_WINDOW(constwin), FALSE, FALSE, TRUE);
	gtk_window_set_modal(GTK_WINDOW(constwin), TRUE);
	gtk_window_set_position(GTK_WINDOW(constwin), GTK_WIN_POS_MOUSE);
#else
	constwin =
	    gtk_dialog_new_with_buttons(str2str(_("Conflicts")),
					GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(constwin), 0);
#endif

#if GTK_MAJOR_VERSION == 1
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(constwin), hbox);
	gtk_widget_show(hbox);
#else
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(constwin)->vbox), hbox);
	gtk_widget_show(hbox);
#endif

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	style = gtk_widget_get_style(vbox);
	conflictpixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_conflict);
	rightpixmap =
	    gdk_pixmap_create_from_xpm_d(mainWindow->window, &mask,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) pixmap_rarrow);

	table = gtk_table_new(3, 7, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(table);

#if GTK_MAJOR_VERSION == 1
	ruler = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), ruler, FALSE, FALSE, 0);
	gtk_widget_show(ruler);

	buttonbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), buttonbox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(buttonbox);

	button = gtk_button_new_with_label(str2str(_("Ok")));
	button_pad(button);
	gtk_signal_connect_object(GTK_OBJECT(button), "pressed",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  (gpointer) constwin);
	gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  (gpointer) constwin);
	gtk_box_pack_start(GTK_BOX(buttonbox), button, TRUE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_grab_focus(GTK_WIDGET(button));
	gtk_widget_show(button);
#endif

	for (i = printerPPD->num_groups, g = printerPPD->groups; i > 0;
	     i--, g++) {
		for (j = g->num_options, o1 = g->options; j > 0; j--, o1++)
			o1->conflicted = 0;
		for (j = g->num_subgroups, sg = g->subgroups; j > 0; j--, sg++) {
			for (k = sg->num_options, o1 = sg->options; k > 0;
			     k--, o1++)
				o1->conflicted = 0;
		}
	}

	for (i = printerPPD->num_consts, c = printerPPD->consts; i > 0;
	     i--, c++) {
		o1 = ppdFindOption(printerPPD, c->option1);
		if (o1 == NULL)
			continue;
		else if (c->choice1[0] != '\0') {
			c1 = ppdFindChoice(o1, c->choice1);
		} else {
			for (j = o1->num_choices, c1 = o1->choices; j > 0;
			     j--, c1++) {
				if (c1->marked)
					break;
			}
			if (j == 0 || strcasecmp(c1->choice, "none") == 0
			    || strcasecmp(c1->choice, "off") == 0
			    || strcasecmp(c1->choice, "false") == 0)
				c1 = NULL;
		}

		o2 = ppdFindOption(printerPPD, c->option2);
		if (o2 == NULL)
			continue;
		else if (c->choice2[0] != '\0') {
			c2 = ppdFindChoice(o2, c->choice2);
		} else {
			for (j = o2->num_choices, c2 = o2->choices; j > 0;
			     j--, c2++) {
				if (c2->marked)
					break;
			}
			if (j == 0 || strcasecmp(c2->choice, "none") == 0
			    || strcasecmp(c2->choice, "off") == 0
			    || strcasecmp(c2->choice, "false") == 0)
				c2 = NULL;
		}

		if ((c1 != NULL) && (c1->marked) && (c2 != NULL)
		    && (c2->marked)) {
			gtk_table_resize(GTK_TABLE(table), count + 1, 7);

			label = gtk_label_new(str2str(o1->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 0, 1, count,
					 count + 1, 0, 0, 0, 0);

			rightimage = gtk_pixmap_new(rightpixmap, mask);
			gtk_widget_show(rightimage);
			gtk_table_attach(GTK_TABLE(table), rightimage, 1, 2,
					 count, count + 1, 0, 0,
					 INFRAME_SPACING_H, 0);

			label = gtk_label_new(str2str(c1->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 2, 3, count,
					 count + 1, 0, 0, 0, 0);

			conflictimage = gtk_pixmap_new(conflictpixmap, mask);
			gtk_widget_show(conflictimage);
			gtk_table_attach(GTK_TABLE(table), conflictimage, 3, 4,
					 count, count + 1, 0, 0,
					 INFRAME_SPACING_H, 0);

			label = gtk_label_new(str2str(o2->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 4, 5, count,
					 count + 1, 0, 0, 0, 0);

			rightimage = gtk_pixmap_new(rightpixmap, mask);
			gtk_widget_show(rightimage);
			gtk_table_attach(GTK_TABLE(table), rightimage, 5, 6,
					 count, count + 1, 0, 0,
					 INFRAME_SPACING_H, 0);

			label = gtk_label_new(str2str(c2->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 6, 7, count,
					 count + 1, 0, 0, 0, 0);

			o1->conflicted = 1;
			o2->conflicted = 1;
			count++;
		}
	}

#if GTK_MAJOR_VERSION == 1
	gtk_widget_show(constwin);
#else
	gtk_dialog_run(GTK_DIALOG(constwin));
	gtk_widget_destroy(constwin);
#endif

}

void parseCustomSize(void)
{
	char *z1, *z2;
	unsigned char c1;

	if (strncasecmp(CustomPageSize, "Custom.", 7) != 0) {
		return;		/* no CustomPageSize to parse */
	}

	z1 = CustomPageSize;
	z1 += 7;

	z2 = strchr(z1, 'x');
	if (z2 == NULL) {
		z2 = strchr(z1, 'X');
		if (z2 == NULL) {
			printf("NOPE\n");
			return;	/* no CustomPageSize to parse */
		}
	}
	c1 = *z2;
	*z2 = (uintptr_t) NULL;
	gtk_entry_set_text(GTK_ENTRY(customSizeFieldX), z1);
	*z2 = c1;

	z1 = z2 + 1;
	z2 = strcasestr(z1, "pt");
	if (z2 == NULL) {
		z2 = strcasestr(z1, "in");
		if (z2 == NULL) {
			z2 = strcasestr(z1, "cm");
			if (z2 == NULL) {
				z2 = strcasestr(z1, "mm");
				if (z2 == NULL) {
					z2 = z1 + strlen(z1) - 1;
				} else {
					gtk_list_select_item(GTK_LIST
							     (GTK_COMBO
							      (customSizeCombo)->
							      list), 3);
				}
			} else {
				gtk_list_select_item(GTK_LIST
						     (GTK_COMBO
						      (customSizeCombo)->list),
						     2);
			}
		} else {
			gtk_list_select_item(GTK_LIST
					     (GTK_COMBO(customSizeCombo)->list),
					     1);
		}
	} else {
		gtk_list_select_item(GTK_LIST(GTK_COMBO(customSizeCombo)->list),
				     0);
	}
	c1 = *z2;
	*z2 = (uintptr_t) NULL;
	gtk_entry_set_text(GTK_ENTRY(customSizeFieldY), z1);
	*z2 = c1;
}

void PPD_DropDown_changed(GtkWidget * widget, gpointer data)
{
	ppd_choice_t *choice;
	ppd_option_t *option;
	int i1;

	if (DEBUG)
		printf("PPD Option %s changed to: ", (char *)data);

	option = ppdFindOption(printerPPD, (const char *)data);

	for (i1 = option->num_choices, choice = option->choices; i1 > 0;
	     i1--, choice++) {
		if (strcmp
		    (str2str(choice->text),
		     gtk_entry_get_text(GTK_ENTRY(widget))) == 0) {
			constraints(ppdMarkOption
				    (printerPPD, option->keyword,
				     choice->choice));
			i1 = 0;
			if (DEBUG)
				printf("%s", choice->text);

			if (strcmp(str2str(option->keyword), "PageSize") == 0) {
				if (strcmp(str2str(choice->choice), "Custom") ==
				    0) {
					wantCustomPageSize = 1;
					parseCustomSize();
					gtk_widget_show(CustomMediaSizeFrame);
				} else {
					wantCustomPageSize = 0;
					gtk_widget_hide(CustomMediaSizeFrame);
				}

			}

		}
	}
	if (DEBUG)
		printf(".\n");
}

void SaveGtkLPPrefs(GtkWidget * widget, gpointer date)
{
	if (saveOptions(PrinterNames[PrinterChoice], 1) >= 0)
		quick_message(str2str(_("Options saved!")), 1);
}

void instWinEntryChanged(GtkWidget * widget, gpointer data)
{
	int i1;
	char tmp[DEF_PRN_LEN + 1];
	char *z1;

	if (strlen((char *)gtk_entry_get_text(GTK_ENTRY(widget))) == 0) {
#if GTK_MAJOR_VERSION == 1
		gtk_label_set_text(GTK_LABEL(button_inst_save_label),
				   str2str(_("Save")));
#else
		gtk_button_set_label(GTK_BUTTON(button_inst_save),
				     GTK_STOCK_SAVE);
#endif
		gtk_widget_set_sensitive((GtkWidget *) button_inst_remove,
					 FALSE);
		return;
	}

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;
	snprintf(tmp, (size_t) DEF_PRN_LEN, "%s/%s",
		 PrinterNames[PrinterChoice],
		 (char *)gtk_entry_get_text(GTK_ENTRY(widget)));
	if (z1 != NULL)
		*z1 = '/';

	for (i1 = 0; i1 <= PrinterNum; i1++) {
		if (strcmp(tmp, PrinterNames[i1]) == 0) {
#if GTK_MAJOR_VERSION == 1
			gtk_label_set_text(GTK_LABEL(button_inst_save_label),
					   str2str(_("Save")));
#else
			gtk_button_set_label(GTK_BUTTON(button_inst_save),
					     GTK_STOCK_SAVE);
#endif
			gtk_widget_set_sensitive((GtkWidget *)
						 button_inst_remove, TRUE);
			return;
		}
	}

#if GTK_MAJOR_VERSION == 1
	gtk_label_set_text(GTK_LABEL(button_inst_save_label),
			   str2str(_("New")));
#else
	gtk_button_set_label(GTK_BUTTON(button_inst_save), GTK_STOCK_ADD);
#endif
	gtk_widget_set_sensitive((GtkWidget *) button_inst_remove, FALSE);

}

#if GTK_MAJOR_VERSION != 1
void instwin_response_ok(GtkEditable * editable, gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(instwin), 1);
}
#endif

void instWin(GtkWidget * widget, gpointer data)
{
	GtkWidget *hbox;
	GtkWidget *vbox;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *ruler;
	GtkWidget *buttonbox;
	GtkWidget *button_inst_save;
#endif
	GtkWidget *printer;
	GtkWidget *instance;
	GtkWidget *entrystable;
	GtkWidget *labelinst;
	GtkWidget *labelprt;
	char *z1;

#if GTK_MAJOR_VERSION == 1
	instwin = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(instwin),
			     str2str(_("Printer Templates")));
	gtk_window_set_policy(GTK_WINDOW(instwin), FALSE, FALSE, TRUE);
	gtk_window_set_modal(GTK_WINDOW(instwin), TRUE);
	gtk_window_set_position(GTK_WINDOW(instwin), GTK_WIN_POS_MOUSE);
#else
	instwin =
	    gtk_dialog_new_with_buttons(str2str(_("Printer Templates")),
					GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
#endif

#if GTK_MAJOR_VERSION == 1
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(instwin), hbox);
	gtk_widget_show(hbox);
#else
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(instwin)->vbox), hbox);
	button_inst_save =
	    gtk_dialog_add_button(GTK_DIALOG(instwin), GTK_STOCK_SAVE, 1);
	button_inst_remove =
	    gtk_dialog_add_button(GTK_DIALOG(instwin), GTK_STOCK_REMOVE, 2);
	gtk_dialog_set_default_response(GTK_DIALOG(instwin), 1);
	gtk_widget_show(hbox);
#endif

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	entrystable = gtk_table_new(2, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), entrystable, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(entrystable);

	labelprt = gtk_label_new(str2str(_("Printer: ")));
	gtk_table_attach(GTK_TABLE(entrystable), labelprt, 0, 1, 0, 1, 0, 0, 0,
			 0);
	gtk_widget_show(labelprt);

	labelinst = gtk_label_new(str2str(_("Instance: ")));
	gtk_table_attach(GTK_TABLE(entrystable), labelinst, 0, 1, 1, 2, 0, 0, 0,
			 0);
	gtk_widget_show(labelinst);

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if (z1 != NULL)
		*z1 = (uintptr_t) NULL;

	printer = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(printer), PrinterNames[PrinterChoice]);
	gtk_entry_set_editable(GTK_ENTRY(printer), FALSE);
	gtk_table_attach(GTK_TABLE(entrystable), printer, 1, 2, 0, 1, 0, 0, 0,
			 0);

	gtk_widget_show(printer);

	instance = gtk_entry_new();
	if (z1 != NULL)
		gtk_entry_set_text(GTK_ENTRY(instance), str2str(z1 + 1));
	else
		gtk_entry_set_text(GTK_ENTRY(instance), "");
	gtk_entry_set_editable(GTK_ENTRY(instance), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(instance), DEF_PRN_LEN);
	gtk_signal_connect(GTK_OBJECT(instance), "changed",
			   GTK_SIGNAL_FUNC(instWinEntryChanged), NULL);
	gtk_table_attach(GTK_TABLE(entrystable), instance, 1, 2, 1, 2, 0, 0, 0,
			 0);
	gtk_widget_show(instance);

	if (z1 != NULL)
		*z1 = '/';

#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect_object(GTK_OBJECT(printer), "activate",
				  GTK_SIGNAL_FUNC(SaveFunc),
				  GTK_OBJECT(instance));

	ruler = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), ruler, FALSE, FALSE, 0);
	gtk_widget_show(ruler);

	buttonbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), buttonbox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(buttonbox);

	button_inst_save = gtk_button_new();
	gtk_signal_connect(GTK_OBJECT(button_inst_save), "clicked",
			   GTK_SIGNAL_FUNC(SaveFunc), GTK_OBJECT(instance));
	gtk_box_pack_start(GTK_BOX(buttonbox), button_inst_save, TRUE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(button_inst_save);
	button_inst_save_label = gtk_label_new(str2str(_("Save")));
	gtk_container_add(GTK_CONTAINER(button_inst_save),
			  button_inst_save_label);
	gtk_widget_show(button_inst_save_label);

	button_inst_remove = gtk_button_new_with_label(str2str(_("Remove")));
	button_pad(button_inst_remove);
	gtk_signal_connect(GTK_OBJECT(button_inst_remove), "clicked",
			   GTK_SIGNAL_FUNC(DelInstFunc), GTK_OBJECT(instance));
	gtk_box_pack_start(GTK_BOX(buttonbox), button_inst_remove, TRUE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(button_inst_remove);

	button_inst_cancel = gtk_button_new_with_label(str2str(_("Cancel")));
	button_pad(button_inst_cancel);
	gtk_signal_connect_object(GTK_OBJECT(button_inst_cancel), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(instwin));
	gtk_box_pack_end(GTK_BOX(buttonbox), button_inst_cancel, TRUE, FALSE,
			 INFRAME_SPACING_V);
	gtk_widget_show(button_inst_cancel);

#endif

	gtk_widget_grab_focus(GTK_WIDGET(button_inst_save));

	gtk_signal_emit_by_name(GTK_OBJECT(instance), "changed", NULL);

#if GTK_MAJOR_VERSION == 1
	gtk_widget_show(instwin);
#else
	switch (gtk_dialog_run(GTK_DIALOG(instwin))) {
	case 1:		/* Save */
		SaveFunc(GTK_WIDGET(instwin), GTK_OBJECT(instance));
		break;
	case 2:		/* Remove */
		DelInstFunc(GTK_WIDGET(instwin), GTK_OBJECT(instance));
		break;
	default:
		gtk_widget_destroy(instwin);
		break;
	}
#endif

}
