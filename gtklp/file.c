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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include <sys/types.h>
#include <dirent.h>

#include <gettext.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "file.h"

GtkWidget *fileTab, *fileRemove, *fileRemoveAll, *fileAdd;
GtkWidget *fileFrame, *fileList;
GtkWidget *fileRequ;
int fileSelectedEntry;

int fileSelectDontSelect;

void fileTabSetDefaults(void)
{
	if (DEBUG)
		printf("Get fileDefaults\n");
	/* Buttons */
	gtk_widget_set_sensitive(fileRemove, FALSE);
	gtk_widget_set_sensitive(fileRemoveAll, FALSE);
	if (filesToPrintAnz >= MAXPRINTFILES)
		gtk_widget_set_sensitive(fileAdd, FALSE);
}

void fileSelected(GtkWidget * widget, gint row, gint column,
		  GdkEventButton * event, gpointer data)
{
	if (DEBUG)
		printf("file-entry selected: %i->%s\n", row, filesToPrint[row]);
	fileSelectedEntry = row;
	gtk_widget_set_sensitive(fileRemove, TRUE);
}

void fileListEmpty(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("file-list empty again !\n");
	gtk_widget_set_sensitive(fileRemoveAll, FALSE);
}

#ifdef OLDDIALOG
void fileSelDialogOk(GtkWidget * widget, GtkFileSelection * fs)
{
	gtk_widget_destroy(fileRequ);
}
#else
void fileSelDialogOk(GtkWidget * widget)
{
	GSList *z1, *z2;
	gchar *entry[1][1];

	z1 = gtk_file_chooser_get_filenames((GtkFileChooser *) widget);
	z2 = z1;
	while (z1 != (GSList *) NULL) {
		if (filesToPrintAnz >= MAXPRINTFILES) {
			gtk_widget_set_sensitive(fileAdd, FALSE);
		} else {
			strncpy(filesToPrint[(unsigned char)filesToPrintAnz],
				(char *)z1->data, (size_t) MAXPATH);
			entry[0][0] =
			    g_filename_to_utf8(filesToPrint
					       [(unsigned char)filesToPrintAnz],
					       -1, NULL, NULL, NULL);
			filesToPrintAnz++;
			gtk_clist_append(GTK_CLIST(fileList), entry[0]);

			g_free(entry[0][0]);
			if (filesToPrintAnz >= MAXPRINTFILES)
				gtk_widget_set_sensitive(fileAdd, FALSE);
			else
				gtk_widget_set_sensitive(fileRemoveAll, TRUE);
		}
		g_free(z1->data);
		z1 = z1->next;
	}
	g_slist_free(z2);

	strncpy(fileOpenPath,
		gtk_file_chooser_get_current_folder((GtkFileChooser *) widget),
		(size_t) MAXPATH);

	gtk_widget_destroy(widget);
}
#endif

#ifdef OLDDIALOG
void fileSelDialogDestroyed(GtkWidget * widget, gpointer data)
{
#if GTK_MAJOR_VERSION != 1
	gchar **z1;
	int i1;
	gchar *entry[1][1];
	DIR *dir;
#endif
	if (DEBUG)
		printf("File-Requester destroyed !\n");

#if GTK_MAJOR_VERSION != 1
	z1 = gtk_file_selection_get_selections((GtkFileSelection *) widget);
	i1 = 0;
	do {
		if (DEBUG)
			printf("File selected: %s\n", z1[i1]);

		dir = opendir(z1[i1]);
		if (dir != (DIR *) NULL) {
			if (DEBUG)
				printf("Try to select whole DIRECTORY!!!\n");
			closedir(dir);
		} else {
			if (filesToPrintAnz >= MAXPRINTFILES) {
				gtk_widget_set_sensitive(fileAdd, FALSE);
			} else {
				strncpy(filesToPrint[filesToPrintAnz], z1[i1],
					(size_t) MAXPATH);
				filesToPrintAnz++;
				entry[0][0] =
				    g_filename_to_utf8(z1[i1], -1, NULL, NULL,
						       NULL);
				gtk_clist_append(GTK_CLIST(fileList), entry[0]);
				g_free(entry[0][0]);
				gtk_widget_set_sensitive(fileRemoveAll, TRUE);
			}
		}
		i1++;
	} while (z1[i1] != (char *)NULL);
	g_strfreev(z1);
#endif

	gtk_window_set_modal(GTK_WINDOW(widget), FALSE);
	gtk_window_set_modal(GTK_WINDOW(mainWindow), TRUE);

}
#endif

