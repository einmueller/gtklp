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

#include "ppd.h"

#include <string.h>
#include <gtk/gtk.h>
#include <cups/ppd.h>

#include <gettext.h>
#include <defaults.h>
#include <libgtklp.h>

#include "gtklp.h"
#include "gtklp_functions.h"

GtkWidget *FrameHBox, *FrameVBox;
GtkWidget *ppdTab, *ppdFrame;

int ppd_entry_count;

int togglePPDOption(ppd_group_t * group, int ct, int nummer, int enable)
{
	int i1, i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for (i1 = group->num_options, option = group->options; i1 * ct > 0;
	     i1--, option++) {
		if ((strcmp(option->keyword, "PageSize") != 0) && (strcmp(option->keyword, "InputSlot") != 0) && (strcmp(option->keyword, "MediaType") != 0) && (strcmp(option->keyword, "Duplex") != 0) && (strcmp(option->keyword, "Copies") != 0) && (strcmp(option->keyword, "Collate") != 0) && (strcmp(option->keyword, "PageRegion") != 0)) {	/* PageRegion seems to be the same as PageSize in Cups */
			for (i2 = option->num_choices, choice = option->choices;
			     i2 * ct > 0; i2--, choice++) {
				if (nummer == ct) {
					g_print
					    ("\nChanged(enable=%i): %i: %s(%s)=%s(%s)\n",
					     enable, ct, option->keyword,
					     option->text, choice->choice,
					     choice->text);
					if (enable == 1) {
						constraints(ppdMarkOption
							    (printerPPD,
							     option->keyword,
							     choice->choice));
					} else {
						choice->marked = 0;
						constraints(ppdConflicts
							    (printerPPD));
					}
					ct = -1;
				} else {
					ct++;
				}
			}
		}
	}

	for (i1 = group->num_subgroups, subgroup = group->subgroups;
	     i1 * ct > 0; i1--, subgroup++)
		ct = togglePPDOption(group, ct, nummer, enable);

	return (ct);
}

void enablePPDOption(int nummer)
{
	int i1;
	int ct;
	ppd_group_t *group;

	g_print("Enable %i\n", nummer);

	ct = 1;
	for (i1 = printerPPD->num_groups, group = printerPPD->groups;
	     i1 * ct > 0; i1--, group++)
		ct = togglePPDOption(group, ct, nummer, 1);

}

void disablePPDOption(int nummer)
{
	int i1;
	int ct;
	ppd_group_t *group;

	g_print("Disable %i\n", nummer);

	ct = 1;
	for (i1 = printerPPD->num_groups, group = printerPPD->groups;
	     i1 * ct > 0; i1--, group++)
		ct = togglePPDOption(group, ct, nummer, 0);

}

void PPD_Multi_changed(GtkWidget * widget, int data)
{
	g_print("PPD_Multi changed %s -> %i\n",
		gtk_widget_get_name(widget), data);
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		enablePPDOption(data);
	else
		disablePPDOption(data);
}

