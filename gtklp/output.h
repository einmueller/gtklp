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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>

extern GSList *rangeButtons;
extern GSList *sheetsButtons;
extern GtkWidget *outputTab;
extern GtkWidget *rangesFrame;
extern GtkWidget *rangeCheck;
extern GtkWidget *rangeText;
extern GtkWidget *sheetsFrame;
extern GtkWidget *landscapeCheck;
extern GtkWidget *brightFrame;
extern GtkWidget *gammaFrame;
extern GtkObject *brightAdj;
extern GtkObject *gammaAdj;
extern char nupLayout[5];
extern char borderType[13];
extern GtkWidget *mirrorCheck;
extern GtkWidget *outputOrderCheck;

void outputTabSetDefaults(void);
void rangeCheckChanged(GtkWidget * widget, gpointer data);
void outputOrderCheckChanged(GtkWidget * widget, gpointer data);
void rangeTextChanged(GtkWidget * widget, gpointer data);
void rangeButtonsChanged(GtkWidget * widget, gpointer data);
void sheetsChanged(GtkWidget * widget, gpointer data);
void landscapeCheckChanged(GtkWidget * widget, gpointer data);
void nupStylePix(int i1);
void nupStyleChange(GtkWidget * widget, gpointer data);
void wantMirrorCheck(GtkWidget * widget, gpointer data);
void sheetUsageFrame(GtkWidget * container);
void RangesFrame(GtkWidget * container);
void BrightFrame(GtkWidget * container);
void GammaFrame(GtkWidget * container);
void OutputTab(void);

#endif				/* OUTPUT_H */