void filesel(GtkCList * clist, gint row, gint column, GdkEventButton * event,
	     gpointer user_data)
{
#if GTK_MAJOR_VERSION == 1
	char tmp[MAXPATH + 1];
	char *z1, *z2;
	gchar *entry[1][1];
	int i1, pos;

	if (DEBUG)
		printf("filesel\n");

	if (fileSelectDontSelect == 1) {	/* Event was initiated from myself... */
		fileSelectDontSelect = 2;
		return;
	}

	/* Build the right path */
	gtk_clist_get_text(GTK_CLIST
			   (((GtkFileSelection *) fileRequ)->file_list), row, 0,
			   &z2);
	strncpy(tmp,
		gtk_file_selection_get_filename(GTK_FILE_SELECTION(fileRequ)),
		(size_t) MAXPATH);
	i1 = strlen(gtk_entry_get_text
		    (GTK_ENTRY
		     ((((GtkFileSelection *) fileRequ)->selection_entry))));
	z1 = tmp + strlen(tmp);
	z1 -= i1;
	*z1 = (uintptr_t) NULL;
	strncat(tmp, z2, (size_t) MAXPATH);

	pos = -1;

	if (DEBUG)
		printf("File selected: %s\n", tmp);

	if (strcmp((char *)user_data, "SEL") == 0) {
		strncpy(filesToPrint[filesToPrintAnz], tmp, (size_t) MAXPATH);
		filesToPrintAnz++;
		if (filesToPrintAnz >= MAXPRINTFILES)
			gtk_widget_set_sensitive(fileAdd, FALSE);

		entry[0][0] = tmp;
		gtk_clist_append(GTK_CLIST(fileList), entry[0]);
	} else {
		fileRemoveFromList(tmp);
	}

	if (DEBUG)
		printf("Path selected: %s\n", tmp);
	strncpy(fileOpenPath, tmp, (size_t) MAXPATH);	/* Remember Path for next time "Add" is pressed */
	if (DEBUG) {
		printf("FilesToPrint: \n");
		for (i1 = 0; i1 < filesToPrintAnz; i1++)
			printf("  %s\n", filesToPrint[i1]);
	}

	if (filesToPrintAnz > 0)
		gtk_widget_set_sensitive(fileRemoveAll, TRUE);
	else
		gtk_widget_set_sensitive(fileRemoveAll, FALSE);
#else
	return;
#endif
}

void dirChanged(GtkWidget * widget, gpointer data)
{
#if GTK_MAJOR_VERSION == 1
	char tmppath[MAXPATH + 1];
	char *z1;
	int i1, i2;

	if (fileSelectDontSelect == 2) {	/* This signal was initiated by myself... */
		fileSelectDontSelect = 0;
		return;
	}

	if (*gtk_entry_get_text(GTK_ENTRY(widget)) == 0) {

		for (i1 = 0; i1 < filesToPrintAnz; i1++) {
			i2 = 0;
			while (gtk_clist_get_text
			       (GTK_CLIST
				(((GtkFileSelection *) fileRequ)->file_list),
				i2, 0, &z1) == 1) {
				snprintf(tmppath, (size_t) MAXPATH, "%s%s",
					 gtk_file_selection_get_filename
					 (GTK_FILE_SELECTION(fileRequ)), z1);
				if (strcmp(tmppath, filesToPrint[i1]) == 0) {
					fileSelectDontSelect = 1;
					gtk_clist_select_row(GTK_CLIST
							     (((GtkFileSelection
								*) fileRequ)->
							      file_list), i2,
							     0);
					gtk_entry_set_text(GTK_ENTRY(widget),
							   "");
				}
				i2++;
			}
		}
	}
#else
	return;
#endif
}