void addPPDGroup(ppd_group_t * group)
{
	int i1, i2;
	int marked_choice, default_choice;
	int morethanone;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;
	GtkWidget *option_box, *label = NULL, *group_frame, *group_box;
	GtkWidget *ppdWidget, *frame = NULL, *vbox = NULL;
	GList *list;
	unsigned thiswidth, maxwidth;

	morethanone = 0;
	maxwidth = 0;

	/* Option types: PPD_UI_BOOLEAN PPD_UI_PICKONE PPD_UI_PICKMANY */

	if (DEBUG)
		printf("--- Adding Group %s: \n", group->text);

	group_frame = gtk_frame_new(str2str(group->text));
	gtk_box_pack_start(GTK_BOX(FrameVBox), group_frame, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(group_frame);

	group_box = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(group_frame), group_box);
	gtk_container_set_border_width(GTK_CONTAINER(group_box), VBOX_BORDER);
	gtk_widget_show(group_box);

	for (i1 = 0, option = group->options;
	     i1 < group->num_options; i1++, option++) {
		if ((strcmp(option->keyword, "PageSize") != 0) && (strcmp(option->keyword, "InputSlot") != 0) && (strcmp(option->keyword, "MediaType") != 0) && (strcmp(option->keyword, "Duplex") != 0) && (strcmp(option->keyword, "Copies") != 0) && (strcmp(option->keyword, "Collate") != 0) && (strcmp(option->keyword, "PageRegion") != 0)) {	/* PageRegion seems to be the same as PageSize in Cups */
			if (DEBUG)
				printf
				    ("Creating widget for <%s> - '%s' from type %i\n",
				     option->keyword, option->text, option->ui);

			morethanone = 1;
			option_box = gtk_hbox_new(FALSE, 8);
			gtk_box_pack_start(GTK_BOX(group_box), option_box,
					   FALSE, FALSE, 1);
			gtk_widget_show(option_box);

			list = NULL;

			marked_choice = -1;
			default_choice = -1;

			for (i2 = 0, choice = option->choices;
			     i2 < option->num_choices; i2++, choice++) {
				ppd_entry_count++;

				switch (option->ui) {
				case PPD_UI_BOOLEAN:
				case PPD_UI_PICKONE:
					if (choice->text == NULL) {
						list =
						    g_list_append(list,
								  str2str
								  (choice->
								   choice));
#if GTK_MAJOR_VERSION == 1
						thiswidth =
						    gdk_string_width
						    (gtk_widget_get_style
						     (option_box)->font,
						     str2str(choice->choice));
#else
						thiswidth =
						    gdk_string_width
						    (gtk_style_get_font
						     (gtk_widget_get_style
						      (option_box)),
						     str2str(choice->choice));
#endif
					} else {
						list =
						    g_list_append(list,
								  str2str
								  (choice->
								   text));
						// The default font is used in every widget!
#if GTK_MAJOR_VERSION == 1
						thiswidth =
						    gdk_string_width
						    (gtk_widget_get_style
						     (option_box)->font,
						     str2str(choice->text));
#else
						thiswidth =
						    gdk_string_width
						    (gtk_style_get_font
						     (gtk_widget_get_style
						      (option_box)),
						     str2str(choice->text));
#endif
					}
					if (thiswidth > maxwidth)
						maxwidth = thiswidth;

					if (strcmp
					    (str2str(choice->text),
					     str2str(option->defchoice)) == 0)
						default_choice = i2;

					if (choice->marked)
						marked_choice = i2;

					break;
				case PPD_UI_PICKMANY:
					if ((strcmp(choice->text, "None") == 0)
					    || (strcmp(choice->choice, "None")
						== 0))
						break;

					if (vbox == NULL) {
						vbox =
						    gtk_vbox_new(FALSE,
								 INFRAME_SPACING_V);
						gtk_container_set_border_width
						    (GTK_CONTAINER(vbox),
						     VBOX_BORDER);
						gtk_widget_show(vbox);
					}

					if (choice->text == NULL)
						ppdWidget =
						    gtk_check_button_new_with_label
						    (choice->choice);
					else
						ppdWidget =
						    gtk_check_button_new_with_label
						    (choice->text);

					if (choice->marked)
						gtk_toggle_button_set_active
						    (GTK_TOGGLE_BUTTON
						     (ppdWidget), TRUE);

					gtk_box_pack_start(GTK_BOX(vbox),
							   ppdWidget, FALSE,
							   FALSE, 0);
					gtk_signal_connect(GTK_OBJECT
							   (ppdWidget),
							   "toggled",
							   GTK_SIGNAL_FUNC
							   (PPD_Multi_changed),
							   GINT_TO_POINTER(ppd_entry_count));
					gtk_widget_show(ppdWidget);
					break;
				default:
					if (DEBUG)
						printf
						    ("Unknown OpenGroup-Type at 1 !\n");
					emergency();
					break;
				}

				if (choice->marked) {
					if (DEBUG)
						printf("Default: %s\n",
						       choice->choice);
				}
			}

			switch (option->ui) {
			case PPD_UI_BOOLEAN:
			case PPD_UI_PICKONE:
				label = gtk_label_new(str2str(option->text));
				gtk_box_pack_start(GTK_BOX(option_box), label,
						   TRUE, TRUE, 1);
				gtk_misc_set_alignment(GTK_MISC(label), 1.0,
						       0.5);
				gtk_widget_show(label);

				ppdWidget = gtk_combo_new();

				if (list == (GList *) NULL)
					list = g_list_append(list, "");

				gtk_combo_set_popdown_strings(GTK_COMBO
							      (ppdWidget),
							      list);
				gtk_combo_set_value_in_list(GTK_COMBO
							    (ppdWidget), TRUE,
							    FALSE);
				gtk_list_set_selection_mode(GTK_LIST
							    (GTK_COMBO
							     (ppdWidget)->list),
							    GTK_SELECTION_SINGLE);
				gtk_entry_set_editable(GTK_ENTRY
						       (GTK_COMBO(ppdWidget)->
							entry), FALSE);

				if (default_choice >= 0)
					gtk_list_select_item(GTK_LIST
							     (GTK_COMBO
							      (ppdWidget)->
							      list),
							     default_choice);

				if (marked_choice >= 0)
					gtk_list_select_item(GTK_LIST
							     (GTK_COMBO
							      (ppdWidget)->
							      list),
							     marked_choice);

				gtk_box_pack_start(GTK_BOX(option_box),
						   ppdWidget, FALSE, FALSE, 0);
				gtk_signal_connect(GTK_OBJECT
						   (GTK_COMBO(ppdWidget)->
						    entry), "changed",
						   GTK_SIGNAL_FUNC
						   (PPD_DropDown_changed),
						   str2str(option->keyword));

				gtk_widget_set_size_request(GTK_WIDGET
							    (GTK_ENTRY
							     (GTK_COMBO
							      (ppdWidget)->
							      entry)),
							    max(maxwidth +
								COMBO_EXTRA_SPACE,
								COMBO_MIN_SPACE),
							    -1);
				gtk_widget_show(ppdWidget);

				break;
			case PPD_UI_PICKMANY:
				frame = gtk_frame_new(str2str(option->text));
				gtk_container_set_border_width(GTK_CONTAINER
							       (frame), 5);
				gtk_box_pack_start(GTK_BOX(option_box), frame,
						   TRUE, TRUE, 1);
				gtk_widget_show(frame);

				gtk_container_add(GTK_CONTAINER(frame), vbox);
				vbox = NULL;
				break;
			default:
				if (DEBUG)
					printf
					    ("Unknown OpenGroup-Type at 2 !\n");
				emergency();
				break;
			}

		} else {
			if (DEBUG)
				printf("Ignoring: %s\n", option->keyword);
		}
	}

	if (morethanone == 0) {
		gtk_widget_destroy(group_frame);
		return;
	}

	for (i1 = group->num_subgroups, subgroup = group->subgroups; i1 > 0;
	     i1--, subgroup++)
		addPPDGroup(subgroup);
}

