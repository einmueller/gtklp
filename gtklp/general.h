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

#ifndef GENERAL_H
#define GENERAL_H

#include <gtk/gtk.h>

extern GtkWidget *generalTab;
extern GtkWidget *printerFrame;
extern GtkWidget *printerFrameCombo;
extern GtkWidget *printerFrameInfoText;
extern GtkWidget *printerFrameLocationText;
extern GtkWidget *printerNumCopiesFrame;
extern GtkWidget *printerNumField;
extern GtkWidget *collateCheck;
extern GtkWidget *printerMediaFrame;
extern GtkWidget *printerMediaSizeCombo;
extern GtkWidget *printerMediaTypeCombo;
extern GtkWidget *printerMediaSourceCombo;
extern GtkWidget *MediaTypeFrame;
extern GtkWidget *MediaSizeFrame;
extern GtkWidget *MediaSourceFrame;
extern GtkWidget *printerDuplexFrame;
extern GtkWidget *gtklpqFrame;
extern GtkWidget *CustomMediaSizeFrame;
extern GtkWidget *CustomMediaSizeFrame;
extern GtkWidget *customSizeFieldX, *customSizeFieldY, *customSizeCombo;
extern GSList *DuplexButtons;

void generalTabSetDefaults(void);
void NumOfCopiesChanged(GtkWidget * widget, gpointer data);
void PrinterChanged(GtkWidget * widget, gpointer data);
void DuplexChanged(GtkWidget * widget, gpointer data);
void PrinterDuplex(GtkWidget * container);
void PrinterMedia(GtkWidget * container);
void PrinterNumCopies(GtkWidget * container);
void PrinterCallGtklpq(GtkWidget * container);
void callGtkLPQFunc(GtkButton * button, gpointer data);
void PrinterFrame(GtkWidget * container);
void GeneralTab(void);

#endif				/* GENERAL_H */
