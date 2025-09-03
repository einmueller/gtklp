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

#include <printer.h>

#include <stdlib.h>
#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/ppd.h>

#include <gettext.h>
#include <libgtklp.h>

#include "gtklpq.h"

int PrtDlgSel = 0;

GtkWidget *printerFrame[2];
GtkWidget *printerFrameCombo[2], *printerFrameInfoText[2],
    *printerFrameLocationText[2];

GtkWidget *queueFrame;
GtkWidget *queueList;

GtkWidget *scrolled_queue_window;

GtkWidget *prioSpin;

#if GTK_MAJOR_VERSION != 1
GtkWidget *qfwin;
#endif

int aktJobHold = 0;
int aktPrinterHold = 0;
int jobsRejected = 0;
int noUpdate = 0;

char reason[MAXLINE + 1];

void printerSetDefaults(int t)
{
	int prt;

	if (t == 0)
		prt = PrinterChoice;
	else
		prt = PrtDlgSel;
	if (DEBUG)
		printf("printerSetDefaults: %i\n", t);

	/* PrinterName */
	gtk_list_select_item(GTK_LIST(GTK_COMBO(printerFrameCombo[t])->list),
			     prt);
	/* PrinterInfo */
	gtk_label_set_text(GTK_LABEL(printerFrameInfoText[t]),
			   str2str(PrinterInfos[prt]));
	/* PrinterLocation */
	gtk_label_set_text(GTK_LABEL(printerFrameLocationText[t]),
			   str2str(PrinterLocations[prt]));

}

int PrinterChanged(GtkWidget * widget, gpointer data)
{
	int i1, strangeerrortrap;
	char tmp[DEF_PRN_LEN + 1];
	int t;

	t = atoi((char *)data);

	if (t == 0)
		selectedJob = 0;

	snprintf(tmp, (size_t) DEF_PRN_LEN, "%s",
		 gtk_entry_get_text(GTK_ENTRY
				    (GTK_COMBO(printerFrameCombo[t])->entry)));

	if (DEBUG)
		printf("Printer changed: %i - %s\n", t, (char *)tmp);

	if (tmp[0] == (uintptr_t) NULL) {
		if (DEBUG)
			printf("Skipping empty entrys\n");
		return (0);
	}

	i1 = 0;
	strangeerrortrap = 0;
	do {
		if (strcmp(str2str(PrinterNames[i1]), str2str(tmp)) == 0) {
			if (t == 0)
				PrinterChoice = i1;
			else
				PrtDlgSel = i1;
			strangeerrortrap = 1;
		}
		i1++;
	}
	while (PrinterNames[i1][0] != (uintptr_t) NULL);

	if (strangeerrortrap == 0) {
		if (DEBUG)
			printf("Damned Combo-Box-Error !\n");
		emergency();

	}

	printerSetDefaults(t);
	if (t == 0) {
		if (buttonHold != NULL) {
			gtk_widget_set_sensitive((GtkWidget *) buttonHold,
						 FALSE);
			gtk_widget_set_sensitive((GtkWidget *) buttonMove,
						 FALSE);
			gtk_widget_set_sensitive((GtkWidget *) buttonCancel,
						 FALSE);
			gtk_widget_set_sensitive((GtkWidget *) buttonCancelAll,
						 FALSE);
			gtk_widget_set_sensitive((GtkWidget *) buttonPrio,
						 FALSE);
		}

		if (queueList != NULL)
			printQueueUpdate(NULL, NULL);
	}
	return (0);
}

void PrinterFrame(GtkWidget * container, int t)
{
	GtkWidget *mainHBox, *hbox, *vbox;
	GList *printerList = NULL;
	int i1;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	printerFrame[t] = gtk_frame_new(str2str(_("Printer")));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerFrame[t], TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Printer Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(printerFrame[t]), hbox);
	gtk_widget_show(hbox);

	printerFrameCombo[t] = gtk_combo_new();
	for (i1 = 0; i1 <= PrinterNum; i1++) {
		printerList =
		    g_list_append(printerList, str2str(PrinterNames[i1]));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(printerFrameCombo[t]),
				      printerList);
	gtk_combo_set_value_in_list(GTK_COMBO(printerFrameCombo[t]), TRUE,
				    FALSE);
	gtk_list_set_selection_mode(GTK_LIST
				    (GTK_COMBO(printerFrameCombo[t])->list),
				    GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY
			       (GTK_COMBO(printerFrameCombo[t])->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), printerFrameCombo[t], FALSE, FALSE,
			   INFRAME_SPACING_H);
	gtk_widget_show(printerFrameCombo[t]);

	/* InfoBoxen */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_set_spacing(GTK_BOX(vbox), 0);
	gtk_box_pack_end(GTK_BOX(hbox), vbox, FALSE, FALSE, INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	/* Information */
	printerFrameInfoText[t] = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameInfoText[t]), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameInfoText[t]),
			      GTK_JUSTIFY_LEFT);
	gtk_widget_set_usize(printerFrameInfoText[t], PRINTER_INFO_FIELD_LENGTH,
			     -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameInfoText[t], FALSE, FALSE,
			   0);
	gtk_widget_show(printerFrameInfoText[t]);

	/* Location */
	printerFrameLocationText[t] = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameLocationText[t]), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameLocationText[t]),
			      GTK_JUSTIFY_LEFT);
	gtk_widget_set_usize(printerFrameLocationText[t],
			     PRINTER_INFO_FIELD_LENGTH, -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameLocationText[t], FALSE,
			   FALSE, INFRAME_SPACING_H);
	gtk_widget_show(printerFrameLocationText[t]);

	/* Signals */
	if (t == 0)
		gtk_signal_connect(GTK_OBJECT
				   (GTK_COMBO(printerFrameCombo[t])->entry),
				   "changed", GTK_SIGNAL_FUNC(PrinterChanged),
				   "0");
	else
		gtk_signal_connect(GTK_OBJECT
				   (GTK_COMBO(printerFrameCombo[t])->entry),
				   "changed", GTK_SIGNAL_FUNC(PrinterChanged),
				   "1");
}