void fileAddPressed(GtkWidget * widget, gpointer data)
{
	if (DEBUG)
		printf("Add request\n");

	fileSelectDontSelect = 0;

#ifdef OLDDIALOG
	fileRequ = gtk_file_selection_new(str2str(_("Print File...")));
#else
	fileRequ =
	    gtk_file_chooser_dialog_new(str2str(_("Print File...")),
					GTK_WINDOW(mainWindow),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
#endif

#if GTK_MAJOR_VERSION == 1
	gtk_clist_set_selection_mode(GTK_CLIST
				     (((GtkFileSelection *) fileRequ)->
				      file_list), GTK_SELECTION_MULTIPLE);
#else
#ifdef OLDDIALOG
	gtk_file_selection_set_select_multiple((GtkFileSelection *) fileRequ,
					       TRUE);
#else
	gtk_file_chooser_set_select_multiple((GtkFileChooser *) fileRequ, TRUE);
#endif
#endif

#ifdef OLDDIALOG
	gtk_widget_set_sensitive((GtkWidget *) GTK_BUTTON(((GtkFileSelection *) fileRequ)->cancel_button), FALSE);	/* Problems with cancel on Multiselection */
#endif
#if GTK_MAJOR_VERSION == 1
	gtk_signal_connect(GTK_OBJECT
			   (GTK_CLIST
			    (((GtkFileSelection *) fileRequ)->file_list)),
			   "select-row", (GtkSignalFunc) filesel, "SEL");
	gtk_signal_connect(GTK_OBJECT
			   (GTK_CLIST
			    (((GtkFileSelection *) fileRequ)->file_list)),
			   "unselect-row", (GtkSignalFunc) filesel, "UNSEL");

	gtk_signal_connect(GTK_OBJECT
			   (GTK_ENTRY
			    (((GtkFileSelection *) fileRequ)->selection_entry)),
			   "changed", (GtkSignalFunc) dirChanged, NULL);
#endif

#ifdef OLDDIALOG
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(fileRequ),
					fileOpenPath);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(fileRequ));
	gtk_window_set_modal(GTK_WINDOW(fileRequ), TRUE);
	gtk_window_set_modal(GTK_WINDOW(mainWindow), FALSE);
	gtk_window_set_position(GTK_WINDOW(fileRequ), GTK_WIN_POS_CENTER);

	gtk_entry_set_editable(GTK_ENTRY
			       (((GtkFileSelection *) fileRequ)->
				selection_entry), FALSE);

	gtk_signal_connect(GTK_OBJECT(fileRequ), "destroy",
			   (GtkSignalFunc) fileSelDialogDestroyed, &fileRequ);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileRequ)->ok_button),
			   "clicked", (GtkSignalFunc) fileSelDialogOk,
			   fileRequ);

	gtk_widget_show(fileRequ);
#else
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileRequ),
					    fileOpenPath);
	gtk_window_set_position(GTK_WINDOW(fileRequ), GTK_WIN_POS_CENTER);
	switch (gtk_dialog_run(GTK_DIALOG(fileRequ))) {
	case GTK_RESPONSE_ACCEPT:
		fileSelDialogOk(fileRequ);
		break;
	case GTK_RESPONSE_CANCEL:
		gtk_widget_destroy(fileRequ);
		break;
	default:		/* should be destroy dialog */
		gtk_widget_destroy(fileRequ);
		break;
	}
#endif

}

void fileRemovePressed(GtkWidget * widget, gpointer data)
{
	int i1;

	if (DEBUG)
		printf("Remove request for entry %i->%s\n", fileSelectedEntry,
		       filesToPrint[fileSelectedEntry]);

	fileRemoveFromList(filesToPrint[fileSelectedEntry]);

	if (DEBUG) {
		printf("FilesToPrint: \n");
		for (i1 = 0; i1 < filesToPrintAnz; i1++)
			printf("  %s\n", filesToPrint[i1]);
	}
}

void fileRemoveAllPressed(GtkWidget * widget, gpointer data)
{
	int i1;

	if (DEBUG)
		printf("Remove All request\n");

	for (i1 = filesToPrintAnz - 1; i1 >= 0; i1--) {
		fileRemoveFromList(filesToPrint[i1]);
	}

	gtk_widget_set_sensitive(fileRemoveAll, FALSE);

}

