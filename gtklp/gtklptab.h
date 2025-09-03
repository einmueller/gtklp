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

#ifndef GTKLPTAB_H
#define GTKLPTAB_H

#include <gtk/gtk.h>

extern GtkWidget *gtklpTab;
extern GtkWidget *gtklpViewable;
extern GtkWidget *gtklpViewOut;
extern GtkWidget *gtklpViewText;
extern GtkWidget *gtklpViewImage;
extern GtkWidget *gtklpViewHpgl2;
extern GtkWidget *gtklpViewSpecial;
extern GtkWidget *gtklpViewPPD;
extern GtkWidget *prefsFrame;
extern GtkWidget *remPrinterCheck;
extern GtkWidget *remTabCheck;
extern GtkWidget *saveExitCheck;
extern GtkWidget *jobIDOnExitCheck;
extern GtkWidget *wantconstCheck;
extern GtkWidget *pathesFrame;
extern GtkWidget *helpFrame;
extern GtkWidget *aboutWin;
extern GtkWidget *clearFileListCheck;
extern GtkWidget *exitOnPrintCheck;
extern GtkWidget *browserCom;
extern GtkWidget *helpURL;
extern GtkWidget *gtklpqCom;
extern GtkWidget *buttonCons;
extern GtkWidget *gtklpqFrame;

void gtklpTabSetDefaults(void);
void gtklpViewChanged(GtkWidget * widget, gpointer data);
void GtklpViewable(GtkWidget * container);
void prefsChanged(GtkWidget * widget, gpointer data);
void constButton(GtkWidget * widget, gpointer data);
void PrefsFrame(GtkWidget * container);
void visitIt(char *what);
void visitMyHome(GtkWidget * widget, gpointer data);
void visitCupsHome(GtkWidget * widget, gpointer data);
void destroyAbout(GtkWidget * widget, gpointer data);
void showAbout(GtkWidget * widget, gpointer data);
void showHTMLHelp(GtkWidget * widget, gpointer data);
void HelpFrame(GtkWidget * container);
void PathesEntryChanged(GtkWidget * widget, gpointer data);
void PathesFrame(GtkWidget * container);
GtkWidget *GtklpTabFrame(GtkWidget * container);
void GtklpTabButtons(GtkWidget * container);
void GtklpTab(void);

#endif				/* GTKLPTAB_H */
