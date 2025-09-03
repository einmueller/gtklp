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

#ifndef LIBGTKLP_H
#define LIBGTKLP_H

#include <stdint.h>
#include <glib.h>
#include <gtk/gtk.h>
#ifndef _LOCALE_H
#define _LOCALE_H
#endif
#include <cups/language.h>
#include <cups/http.h>

#include <defaults.h>

extern int DEBUG;
extern int nox;
extern int PrinterChoice;
extern int PrinterNum;
extern int lastPrinterChoice;
extern char DEF_PRN[DEF_PRN_LEN + 1];
extern char PrinterBannerStart[MAX_PRT][MAXLINE + 1];
extern char PrinterBannerEnd[MAX_PRT][MAXLINE + 1];
extern char PrinterInfos[MAX_PRT][MAXLINE + 1];
extern char PrinterLocations[MAX_PRT][MAXLINE + 1];
extern char PrinterNames[MAX_PRT][DEF_PRN_LEN + 1];
extern char ServerName[MAX_SERVER_NAME + 1];
extern char PortString[MAX_PORT + 11];
extern char ServerAddr[MAX_SERVER_NAME + MAX_PORT + 1];
extern char UserName[MAX_USERNAME + 1];
extern char passWord[MAX_PASSLEN + 1];

extern char PrinterFavNames[MAX_PRT][DEF_PRN_LEN + 1];

extern GtkWidget *mainWindow;

extern cups_lang_t *cupsLanguage;
extern http_t *cupsHttp;

void quick_message(gchar * message, int which);
void checkFavorites(void);
void getPrinters(int loop);
void gtklp_fatal_end(char *text);
void emergency(void);
void passDestroy(void);
void passNO(GtkWidget * dialog);
void passChanged(GtkEditable * editable, gpointer data);
void loginChanged(GtkEditable * editable, gpointer data);
#if GTK_MAJOR_VERSION != 1
void passwin_response_ok(GtkEditable * editable, gpointer data);
#endif
const char *getPass(const char *prompt);
int noX(void);
int max(int a, int b);
gchar *str2str(gchar * text);
void exitOnErrorExit(GtkWidget * widget, GtkObject * object);
void exitOnError(char *gerror1, char *gerror2, int exitcode);
void hello(GtkWidget * widget, gpointer data);
void button_pad(GtkWidget * widget);
#if GTK_MAJOR_VERSION != 1
GtkWidget *progressBar;
void setWindowDefaultIcons(void);
#endif
#endif				/* LIBGTKLP_H */
