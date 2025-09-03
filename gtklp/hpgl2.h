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

#ifndef HPGL2_H
#define HPGL2_H

#include <gtk/gtk.h>

extern GtkWidget *hpgl2Tab;
extern GtkWidget *hpgl2OptFrame;
extern GtkWidget *hpgl2BlackCheck;
extern GtkWidget *hpgl2FitCheck;
extern GtkWidget *hpgl2PenFrame;
extern GtkWidget *hpgl2PenField;

void hpgl2TabSetDefaults(void);
void hpgl2CheckChanged(GtkWidget * widget, gpointer data);
void Hpgl2OptFrame(GtkWidget * container);
void hpgl2PenChanged(GtkWidget * widget, gpointer data);
void Hpgl2PenFrame(GtkWidget * container);
void Hpgl2Tab(void);

#endif				/* HPGL2_H */
