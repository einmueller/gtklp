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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <limits.h>

#define PROGNAME "GtkLP"
#define MY_MAIL "gtklp@sirtobi.com"
#define MY_HOME "http://gtklp.sirtobi.com/"
#define MY_NAME "Tobias Mueller"
#define CUPS_HOME "http://www.cups.org/"
#define ABOUT_CRIGHT "(c) by Tobias Mueller, 1998-2017"
#define ABOUT_LICENSE "licensed under the terms of the GPL"

#define ProgOpts "U:P:d:c:S:lDV#:p::bio:C::h:En:q:t:H::mh::sJ:T:rf:w::y:1:2:3:4:XF"

/* Spacings and Sizes */
#define WAIT_WIN_SIZE 300
#define FRAME_SPACING_H 10
#define FRAME_SPACING_V 8
#define INFRAME_SPACING_H 10
#define INFRAME_SPACING_V 2
#define BUTTON_SPACING_H 10
#define BUTTON_SPACING_V 3
#ifndef MAX_PRT
#define MAX_PRT 150
#endif
#define MESSAGE_SIZE_H 300
#define MESSAGE_SIZE_V 100
#define MESSAGE_SPACING_H 10
#define MESSAGE_SPACING_V 10
#define NUMCOPIES_FIELD_LENGTH 40
#define MEDIA_FIELDLENGTH 60+60
#define RANGE_FIELDLENGTH 100
#define TEXTSIZE_FIELDLENGTH 130+10
#define TEXTMARGIN_FIELDLENGTH 140
#define BANNERLABELFIELD_FIELDLENGTH 50
#define JOBNAME_FIELD_LENGTH 200
#define EXTRAOPT_FIELD_LENGTH 400
#define LOGINLABELFIELD_FIELDLENGTH 80
#define NUPCOMBOWIDTH 50
#define FRAME_BORDER 4
#define VBOX_BORDER 8
#define BUTTON_PAD 16
#define DIALOG_PAD 30
#define WINDOWINSCREENBORDER 50

/* PATHS */
#define DEF_GTKLPRC "/etc/gtklp"
#define DEF_HELP_HOME "http://localhost:631/sum.html#STANDARD_OPTIONS"
#define DEF_BROWSER_CMD "netscape $0 2>/dev/null&"

#define GTKLPRC_USER ".gtklp"
#define GTKLPRC_GLOBAL "gtklprc"

/* Constants */
#define DEF_PRN_LEN 256
#define MAX_BANNER 20
#ifdef PATH_MAX
#define MAXPATH PATH_MAX
#else
#define MAXPATH 256
#endif
#define MAXLINE 256
#define MAXPRINTFILES 100
#define MAXCMD 65536
#define MAXNUMCOPYDIGIT 3
#define MAXMEDIA 70
#define MEDIALNG 100
#define MAXRANGEENTRY 20
#define WAITFORSTDIN 1000
#define SLEEPBEFORE 1
#define BRIGHT_MIN 0
#define BRIGHT_MAX 200
#define BRIGHT_STEP 10
#define BRIGHT_PAGE 1
#define BRIGHT_DIGIT 0
#define GAMMA_MIN 0
#define GAMMA_MAX 2000
#define GAMMA_STEP 20
#define GAMMA_PAGE 1
#define GAMMA_DIGIT 0
#define IMAGE_SCALE_STEP 20
#define IMAGE_SCALE_PAGE 1
#define IMAGE_SCALE_DIGIT 0
#define IMAGE_PPI_STEP 20
#define IMAGE_PPI_PAGE 1
#define IMAGE_PPI_DIGIT 0
#define IMAGE_SCALE_NATURAL_STEP 20
#define IMAGE_SCALE_NATURAL_PAGE 1
#define IMAGE_SCALE_NATURAL_DIGIT 0
#define HUE_STEP 5
#define HUE_PAGE 1
#define HUE_DIGIT 0
#define SAT_STEP 10
#define SAT_PAGE 1
#define SAT_DIGIT 0
#define MAXTEXTPERFIELDDIGITS 30
#define MAX_CPI_DIGITS 2
#define MAX_LPI_DIGITS 2
#define MAX_CPP_DIGITS 2
#define MAX_MARGIN_DIGITS 3
#define MARGINS_POINTS "points = "
#define MARGINS_INCH " inch = "
#define MARGINS_MM " mm"
#define MAXMARGIN_ENTRY_LENGTH 30
#define MAX_IMAGE_POS_LENGTH 15
#define MAX_HPGL_PEN_DIGIT 6
#define MAX_HPGL_PEN_FIELD_LENGTH 45
#define MAX_JOB_NAME_LENGTH 20
#define MAX_EXTRAOPT 256
#define FILE_DIALOG_HEIGHT 150
#define MAX_SERVER_NAME 1024
#define MAX_PORT 6
#define MAX_URI 1024
#define PRINTER_INFO_FIELD_LENGTH 160
#define MAXOPTLEN 128
#define COMBO_MIN_SPACE 100
#define COMBO_EXTRA_SPACE 10
#define MAX_CUSTOM_SIZE_LENGTH 16
#define MAX_CUSTOM_SIZE_FIELD_LENGTH 50

/* Password - Entrys */
#define MAX_USERNAME 128
#define MAX_PASSTRY 3
#define MAX_PASSLEN 128

/* GTKLPQ - Entrys */
#define GTKLPQ_ProgOpts "P:d:S:DVp:U:t:g:CXEcr:h::"
#define GTKLPQ_RANK_SIZE 80
#define GTKLPQ_OWNER_SIZE 100
#define GTKLPQ_JOB_SIZE 30
#define GTKLPQ_FILE_SIZE 240
#define GTKLPQ_MAINSIZEH 640
#define GTKLPQ_MAINSIZEV 400
#define QUERY_INTERVALL 3
#define GTKLPQ_PRIO_STEP_INCR 1
#define GTKLPQ_PRIO_PAGE_INCR 10
#define GTKLPQ_PRIO_PAGE_SIZE 10
#define GTKLPQ_PRIO_SPIN_INCR 1
#define TOOLTIP_DELAY 500

#endif				/* CONFIG_H */
