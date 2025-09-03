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

#ifndef PRINTER_C
#define PRINTER_C

#include <gtk/gtk.h>
#include <cups/cups.h>

extern GtkWidget *printerFrame[2];
extern GtkWidget *queueFrame;
extern char reason[];

/* printer.c */
void printerSetDefaults(int t);
int PrinterChanged(GtkWidget * widget, gpointer data);
void PrinterFrame(GtkWidget * container, int t);
void queue_select(GtkWidget * clist, gint row, gint column,
		  GdkEventButton * event, gpointer data);
void printQueueInit(GtkWidget * container);
void printerStatus(char *URI);
void putnchar(int c, int count);
void putnstr(char *str, int count);
int printQueueUpdate(GtkWidget * widget, gpointer data);
#if GTK_MAJOR_VERSION != 1
void queueFunc_response_no(GtkEditable * editable, gpointer data);
void queueFunc_response_yes(GtkEditable * editable, gpointer data);
#endif
void queueFunc(GtkWidget * widget, gpointer data);
int jobAttr(int job_id, int num_options, cups_option_t * options);
void jobPrio(GtkWidget * dialog);
void cancelJob(GtkWidget * dialog);

#endif				/* PRINTER_C */