void
queue_select(GtkWidget * clist, gint row, gint column,
	     GdkEventButton * event, gpointer data)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(clist), row, 3, &text);
	selectedJob = atoi(text);
	gtk_widget_set_sensitive((GtkWidget *) buttonHold, TRUE);
	gtk_widget_set_sensitive((GtkWidget *) buttonCancel, TRUE);
	gtk_widget_set_sensitive((GtkWidget *) buttonPrio, TRUE);

	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &text);
	if (strcmp((char *)text, str2str(_("H"))) == 0) {
		aktJobHold = 1;
		gtk_container_remove(GTK_CONTAINER(buttonHold), holdimage);
		holdimage = gtk_pixmap_new(gtklpq_release, mgtklpq_release);
		gtk_widget_show(holdimage);
		gtk_container_add(GTK_CONTAINER(buttonHold), holdimage);

		gtk_tooltips_set_tip(tooltips, buttonHold,
				     str2str(_("Release Job")),
				     str2str(_("Release Job")));

	} else {
		aktJobHold = 0;
		gtk_container_remove(GTK_CONTAINER(buttonHold), holdimage);
		holdimage = gtk_pixmap_new(gtklpq_hold, mgtklpq_hold);
		gtk_widget_show(holdimage);
		gtk_container_add(GTK_CONTAINER(buttonHold), holdimage);

		gtk_tooltips_set_tip(tooltips, buttonHold,
				     str2str(_("Hold Job")),
				     str2str(_("Hold Job")));
	}

	if (strcmp((char *)text, str2str(_("A"))) == 0) {
		gtk_widget_set_sensitive((GtkWidget *) buttonMove, FALSE);
	} else {
		gtk_widget_set_sensitive((GtkWidget *) buttonMove, TRUE);
	}
}

