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

#include <config.h>

#include "gtklpq_functions.h"

#include <gtk/gtk.h>
#include <cups/http.h>
#include <cups/language.h>

#include <gettext.h>
#include <libgtklp.h>

void gtklpq_end(void)
{
	cupsLangFlush();
	cupsLanguage = NULL;
	httpClose(cupsHttp);
	cupsHttp = NULL;
	gtk_main_quit();
}
