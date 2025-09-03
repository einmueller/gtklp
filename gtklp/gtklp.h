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

#ifndef GTKLP_H
#define GTKLP_H

#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/http.h>

#include <defaults.h>

extern char BROWSER[MAXPATH + 1];
extern char ExtraOpt[MAX_EXTRAOPT + 1];
extern char GTKLPRC[MAXPATH + 1];
extern char HELPURL[MAXPATH + 1];
extern char GTKLPQCOM[MAXPATH + 1];
extern char JobName[MAX_JOB_NAME_LENGTH + 1];
extern char CustomPageSize[2 * MAX_CUSTOM_SIZE_LENGTH + 2];
extern char Range[MAXRANGEENTRY + 1];
extern char confdir[MAXPATH + 1];
extern char fileOpenPath[MAXPATH + 1];
extern char filesToPrint[MAXPRINTFILES][MAXPATH + 1];
extern char filesToPrintAnz;
extern char *PPDfilename;
extern unsigned int cppvalue, lpivalue, cpivalue;
extern unsigned int hpgl2pen;
extern unsigned int topmargin, bottommargin, leftmargin, rightmargin;
extern int NumberOfCopies;
extern int collate;
extern int MediaSizeSelected;
extern int MediaTypeSelected;
extern int MediaSourceSelected;
extern int PPDopen;
extern int ReallyPrint;
extern int builtin;
extern int conflict_active;
extern int clearfilelist;
extern int cmdNumCopies;
extern int exitonprint;
extern int saveanz;
extern int oldnumcopies;
extern int hasAskedPWD;
extern int hpgl2black;
extern int hpgl2fit;
extern int ignore_stdin;
extern int imageScalingType;
extern int jobidonexit;
extern int needFileSelection;
extern int nox;
extern int num_commandline_opts;
extern int prettyprint;
extern int rangeType;
extern int rememberprinter;
extern int remembertab;
extern int saveonexit;
extern int mirroroutput;
extern int sheetsType;
extern int tabtoopen;
extern int wantconst;
extern int wantLandscape;
extern int wantCustomPageSize;
extern int wantRange;
extern int wantReverseOutputOrder;
extern int wantraw;
extern int ImagePos[2];
extern int viewable[6];
extern double brightness;
extern double ggamma;
extern double hue;
extern double imageScalingScalingValue;
extern double imageScalingNaturalValue;
extern double imageScalingPPIValue;
extern double sat;
extern int textwrap;
#if GTK_MAJOR_VERSION != 1
extern int mainWindowHeight, mainWindowWidth, mainWindowX, mainWindowY,
    wantSaveSizePos;
#endif

extern cups_option_t *commandline_opts;
extern ppd_file_t *printerPPD;

extern GtkWidget *tabs;

extern GtkWidget *dialog;

#endif				/* GTKLP_H */
