/**
 *  @file package-list.h
 *  @brief The package list that get from the lib ipkg
 *
 *  Copyright (C) 2006-2007 OpenMoko Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2.1 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Public License for more details.
 *
 *  Current Version: $Rev$ ($Date$) [$Author$]
 *
 *  @author Chaowei Song (songcw@fic-sh.com.cn)
 */
#ifndef _FIC_PACKAGE_LIST_H
#define _FIC_PACKAGE_LIST_H

#include <gtk/gtk.h>

#define PACKAGE_LIST_NO_SECTION_STRING "no section"

gint init_package_list (ApplicationManagerData *appdata);

gint reinit_package_list (ApplicationManagerData *appdata);

gint package_list_build_index (ApplicationManagerData *appdata);

void package_list_add_section_to_filter_menu (ApplicationManagerData *appdata);

void translate_package_list_to_store (ApplicationManagerData *appdata, 
                                      GtkListStore *store, 
                                      gpointer pkglist);

gpointer package_list_get_with_name (ApplicationManagerData *appdata,
                                     const gchar *name);

gint package_list_get_package_status (gpointer data);

void package_list_set_package_status (gpointer data, gint status);

void package_list_remove_package_from_selected_list (ApplicationManagerData *appdata,
                                                     gpointer pkg);

void package_list_add_node_to_selected_list (ApplicationManagerData *appdata,
                                             gpointer pkg);

char *package_list_get_package_version (gpointer pkg);

char *package_list_get_package_name (gpointer pkg);

char *package_list_get_package_depends (gpointer pkg);

char *package_list_get_package_description (gpointer pkg);

char *package_list_get_package_maintainer (gpointer pkg);

void search_and_translate_package_list_to_store (ApplicationManagerData *appdata, 
                                                 GtkListStore *store, 
                                                 gpointer pkglist,
                                                 const gchar *str);
gboolean 
package_list_check_marked_list_empty (ApplicationManagerData *appdata);

void package_list_fill_store_with_selected_list (GtkTreeStore *treestore,
                                                 gpointer *selectedlist,
                                                 gint column);
void package_list_mark_all_upgradeable (ApplicationManagerData *appdata);

gpointer package_list_execute_change (gpointer data);

void package_list_free_all_dynamic (ApplicationManagerData *appdata);

gint package_list_get_number_of_selected (ApplicationManagerData *appdata);

#endif

