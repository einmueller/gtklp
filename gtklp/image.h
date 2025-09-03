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

#ifndef IMAGE_H
#define IMAGE_H

#include <gtk/gtk.h>

extern GtkWidget *imageTab;
extern GtkWidget *imageScalingAllFrame;
extern GtkObject *imageScaleAllAdj;

extern GtkWidget *imageHUEFrame;
extern GtkWidget *imageHUEButton;
extern GtkObject *imageHUEAdj;
extern GtkWidget *imageSatFrame;
extern GtkWidget *imageSatButton;
extern GtkObject *imageSatAdj;
extern GtkWidget *imagePosFrame;
extern GtkWidget *scaleTypeCombo;
extern GtkWidget *imageScaleAllScale;
extern GtkWidget *imagePosBut[3][3];

void imageTabSetDefaults(void);
void imageScalingAdjChanged(GtkWidget * widget, gpointer data);
void imageScalingAllChanged(GtkWidget * widget, gpointer data);
void ImageScalingAllFrame(GtkWidget * container);
void ImageHUEFrame(GtkWidget * container);
void ImageSatFrame(GtkWidget * container);
void imagePosChanged(GtkWidget * widget, gpointer data);
void ImagePosFrame(GtkWidget * container);
void ImageTab(void);

#endif				/* IMAGE_H */
