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

#ifndef GTKLPQ_C
#define GTKLPQ_C

#include <gtk/gtk.h>

#include <defaults.h>

extern unsigned int selectedJob;
extern int m_id_upd, c_id_upd, m_id_state, c_id_state;
extern GtkWidget *buttonPrio, *buttonHold, *buttonMove, *buttonCancel,
    *buttonCancelAll, *holdLabel, *buttonHoldPrinter, *holdPrinterLabel,
    *buttonRejectJob;
extern GtkWidget *holdimage, *printerimage, *rejectjobimage;
extern GtkWidget *tabs, *statbar;
extern GdkPixmap *gtklpq_hold, *gtklpq_release, *gtklpq_stop_printer,
    *gtklpq_start_printer, *gtklpq_reject_job, *gtklpq_accept_job;
extern GdkBitmap *mgtklpq_hold, *mgtklpq_release, *mgtklpq_stop_printer,
    *mgtklpq_start_printer, *mgtklpq_reject_job, *mgtklpq_accept_job;
extern GtkTooltips *tooltips;

extern GtkWidget *dialog;

extern int cancelmode;

#endif				/* GTKLPQ_C */
