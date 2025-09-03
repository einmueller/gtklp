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

#ifndef FILE_H
#define FILE_H

#include <gtk/gtk.h>

extern GtkWidget *fileTab;
extern GtkWidget *fileRemove;
extern GtkWidget *fileAdd;
extern GtkWidget *fileFrame;
extern GtkWidget *fileList;
extern GtkWidget *fileRequ;
extern int fileSelectedEntry;

void fileTabSetDefaults(void);
void fileSelected(GtkWidget * widget, gint row, gint column,
		  GdkEventButton * event, gpointer data);
void fileListEmpty(GtkWidget * widget, gpointer data);
#ifdef OLDDIALOG
void fileSelDialogOk(GtkWidget * widget, GtkFileSelection * fs);
void fileSelDialogDestroyed(GtkWidget * widget, gpointer data);
#endif
void filesel(GtkCList * clist, gint row, gint column, GdkEventButton * event,
	     gpointer user_data);
void dirChanged(GtkWidget * widget, gpointer data);
void fileAddPressed(GtkWidget * widget, gpointer data);
void fileRemovePressed(GtkWidget * widget, gpointer data);
void fileRemoveFromList(char *file);
void FileFrame(GtkWidget * container);
void FileTab(void);
void fileRemoveFromList(char *file);

#endif				/* FILE_H */