void fileRemoveFromList(char *file)
{
	int i1, i2, i3;

	if (DEBUG)
		printf("Remove request for entry %s\n", file);

	i2 = filesToPrintAnz;	/* Better not use variables which were changed during "for" */
	for (i1 = 0; i1 < i2; i1++) {
		if (strcmp(filesToPrint[i1], file) == 0) {
			gtk_clist_remove(GTK_CLIST(fileList), i1);
			filesToPrintAnz--;
			for (i3 = i1; i3 < filesToPrintAnz; i3++)
				strncpy(filesToPrint[i3], filesToPrint[i3 + 1],
					(size_t) MAXPATH);
			i1 = i2;
		}
	}

	if (filesToPrintAnz < MAXPRINTFILES)
		gtk_widget_set_sensitive(fileAdd, TRUE);
	gtk_widget_set_sensitive(fileRemove, FALSE);

}

void FileFrame(GtkWidget * container)
{
	GtkWidget *mainHBox, *vbox, *buttonBox, *scrolled;
	gchar *entry[1][1];

	int i1;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	fileFrame = gtk_frame_new(str2str(_("Files to print")));
	gtk_box_pack_start(GTK_BOX(mainHBox), fileFrame, TRUE, TRUE,
			   FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, 8);
	gtk_container_add(GTK_CONTAINER(fileFrame), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_widget_show(vbox);

	scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
	gtk_widget_show(scrolled);

	fileList = gtk_clist_new(1);
	gtk_widget_set_size_request(fileList, -1, FILE_DIALOG_HEIGHT);
	gtk_container_add(GTK_CONTAINER(scrolled), fileList);
	gtk_clist_set_shadow_type(GTK_CLIST(fileList), GTK_SHADOW_OUT);

	for (i1 = 0; i1 < filesToPrintAnz; i1++) {
		entry[0][0] =
		    g_filename_to_utf8((gchar *) filesToPrint[i1], -1, NULL,
				       NULL, NULL);
		gtk_clist_append(GTK_CLIST(fileList), entry[0]);
		g_free(entry[0][0]);
	}
	fileSelectedEntry = -1;

	buttonBox = gtk_hbox_new(FALSE, BUTTON_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), buttonBox, FALSE, FALSE,
			   INFRAME_SPACING_V);
	gtk_widget_show(buttonBox);

#if GTK_MAJOR_VERSION != 1
	fileAdd = gtk_button_new_from_stock(GTK_STOCK_ADD);
#else
	fileAdd = gtk_button_new_with_label(str2str(_("Add")));
#endif
	button_pad(fileAdd);
	gtk_box_pack_start(GTK_BOX(buttonBox), fileAdd, FALSE, FALSE, 0);
	gtk_widget_show(fileAdd);

#if GTK_MAJOR_VERSION != 1
	fileRemove = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
#else
	fileRemove = gtk_button_new_with_label(str2str(_("Remove")));
#endif
	button_pad(fileRemove);
	gtk_box_pack_start(GTK_BOX(buttonBox), fileRemove, FALSE, FALSE, 0);
	gtk_widget_show(fileRemove);

#if GTK_MAJOR_VERSION != 1
	fileRemoveAll = gtk_button_new_from_stock(GTK_STOCK_CLEAR);
#else
	fileRemoveAll = gtk_button_new_with_label(str2str(_("Remove All")));
#endif
	button_pad(fileRemoveAll);
	gtk_box_pack_start(GTK_BOX(buttonBox), fileRemoveAll, FALSE, FALSE, 0);
	gtk_widget_show(fileRemoveAll);

	gtk_signal_connect(GTK_OBJECT(fileAdd), "released",
			   GTK_SIGNAL_FUNC(fileAddPressed), NULL);

	gtk_signal_connect(GTK_OBJECT(fileRemove), "released",
			   GTK_SIGNAL_FUNC(fileRemovePressed), NULL);
	gtk_signal_connect(GTK_OBJECT(fileRemoveAll), "released",
			   GTK_SIGNAL_FUNC(fileRemoveAllPressed), NULL);
	gtk_signal_connect(GTK_OBJECT(fileList), "select_row",
			   GTK_SIGNAL_FUNC(fileSelected), "NULL");

	gtk_widget_show(fileList);

}

void FileTab(void)
{
	fileTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(fileTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), fileTab,
				 gtk_label_new(str2str(_("File"))));

	FileFrame(fileTab);

}
