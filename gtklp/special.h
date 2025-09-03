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

#ifndef SPECIAL_H
#define SPECIAL_H

#include <gtk/gtk.h>

#include <defaults.h>

extern GtkWidget *specialTab;
extern GtkWidget *bannerFrame;
extern GtkWidget *bannerFrameStartCombo;
extern GtkWidget *bannerFrameStopCombo;
extern GtkWidget *jobNameFrame;
extern GtkWidget *jobNameField;
extern GtkWidget *specialOptFrame;
extern GtkWidget *specialRawCheck;
extern GtkWidget *extraOptFrame;
extern GtkWidget *extraOptField;
extern GtkWidget *passFrame;
extern GtkWidget *loginEntrySpecial;
extern GtkWidget *passEntrySpecial;
extern GList *bannerList;

extern int bannerAnz;
extern char Banners[MAX_BANNER][MAXLINE + 1];

void specialTabSetDefaults(void);
void BannerFrame(GtkWidget * container);
gboolean loginEntrySpecialNew(GtkWidget * widget, GdkEventFocus * event,
			      gpointer user_data);
gboolean passEntrySpecialNew(GtkEditable * editable, gpointer data);
void PassFrame(GtkWidget * container);
void JobNameChanged(GtkWidget * widget, gpointer data);
void JobNameFrame(GtkWidget * container);
void specialCheckChanged(GtkWidget * widget, gpointer data);
void SpecialOptFrame(GtkWidget * container);
void extraOptChanged(GtkWidget * widget, gpointer data);
void ExtraOptFrame(GtkWidget * container);
void SpecialTab(void);

#endif				/* SPECIAL_H */
