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

#ifndef TEXT_H
#define TEXT_H

#include <gtk/gtk.h>

extern GtkWidget *textTab;
extern GtkWidget *textSizesFrame;
extern GtkWidget *textCPIField;
extern GtkWidget *textLPIField;
extern GtkWidget *textCPPField;
extern GtkWidget *textLeftField;
extern GtkWidget *textRightField;
extern GtkWidget *textTopField;
extern GtkWidget *textBottomField;
extern GtkWidget *textLeftField_I;
extern GtkWidget *textRightField_I;
extern GtkWidget *textTopField_I;
extern GtkWidget *textBottomField_I;
extern GtkWidget *textLeftField_C;
extern GtkWidget *textRightField_C;
extern GtkWidget *textTopField_C;
extern GtkWidget *textBottomField_C;
extern GtkWidget *textMarginsFrame;
extern GtkWidget *textPrettyFrame;
extern GtkWidget *prettyCheck;

void textTabSetDefaults(void);
void textSizesChanged(GtkWidget * widget, gpointer data);
void textSizes(GtkWidget * container);
void textMarginsChanged(GtkWidget * widget, gpointer data);
void marginSizes(GtkWidget * container);
void wantPrettyCheck(GtkWidget * widget, gpointer data);
void textForm(GtkWidget * container);
void TextTab(void);

#endif				/* TEXT_H */