void printQueueInit(GtkWidget * container)
{
	GtkWidget *mainHBox, *hbox;
	int i;
	GtkWidget *titles[7];

	titles[0] = gtk_label_new("   ");
	titles[1] = gtk_label_new(str2str(_("Rank")));
	titles[2] = gtk_label_new(str2str(_("Owner")));
	titles[3] = gtk_label_new(str2str(_("Job")));
	titles[4] = gtk_label_new(str2str(_("File(s)")));
	titles[5] = gtk_label_new(str2str(_("#")));
	titles[6] = gtk_label_new(str2str(_("Size")));
	for (i = 0; i < 7; i++) {
		gtk_misc_set_alignment(GTK_MISC(titles[i]), 0, 0.5);
		gtk_widget_show(titles[i]);
	}

	if (DEBUG)
		printf("printQueueInit\n");

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, TRUE, TRUE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	queueFrame = gtk_frame_new(str2str(_("Queue")));
	gtk_box_pack_start(GTK_BOX(mainHBox), queueFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	/* Queue Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(queueFrame), hbox);
	gtk_widget_show(hbox);

	scrolled_queue_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolled_queue_window),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(hbox), scrolled_queue_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_queue_window);

	/* Queue List */
	queueList = gtk_clist_new(7);
	for (i = 0; i < 7; i++) {
		gtk_clist_set_column_widget(GTK_CLIST(queueList), i, titles[i]);
		gtk_clist_set_column_justification(GTK_CLIST(queueList), i,
						   GTK_JUSTIFY_LEFT);
	}
	gtk_clist_column_titles_show(GTK_CLIST(queueList));
	gtk_signal_connect(GTK_OBJECT(queueList), "select_row",
			   GTK_SIGNAL_FUNC(queue_select), NULL);

	gtk_scrolled_window_add_with_viewport((GtkScrolledWindow *)
					      scrolled_queue_window,
					      (GtkWidget *) queueList);
	gtk_clist_set_shadow_type(GTK_CLIST(queueList), GTK_SHADOW_OUT);

#if GTK_MAJOR_VERSION == 1
	gtk_clist_set_column_width(GTK_CLIST(queueList), 1,
				   max(GTKLPQ_RANK_SIZE,
				       gdk_string_width(gtk_widget_get_style
							(titles[1])->font,
							str2str(_("Rank")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 2,
				   max(GTKLPQ_OWNER_SIZE,
				       gdk_string_width(gtk_widget_get_style
							(titles[2])->font,
							str2str(_("Owner")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 3,
				   max(GTKLPQ_JOB_SIZE,
				       gdk_string_width(gtk_widget_get_style
							(titles[3])->font,
							str2str(_("Job")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 4,
				   max(GTKLPQ_FILE_SIZE,
				       gdk_string_width(gtk_widget_get_style
							(titles[4])->font,
							str2str(_
								("File(s)")))));
#else
	gtk_clist_set_column_width(GTK_CLIST(queueList), 1,
				   max(GTKLPQ_RANK_SIZE,
				       gdk_string_width(gtk_style_get_font
							(gtk_widget_get_style
							 (titles[1])),
							str2str(_("Rank")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 2,
				   max(GTKLPQ_OWNER_SIZE,
				       gdk_string_width(gtk_style_get_font
							(gtk_widget_get_style
							 (titles[2])),
							str2str(_("Owner")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 3,
				   max(GTKLPQ_JOB_SIZE,
				       gdk_string_width(gtk_style_get_font
							(gtk_widget_get_style
							 (titles[3])),
							str2str(_("Job")))));
	gtk_clist_set_column_width(GTK_CLIST(queueList), 4,
				   max(GTKLPQ_FILE_SIZE,
				       gdk_string_width(gtk_style_get_font
							(gtk_widget_get_style
							 (titles[4])),
							str2str(_
								("File(s)")))));
#endif
	gtk_widget_show(queueList);

}

void printerStatus(char *URI)
{
	ipp_t *request, *response;
	ipp_attribute_t *attr;
	char toolstring[MAXLINE + 1];
	int err = 0;

	if (nox == 0) {
		if (m_id_state > 0)
			gtk_statusbar_remove((GtkStatusbar *) statbar,
					     c_id_state, m_id_state);
	}

	request = ippNew();
	ippSetOperation(request, IPP_GET_PRINTER_ATTRIBUTES);
	ippSetRequestId(request, 1);
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL,
		     cupsLangEncoding(cupsLanguage));
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     cupsLanguage->language);
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
		     NULL, URI);

	if ((response = cupsDoRequest(cupsHttp, request, "/")) != NULL) {
		if ((attr =
		     ippFindAttribute(response, "printer-state", IPP_TAG_ZERO))
		    != NULL) {
			aktPrinterHold = 0;
			switch (ippGetInteger(attr, 0)) {
			case IPP_PRINTER_IDLE:
				strncpy(toolstring, str2str(_("Printer Idle")),
					(size_t) MAXLINE);
				break;
			case IPP_PRINTER_PROCESSING:
				strncpy(toolstring,
					str2str(_("Printer Processing")),
					(size_t) MAXLINE);
				break;
			case IPP_PRINTER_STOPPED:
				strncpy(toolstring,
					str2str(_("Printer Stopped")),
					(size_t) MAXLINE);
				aktPrinterHold = 1;
				break;
			default:
				snprintf(toolstring, (size_t) MAXLINE, "%i",
					 ippGetInteger(attr, 0));
				break;
			}

			jobsRejected = 0;
			attr =
			    ippFindAttribute(response,
					     "printer-is-accepting-jobs",
					     IPP_TAG_ZERO);
			if (attr != NULL) {
				if (ippGetInteger(attr, 0) == 0)
					jobsRejected = 1;
				if (DEBUG)
					printf
					    ("Attr=printer-is-accepting-jobs: %i\n",
					     ippGetInteger(attr, 0));
			}

			if (nox == 0) {
				if (aktPrinterHold == 1) {
					gtk_container_remove(GTK_CONTAINER
							     (buttonHoldPrinter),
							     printerimage);
					printerimage =
					    gtk_pixmap_new(gtklpq_start_printer,
							   mgtklpq_start_printer);
					gtk_widget_show(printerimage);
					gtk_container_add(GTK_CONTAINER
							  (buttonHoldPrinter),
							  printerimage);

					gtk_tooltips_set_tip(tooltips,
							     buttonHoldPrinter,
							     str2str(_
								     ("Start Printer")),
							     str2str(_
								     ("Start Printer")));
				} else {
					gtk_container_remove(GTK_CONTAINER
							     (buttonHoldPrinter),
							     printerimage);
					printerimage =
					    gtk_pixmap_new(gtklpq_stop_printer,
							   mgtklpq_stop_printer);
					gtk_widget_show(printerimage);
					gtk_container_add(GTK_CONTAINER
							  (buttonHoldPrinter),
							  printerimage);

					gtk_tooltips_set_tip(tooltips,
							     buttonHoldPrinter,
							     str2str(_
								     ("Stop Printer")),
							     str2str(_
								     ("Stop Printer")));
				}

				if (jobsRejected == 1) {
					gtk_container_remove(GTK_CONTAINER
							     (buttonRejectJob),
							     rejectjobimage);
					rejectjobimage =
					    gtk_pixmap_new(gtklpq_accept_job,
							   mgtklpq_accept_job);
					gtk_widget_show(rejectjobimage);
					gtk_container_add(GTK_CONTAINER
							  (buttonRejectJob),
							  rejectjobimage);

					gtk_tooltips_set_tip(tooltips,
							     buttonRejectJob,
							     str2str(_
								     ("Accept Jobs")),
							     str2str(_
								     ("Accept Jobs")));
				} else {
					gtk_container_remove(GTK_CONTAINER
							     (buttonRejectJob),
							     rejectjobimage);
					rejectjobimage =
					    gtk_pixmap_new(gtklpq_reject_job,
							   mgtklpq_reject_job);
					gtk_widget_show(rejectjobimage);
					gtk_container_add(GTK_CONTAINER
							  (buttonRejectJob),
							  rejectjobimage);

					gtk_tooltips_set_tip(tooltips,
							     buttonRejectJob,
							     str2str(_
								     ("Reject Jobs")),
							     str2str(_
								     ("Reject Jobs")));
				}
			}
		} else {
			err = 1;
		}

		if ((attr =
		     ippFindAttribute(response, "printer-state-message",
				      IPP_TAG_ZERO)) != NULL) {
			if ((strlen(toolstring) +
			     strlen(ippGetString(attr, 0, NULL))) < MAXLINE)
				snprintf(toolstring, (size_t) MAXLINE,
					 "%s - %s", toolstring,
					 ippGetString(attr, 0, NULL));
		}
		if (nox == 0)
			m_id_state =
			    gtk_statusbar_push((GtkStatusbar *) statbar,
					       c_id_state,
					       (const char *)
					       str2str(toolstring));
		else
			g_print("[%s] %s\n", PrinterNames[PrinterChoice],
				toolstring);
		ippDelete(response);
	}
	if (nox == 0) {
		if (err == 1)
			gtk_widget_set_sensitive((GtkWidget *)
						 buttonHoldPrinter, FALSE);
		else
			gtk_widget_set_sensitive((GtkWidget *)
						 buttonHoldPrinter, TRUE);
	}
}

void putnchar(int c, int count)
{
	int i1;

	for (i1 = 0; i1 < count; i1++)
		putchar(c);
}

void putnstr(char *str, int count)
{
	int i1, i2;

	i1 = strlen(str);

	if (i1 < count) {
		putnchar(' ', count - i1);
		count = i1;
	}

	if (i1 == count) {
		for (i2 = 0; i2 < i1; i2++) {
			putchar(*str);
			str++;
		}
		return;
	}

	if (i1 > count) {
		for (i2 = 0; i2 < count; i2++) {
			putchar(*str);
			str++;
		}
		return;
	}
}

int printQueueUpdate(GtkWidget * widget, gpointer data)
{
	ipp_t *request, *response;
	ipp_attribute_t *attr;
	cups_lang_t *language;
	const char *jobdest, *jobuser, *jobname;
	ipp_jstate_t jobstate;
	int jobid, jobsize, jobcount, jobcopies, rank;
	char resource[MAX_URI + 1];
	char rankstr[MAXLINE + 1];
	char *z1;
	int i1 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;

	gchar queryLine[7][MAXLINE + 1];
	gchar *g[7];
	int queueIsSel = 0;
	static const char *jattrs[] = {
		"copies",
		"job-name",
		"job-originating-user-name",
		"job-printer-uri",
		"job-state",
		"job-k-octets",
		"job-id",
		"job-priority",
		"job-state"
	};

	if (DEBUG)
		printf("Update...\n");

	for (i1 = 0; i1 < 7; i1++)
		g[i1] = queryLine[i1];

	if (nox == 0) {
		if (m_id_upd > 0)
			gtk_statusbar_remove((GtkStatusbar *) statbar, c_id_upd,
					     m_id_upd);
	}

	request = ippNewRequest(IPP_GET_JOBS);
	ippSetRequestId(request, 1);

	language = cupsLangDefault();
	attr =
	    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
			 "attributes-charset", NULL,
			 cupsLangEncoding(language));
	attr =
	    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
			 "attributes-natural-language", NULL,
			 language->language);

	if (strlen(ServerAddr) + strlen(PrinterNames[PrinterChoice] + 20) <
	    MAX_URI) {
		z1 = strchr(PrinterNames[PrinterChoice], '/');
		if (z1 != NULL)
			*z1 = (uintptr_t) NULL;
		snprintf(resource, (size_t) MAX_URI, "ipp://%s/printers/%s",
			 ServerAddr, PrinterNames[PrinterChoice]);
		if (z1 != NULL)
			*z1 = '/';

		printerStatus(resource);
	} else {
		if (DEBUG)
			printf("MAXURI too short !\n");
		return (FALSE);
	}

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
		     NULL, resource);

	ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
		      "requested-attributes",
		      (sizeof(jattrs) / sizeof(jattrs[0])), NULL, jattrs);

	jobcount = 0;

	if (nox == 0)
		gtk_clist_clear(GTK_CLIST(queueList));

	if ((response = cupsDoRequest(cupsHttp, request, "/")) != NULL) {
		if (ippGetStatusCode(response) > IPP_OK_CONFLICT) {
			if (nox == 0)
				m_id_upd =
				    gtk_statusbar_push((GtkStatusbar *) statbar,
						       c_id_upd,
						       str2str((char *)
							       ippErrorString
							       (ippGetStatusCode
								(response))));
			else
				g_print("[%s] %s: %s\n\n",
					PrinterNames[PrinterChoice],
					str2str(_("Error")),
					ippErrorString(ippGetStatusCode
						       (response)));
			ippDelete(response);
			return (FALSE);
		} else {
			if (nox == 0) {
				gtk_widget_set_sensitive((GtkWidget *)
							 buttonHold, FALSE);
				gtk_widget_set_sensitive((GtkWidget *)
							 buttonMove, FALSE);
				gtk_widget_set_sensitive((GtkWidget *)
							 buttonCancel, FALSE);
				gtk_widget_set_sensitive((GtkWidget *)
							 buttonCancelAll,
							 FALSE);
				gtk_widget_set_sensitive((GtkWidget *)
							 buttonPrio, FALSE);
			}
		}

		if (nox == 1) {
			l1 = max(strlen(""), 1);
			l2 = max(strlen(str2str(_("Rank"))), 8);
			l3 = max(strlen(str2str(_("Owner"))), 10);
			l4 = max(strlen(str2str(_("Job"))), 6);
			l6 = max(strlen(str2str(_("#"))), 3);
			l7 = max(strlen(str2str(_("Size"))), 10);
			l5 = max(strlen(str2str(_("Job(s)"))),
				 73 - l1 - l2 - l3 - l4 - l6 - l7);

			putnchar('-', l1 + l2 + 2);
			putchar('+');
			putnchar('-', l3);
			putchar('+');
			putnchar('-', l4);
			putchar('+');
			putnchar('-', l5);
			putchar('+');
			putnchar('-', l6);
			putchar('+');
			putnchar('-', l7);
			putchar('\n');

			putchar(' ');
			putnstr("", l1);
			putchar(' ');
			putnstr(str2str(_("Rank")), l2);
			putchar('|');
			putnstr(str2str(_("Owner")), l3);
			putchar('|');
			putnstr(str2str(_("Job")), l4);
			putchar('|');
			putnstr(str2str(_("Job(s)")), l5);
			putchar('|');
			putnstr(str2str(_("#")), l6);
			putchar('|');
			putnstr(str2str(_("Size")), l7);
			putchar('\n');

			putnchar('-', l1 + l2 + 2);
			putchar('+');
			putnchar('-', l3);
			putchar('+');
			putnchar('-', l4);
			putchar('+');
			putnchar('-', l5);
			putchar('+');
			putnchar('-', l6);
			putchar('+');
			putnchar('-', l7);
			putchar('\n');

		}

		rank = 1;
		for (attr = ippFirstAttribute(response); attr != NULL;
		     attr = ippNextAttribute(response)) {
			while (attr != NULL
			       && ippGetGroupTag(attr) != IPP_TAG_JOB)
				attr = ippNextAttribute(response);

			if (attr == NULL)
				break;

			jobid = 0;
			jobsize = 0;
			jobstate = IPP_JOB_PENDING;
			jobname = str2str(_("Untitled"));
			jobuser = "()";
			jobdest = NULL;
			jobcopies = 1;

			while (attr != NULL
			       && ippGetGroupTag(attr) == IPP_TAG_JOB) {
				if (strcmp(ippGetName(attr), "job-id") == 0
				    && ippGetValueTag(attr) == IPP_TAG_INTEGER)
					jobid = ippGetInteger(attr, 0);

				if (strcmp(ippGetName(attr), "job-k-octets") ==
				    0
				    && ippGetValueTag(attr) == IPP_TAG_INTEGER)
					jobsize = ippGetInteger(attr, 0) * 1024;

				if (strcmp(ippGetName(attr), "job-state") == 0
				    && ippGetValueTag(attr) == IPP_TAG_ENUM)
					jobstate =
					    (ipp_jstate_t) ippGetInteger(attr,
									 0);

				if (strcmp(ippGetName(attr), "job-printer-uri")
				    == 0 && ippGetValueTag(attr) == IPP_TAG_URI)
					if ((jobdest =
					     strrchr(ippGetString
						     (attr, 0, NULL),
						     '/')) != NULL)
						jobdest++;

				if (strcmp
				    (ippGetName(attr),
				     "job-originating-user-name") == 0
				    && ippGetValueTag(attr) == IPP_TAG_NAME)
					jobuser = ippGetString(attr, 0, NULL);

				if (strcmp(ippGetName(attr), "job-name") == 0
				    && ippGetValueTag(attr) == IPP_TAG_NAME)
					jobname = ippGetString(attr, 0, NULL);

				if (strcmp(ippGetName(attr), "copies") == 0
				    && ippGetValueTag(attr) == IPP_TAG_INTEGER)
					jobcopies = ippGetInteger(attr, 0);

				attr = ippNextAttribute(response);
			}

			if (jobdest == NULL || jobid == 0) {
				if (attr == NULL)
					break;
				else
					continue;
			}
			jobcount++;
			if (jobstate == IPP_JOB_PROCESSING)
				strncpy(rankstr, "active", (size_t) MAXLINE);
			else {
				snprintf(rankstr, sizeof(rankstr), "%d", rank);
				rank++;
			}
			i1 = jobstate;
			switch (i1) {
			case IPP_JOB_PROCESSING:	/* Active 5 */
				strncpy(queryLine[0], str2str(_("A")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_HELD:	/* Hold 4 */
				strncpy(queryLine[0], str2str(_("H")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_PENDING:	/* Pending 3 */
				strncpy(queryLine[0], str2str(_("P")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_STOPPED:
				strncpy(queryLine[0], str2str(_("S")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_CANCELLED:
				strncpy(queryLine[0], str2str(_("ca")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_ABORTED:
				strncpy(queryLine[0], str2str(_("ab")),
					(size_t) MAXLINE);
				break;
			case IPP_JOB_COMPLETED:
				strncpy(queryLine[0], str2str(_(".")),
					(size_t) MAXLINE);
				break;
			default:
				snprintf(queryLine[0], (size_t) MAXLINE, "%i",
					 i1);
				break;
			}

			strncpy(queryLine[1], str2str(rankstr),
				(size_t) MAXLINE);
			strncpy(queryLine[2], str2str((char *)jobuser),
				(size_t) MAXLINE);
			snprintf(queryLine[3], (size_t) MAXLINE, "%d", jobid);
			strncpy(queryLine[4], str2str((char *)jobname),
				(size_t) MAXLINE);
			snprintf(queryLine[5], (size_t) MAXLINE, "%i",
				 jobcopies);
			snprintf(queryLine[6], (size_t) MAXLINE, "%d", jobsize);

			if (nox == 1) {
				putchar(' ');
				putnstr(g[0], l1);
				putchar(' ');
				putnstr(g[1], l2);
				putchar('|');
				putnstr(g[2], l3);
				putchar('|');
				putnstr(g[3], l4);
				putchar('|');
				putnstr(g[4], l5);
				putchar('|');
				putnstr(g[5], l6);
				putchar('|');
				putnstr(g[6], l7);
				putchar('\n');
			}

			if (nox == 0) {
				/* honor button pushes */
				while (gtk_events_pending())
					gtk_main_iteration();
				i1 = gtk_clist_append((GtkCList *) queueList,
						      g);

				if (selectedJob == jobid) {
					gtk_clist_select_row((GtkCList *)
							     queueList, i1, 0);
					queueIsSel = 1;
					if (jobstate == IPP_JOB_HELD) {
						aktJobHold = 1;
						gtk_container_remove
						    (GTK_CONTAINER(buttonHold),
						     holdimage);
						holdimage =
						    gtk_pixmap_new
						    (gtklpq_release,
						     mgtklpq_release);
						gtk_widget_show(holdimage);
						gtk_container_add(GTK_CONTAINER
								  (buttonHold),
								  holdimage);

						gtk_tooltips_set_tip(tooltips,
								     buttonHold,
								     str2str(_
									     ("Release Job")),
								     str2str(_
									     ("Release Job")));
					} else {
						aktJobHold = 0;
						gtk_container_remove
						    (GTK_CONTAINER(buttonHold),
						     holdimage);
						holdimage =
						    gtk_pixmap_new(gtklpq_hold,
								   mgtklpq_hold);
						gtk_widget_show(holdimage);
						gtk_container_add(GTK_CONTAINER
								  (buttonHold),
								  holdimage);

						gtk_tooltips_set_tip(tooltips,
								     buttonHold,
								     str2str(_
									     ("Hold Job")),
								     str2str(_
									     ("Hold Job")));
					}

				}
				if (queueIsSel == 0) {
					gtk_widget_set_sensitive((GtkWidget *)
								 buttonHold,
								 FALSE);
					gtk_widget_set_sensitive((GtkWidget *)
								 buttonMove,
								 FALSE);
					gtk_widget_set_sensitive((GtkWidget *)
								 buttonCancel,
								 FALSE);
					gtk_widget_set_sensitive((GtkWidget *)
								 buttonPrio,
								 FALSE);
				}
				if (jobcount > 0) {
					gtk_widget_set_sensitive((GtkWidget *)
								 buttonCancelAll,
								 TRUE);
				}
			}

			if (attr == NULL)
				break;
		}
		ippDelete(response);
	} else {
		if (nox == 0)
			m_id_upd =
			    gtk_statusbar_push((GtkStatusbar *) statbar,
					       c_id_upd,
					       str2str((char *)
						       ippErrorString
						       (cupsLastError())));
		else
			g_print("[%s] %s: %s\n\n", PrinterNames[PrinterChoice],
				str2str(_("Error")),
				ippErrorString(cupsLastError()));
		return (FALSE);
	}

	if (queueIsSel == 0) {
		selectedJob = 0;
	}

	if ((jobcount == 0) && (nox == 1))
		g_print("%s\n", str2str(_("No jobs.")));

	return (TRUE);

}

void reasonChanged(GtkWidget * entry, gpointer data)
{
	strncpy(reason, (char *)gtk_entry_get_text(GTK_ENTRY(entry)),
		(size_t) MAXLINE);
}

#if GTK_MAJOR_VERSION != 1
void queueFunc_response_no(GtkEditable * editable, gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(qfwin), 0);
}

void queueFunc_response_yes(GtkEditable * editable, gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(qfwin), 1);
}
#endif

void queueFunc(GtkWidget * widget, gpointer data)
{
	GtkWidget *hbox1, *vbox, *label, *sep1, *prioBox, *labelr, *entry;
#if GTK_MAJOR_VERSION == 1
	GtkWidget *sep, *hbox, *yes, *no, *qfwin;
#endif

	GtkObject *prioAdj;
#if GTK_MAJOR_VERSION == 1
	qfwin = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(qfwin), str2str(_("Are you sure?")));
	gtk_window_set_policy(GTK_WINDOW(qfwin), FALSE, FALSE, TRUE);
#else
	qfwin =
	    gtk_dialog_new_with_buttons(str2str(_("Are you sure?")),
					GTK_WINDOW(mainWindow),
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_YES, 1, GTK_STOCK_NO, 0,
					NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(qfwin), 0);
#endif

	gtk_window_set_title(GTK_WINDOW(qfwin), str2str(_("Are you sure?")));
	gtk_window_set_policy(GTK_WINDOW(qfwin), FALSE, FALSE, TRUE);

	hbox1 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
#if GTK_MAJOR_VERSION == 1
	gtk_container_add(GTK_CONTAINER(qfwin), hbox1);
#else
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(qfwin)->vbox), hbox1);
#endif
	gtk_widget_show(hbox1);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_box_pack_start(GTK_BOX(hbox1), vbox, FALSE, FALSE, DIALOG_PAD);
	gtk_widget_show(vbox);

	label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, DIALOG_PAD);
	gtk_widget_show(label);

	if (strcmp((char *)data, "MOVE") == 0) {
		sep1 = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox), sep1, FALSE, FALSE, 0);
		gtk_widget_show(sep1);
		PrinterFrame(vbox, 1);
#if GTK_MAJOR_VERSION != 1
		gtk_signal_connect_object(GTK_OBJECT
					  (GTK_COMBO(printerFrameCombo[1])->
					   entry), "activate",
					  GTK_SIGNAL_FUNC
					  (queueFunc_response_yes), NULL);
#endif
	}

	if (strcmp((char *)data, "PRIORITY") == 0) {
		prioBox = gtk_hbox_new(TRUE, INFRAME_SPACING_H);
		gtk_box_pack_start(GTK_BOX(vbox), prioBox, FALSE, FALSE, 0);
		gtk_widget_show(prioBox);

		prioAdj =
		    gtk_adjustment_new(50, 1, 100, GTKLPQ_PRIO_STEP_INCR,
				       GTKLPQ_PRIO_PAGE_INCR,
				       GTKLPQ_PRIO_PAGE_SIZE);
		prioSpin =
		    gtk_spin_button_new(GTK_ADJUSTMENT(prioAdj),
					GTKLPQ_PRIO_SPIN_INCR, 0);
		gtk_widget_show(prioSpin);
		gtk_box_pack_start(GTK_BOX(prioBox), prioSpin, FALSE, FALSE, 0);

#if GTK_MAJOR_VERSION != 1
		gtk_signal_connect_object(GTK_OBJECT(prioSpin), "activate",
					  GTK_SIGNAL_FUNC
					  (queueFunc_response_yes), NULL);
#endif

	}

	if (((strcmp((char *)data, "HOLDPRINTER") == 0)
	     && (aktPrinterHold == 0))
	    || ((strcmp((char *)data, "REJECTJOB") == 0)
		&& (jobsRejected == 0))) {

		labelr = gtk_label_new(str2str(_("And for what reason ?")));
		gtk_box_pack_start(GTK_BOX(vbox), labelr, FALSE, FALSE, 0);
		gtk_widget_show(labelr);

		entry = gtk_entry_new();
		if (reason[0] != (uintptr_t) NULL)
			gtk_entry_set_text(GTK_ENTRY(entry), reason);
		gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
		gtk_signal_connect(GTK_OBJECT(entry), "changed",
				   GTK_SIGNAL_FUNC(reasonChanged), NULL);
		gtk_widget_show(entry);

#if GTK_MAJOR_VERSION != 1
		gtk_signal_connect_object(GTK_OBJECT(entry), "activate",
					  GTK_SIGNAL_FUNC
					  (queueFunc_response_no), NULL);
#endif

	}
#if GTK_MAJOR_VERSION == 1
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);
	gtk_widget_show(sep);
#endif

#if GTK_MAJOR_VERSION == 1
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);
#endif
#if GTK_MAJOR_VERSION == 1
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, INFRAME_SPACING_V);
#endif
#if GTK_MAJOR_VERSION == 1
	gtk_widget_show(hbox);

	yes = gtk_button_new_with_label(str2str(_("Yes")));
	gtk_box_pack_start(GTK_BOX(hbox), yes, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_show(yes);

	no = gtk_button_new_with_label(str2str(_("No")));
	gtk_box_pack_start(GTK_BOX(hbox), no, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_grab_focus(no);
	gtk_widget_show(no);

	gtk_signal_connect_object(GTK_OBJECT(no), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(qfwin));
#endif

	if (strcmp((char *)data, "CANCEL") == 0) {
		cancelmode = 0;
		gtk_label_set_text(GTK_LABEL(label),
				   str2str(_
					   ("Do you really want to cancel this job?")));
#if GTK_MAJOR_VERSION == 1
		gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
					  GTK_SIGNAL_FUNC(cancelJob),
					  GTK_OBJECT(qfwin));
#endif
	}
	if (strcmp((char *)data, "CANCELALL") == 0) {
		cancelmode = 1;
		gtk_label_set_text(GTK_LABEL(label),
				   str2str(_
					   ("Do you really want to cancel ALL jobs?")));
#if GTK_MAJOR_VERSION == 1
		gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
					  GTK_SIGNAL_FUNC(cancelJob),
					  GTK_OBJECT(qfwin));
#endif
	}
	if (strcmp((char *)data, "HOLD") == 0) {
		if (aktJobHold == 0) {
			cancelmode = 2;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to hold this job?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		} else {
			cancelmode = 3;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to release this job?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		}
	}
	if (strcmp((char *)data, "MOVE") == 0) {
		cancelmode = 4;
		PrtDlgSel = PrinterChoice;
		gtk_widget_show(printerFrame[1]);
		printerSetDefaults(1);
		gtk_label_set_text(GTK_LABEL(label),
				   str2str(_
					   ("Do you really want to move this Job to the following queue?")));
#if GTK_MAJOR_VERSION == 1
		gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
					  GTK_SIGNAL_FUNC(cancelJob),
					  GTK_OBJECT(qfwin));
#endif
	}
	if (strcmp((char *)data, "HOLDPRINTER") == 0) {
		if (aktPrinterHold == 0) {
			cancelmode = 5;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to stop this printer?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		} else {
			cancelmode = 6;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to start this printer?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		}
	}

	if (strcmp((char *)data, "PRIORITY") == 0) {
		cancelmode = 7;
		gtk_label_set_text(GTK_LABEL(label),
				   str2str(_
					   ("Do you really want to change the job priority?")));
#if GTK_MAJOR_VERSION == 1
		gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
					  GTK_SIGNAL_FUNC(jobPrio),
					  GTK_OBJECT(qfwin));
#endif
	}

	if (strcmp((char *)data, "REJECTJOB") == 0) {
		if (jobsRejected == 0) {
			cancelmode = 8;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to reject jobs for this printer?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		} else {
			cancelmode = 9;
			gtk_label_set_text(GTK_LABEL(label),
					   str2str(_
						   ("Do you really want to accept jobs for this printer?")));
#if GTK_MAJOR_VERSION == 1
			gtk_signal_connect_object(GTK_OBJECT(yes), "clicked",
						  GTK_SIGNAL_FUNC(cancelJob),
						  GTK_OBJECT(qfwin));
#endif
		}
	}
#if GTK_MAJOR_VERSION == 1
	gtk_window_set_position(GTK_WINDOW(qfwin), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(qfwin), TRUE);
	gtk_widget_show(qfwin);
#else
	if (gtk_dialog_run(GTK_DIALOG(qfwin)) == 1) {	/* Yes clicked */
		switch (cancelmode) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 8:
		case 9:
			cancelJob(qfwin);
			break;
		case 7:
			jobPrio(qfwin);
			break;
		default:
			break;
		}
	} else {
		gtk_widget_destroy(qfwin);
	}
#endif
}

int jobAttr(int job_id, int num_options, cups_option_t * options)
{
	ipp_t *request, *response;
	char uri[MAX_URI + 1];
	cups_lang_t *language;

	request = ippNew();
	ippSetOperation(request, IPP_SET_JOB_ATTRIBUTES);
	ippSetRequestId(request, 1);

	language = cupsLangDefault();

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL, language->language);

	if ((strlen(ServerAddr) + 25) < MAX_URI) {
		snprintf(uri, (size_t) MAX_URI, "ipp://%s/jobs/%i", ServerAddr,
			 job_id);
		if (DEBUG)
			printf("uri: %s\n", uri);
	} else {
		if (DEBUG)
			printf("MAXURI too short !\n");
		return (-1);
	}

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri", NULL,
		     uri);
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
		     "requesting-user-name", NULL, cupsUser());

	cupsEncodeOptions(request, num_options, options);

	if ((response = cupsDoRequest(cupsHttp, request, "/jobs")) != NULL) {
		if (ippGetStatusCode(response) > IPP_OK_CONFLICT) {
			if (DEBUG)
				printf("set-job-attributes failed(1): %s\n",
				       ippErrorString(ippGetStatusCode
						      (response)));
			quick_message(str2str(_("Unable to change priority!")),
				      2);
			ippDelete(response);
			return (1);
		}
		ippDelete(response);
	} else {
		if (DEBUG)
			printf("set-job-attributes failed(2): %s\n",
			       ippErrorString(cupsLastError()));
		quick_message(str2str(_("Unable to change priority!")), 2);
		return (1);
	}
	return (0);

}

void jobPrio(GtkWidget * dialog)
{
	int num_options;
	int prio;
	char TMPPRIO[5];
	cups_option_t *options;

	num_options = 0;
	prio = 0;
	TMPPRIO[0] = (uintptr_t) NULL;
	options = NULL;

	gtk_spin_button_update(GTK_SPIN_BUTTON(prioSpin));
	prio = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(prioSpin));
	if (prio > 100) {
		prio = 100;
	} else {
		if (prio < 1) {
			prio = 1;
		}
	}
	snprintf(TMPPRIO, (size_t) 4, "%i", prio);
	num_options =
	    cupsAddOption("job-priority", TMPPRIO, num_options, &options);
	jobAttr(selectedJob, num_options, options);
	gtk_widget_destroy(dialog);

}

void cancelJob(GtkWidget * dialog)
{
	int job_id;		/* Job ID */
	char uri[MAX_URI + 1];	/* Printer or job URI */
	char touri[MAX_URI + 1];
	ipp_t *request;		/* IPP request */
	ipp_t *response;	/* IPP response */
	ipp_op_t op;		/* Operation */
	cups_lang_t *language;	/* Language */
	char *z1, *z2;

	/*
	 * Setup to cancel individual print jobs...
	 */

	switch (cancelmode) {
	case 0:		/* Cancel */
		op = IPP_CANCEL_JOB;
		job_id = selectedJob;
		if (DEBUG)
			printf("IPP_CANCEL_JOB\n");
		break;
	case 1:		/* CancelAll */
		op = IPP_PURGE_JOBS;
		job_id = 0;
		if (DEBUG)
			printf("IPP_PURGE_JOBS\n");
		break;
	case 2:		/* Hold Job */
		op = IPP_HOLD_JOB;
		job_id = selectedJob;
		if (DEBUG)
			printf("IPP_HOLD_JOB\n");
		break;
	case 3:		/* Release Job */
		op = IPP_RELEASE_JOB;
		job_id = selectedJob;
		if (DEBUG)
			printf("IPP_RELEASE_JOB\n");
		break;
	case 4:
		op = CUPS_MOVE_JOB;
		job_id = selectedJob;
		if (DEBUG)
			printf("CUPS_MOVE_JOB\n");
		break;
	case 5:
		op = IPP_PAUSE_PRINTER;
		job_id = 0;
		if (DEBUG)
			printf("IPP_PAUSE_PRINTER\n");
		break;
	case 6:
		op = IPP_RESUME_PRINTER;
		job_id = 0;
		if (DEBUG)
			printf("IPP_RESUME_PRINTER\n");
		break;
	case 8:		/* reject jobs */
		op = CUPS_REJECT_JOBS;
		job_id = 0;
		if (DEBUG)
			printf("IPP_REJECT_JOBS\n");
		break;
	case 9:		/* accept jobs */
		op = CUPS_ACCEPT_JOBS;
		job_id = 0;
		if (DEBUG)
			printf("IPP_ACCEPT_JOBS\n");
		break;

	default:
		return;
		break;
	}
	response = NULL;

	if (dialog != (GtkWidget *) NULL)
		gtk_widget_destroy(dialog);

	request = ippNew();
	ippSetOperation(request, op);
	ippSetRequestId(request, 1);
	language = cupsLangDefault();
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));
	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL, language->language);

	if ((strlen(ServerAddr) + 25) < MAX_URI) {
		z1 = strchr(PrinterNames[PrinterChoice], '/');
		z2 = strchr(PrinterNames[PrtDlgSel], '/');
		if (z1 != NULL)
			*z1 = (uintptr_t) NULL;
		if (z2 != NULL)
			*z2 = (uintptr_t) NULL;
		snprintf(uri, (size_t) MAX_URI, "ipp://%s/printers/%s",
			 ServerAddr, PrinterNames[PrinterChoice]);
		snprintf(touri, (size_t) MAX_URI, "ipp://%s/printers/%s",
			 ServerAddr, PrinterNames[PrtDlgSel]);
		if (z1 != NULL)
			*z1 = '/';
		if (z2 != NULL)
			*z2 = '/';
		if (DEBUG) {
			printf("Modify-Uri: %s\n", uri);
			printf("Modify-ToUri: %s\n", touri);
			printf("Job-ID: %i\n", job_id);
		}
	} else {
		if (DEBUG)
			printf("MAXURI too short !\n");
		return;
	}

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
		     NULL, uri);
	ippAddInteger(request, IPP_TAG_OPERATION, IPP_TAG_INTEGER, "job-id",
		      job_id);

	if ((cancelmode == 5) || (cancelmode == 8)) {
		if (reason[0] != (uintptr_t) NULL)
			ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_TEXT,
				     "printer-state-message", NULL, reason);
	}

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
		     "requesting-user-name", NULL, cupsUser());

	if (op == CUPS_MOVE_JOB) {
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
			     "job-printer-uri", NULL, touri);
	}

	/*
	 * Do the request and get back a response...
	 */

	if ((op == IPP_PURGE_JOBS) || (op == IPP_PAUSE_PRINTER)
	    || (op == IPP_RESUME_PRINTER) || (op == CUPS_ACCEPT_JOBS)
	    || (op == CUPS_REJECT_JOBS)) {
		response = cupsDoRequest(cupsHttp, request, "/admin/");
	} else
		response = cupsDoRequest(cupsHttp, request, "/jobs/");

	if (response != NULL) {
		switch (ippGetStatusCode(response)) {
		case IPP_NOT_FOUND:
			quick_message(str2str(_("Job or printer not found!")),
				      2);
			break;
		case IPP_NOT_AUTHORIZED:
			quick_message(str2str(_("Authorization failed!")), 2);
			break;
		case IPP_FORBIDDEN:
			quick_message(str2str(_("You don't own this job!")), 2);
			break;
		default:
			if (ippGetStatusCode(response) > IPP_OK_CONFLICT)
				quick_message(str2str(_("Unable to do so!")),
					      2);
			break;
		}

		ippDelete(response);
	} else {
		if (op == IPP_PURGE_JOBS) {
			quick_message(str2str(_("Unable to cancel job(s)!")),
				      2);
		} else {
			if (op == IPP_PAUSE_PRINTER) {
				quick_message(str2str
					      (_("Unable to cancel printer!")),
					      2);
			} else {
				if (op == IPP_RESUME_PRINTER) {
					quick_message(str2str
						      (_
						       ("Unable to start printer!")),
						      2);
				} else {
					if (op == CUPS_ACCEPT_JOBS) {
						quick_message(str2str
							      (_
							       ("Unable to accept jobs for this printer!")),
							      2);
					} else {
						if (op == CUPS_REJECT_JOBS) {
							quick_message(str2str
								      (_
								       ("Unable to reject jobs for this printer!")),
								      2);
						} else {
							quick_message(str2str
								      (_
								       ("Unable to do so!")),
								      2);
						}
					}
				}
			}
		}
		return;
	}

	/*
	 * If nothing has been cancelled yet, cancel the current job on the specified
	 * (or default) printer...
	 */

	if (nox == 0)
		printQueueUpdate(NULL, NULL);

}
