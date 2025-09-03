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

#ifndef PPD_H
#define PPD_H

#include <gtk/gtk.h>
#include <cups/ppd.h>

extern GtkWidget *ppdTab, *ppdFrame;

int togglePPDOption(ppd_group_t * group, int ct, int nummer, int enable);
void enablePPDOption(int nummer);
void disablePPDOption(int nummer);
void PPD_Multi_changed(GtkWidget * widget, int data);
void addPPDGroup(ppd_group_t * group);
void ppdTabSetDefaults(void);
void PPDFrame(GtkWidget * container);
void PPDTab(void);

#endif				/* PPD_H */
