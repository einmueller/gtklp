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

#ifndef GTKLP_FUNCTIONS_H
#define GTKLP_FUNCTIONS_H

#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/ppd.h>

extern char nupLayout[5];

int teststdin(void);
void getPaths(void);
void getBannerList(char *printername);
void getOptions(char *printername, int which);
int addAllOptionsHelp(ppd_group_t * group, cups_option_t ** optionsptr,
		      int num_options);
int addAllOptions(cups_option_t ** optionsptr, int num_options);
void setPrinterAsDefault(char *printername);
int saveOptions(char *printername, int onlyglobal);
void setDefaults(void);
int freadline(FILE * file, char *String, unsigned howmuch);
int getPPDOpts(char *printername);
void resetButton(GtkWidget * widget, gpointer data);
void CreateWidgets(void);
void ShowWidgets(void);
void gtklp_end(void);
void AbbruchFunc(GtkWidget * widget, gpointer data);
void destroy(GtkWidget * widget, gpointer data);
void SaveFunc(GtkWidget * widget, gpointer instance);
void DelInstFunc(GtkWidget * widget, gpointer instance);
void ResetFunc(GtkWidget * widget, gpointer data);
void clearFileList(void);
void PrintFunc(GtkWidget * widget, gpointer data);
void printPPDoptsHelp(ppd_group_t * group);
void printPPDopts(void);
void constraints(int prob);
void PPD_DropDown_changed(GtkWidget * widget, gpointer data);
void SaveGtkLPPrefs(GtkWidget * widget, gpointer date);
#if GTK_MAJOR_VERSION != 1
void instwin_response_ok(GtkEditable * editable, gpointer data);
#endif
void instWinEntryChanged(GtkWidget * widget, gpointer data);
void instWin(GtkWidget * widget, gpointer data);

#endif				/* GTKLP_FUNCTIONS_H */