void ppdTabSetDefaults(void)
{
	int i1;
	ppd_group_t *group;

	if (DEBUG)
		printf("Get ppdDefaults\n");
	gtk_widget_destroy(FrameVBox);
	FrameVBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(FrameHBox), FrameVBox, TRUE, TRUE,
			   FRAME_SPACING_V);
	gtk_widget_show(FrameVBox);

	ppd_entry_count = 0;

	if (printerPPD != NULL) {
		for (i1 = printerPPD->num_groups, group = printerPPD->groups;
		     i1 > 0; i1--, group++)
			addPPDGroup(group);
	}
}

void PPDFrame(GtkWidget * container)
{
	GtkWidget *frameScrolled;

	frameScrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frameScrolled),
				       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(container), frameScrolled, TRUE, TRUE, 0);
	gtk_widget_show(frameScrolled);

	FrameHBox = gtk_hbox_new(FALSE, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW
					      (frameScrolled), FrameHBox);
	gtk_widget_show(FrameHBox);

	FrameVBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(FrameHBox), FrameVBox, FALSE, FALSE,
			   FRAME_SPACING_V);
	gtk_widget_show(FrameVBox);

}

void PPDTab(void)
{
	ppdTab = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), ppdTab,
				 gtk_label_new(str2str(_("PPD"))));
	PPDFrame(ppdTab);

}
