/*
 *  @file appmanager-window.c
 *  @brief The application manager in the Openmoko
 *
 *  Copyright (C) 2006-2007 OpenMoko Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
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

#include <gtk/gtk.h>

#include "appmanager-window.h"
#include "application-menu.h"
#include "filter-menu.h"
#include "navigation-area.h"
#include "tool-box.h"
#include "detail-area.h"
#include "appmanager-data.h"
#include "errorcode.h"
#include "package-list.h"
#include "select-menu.h"

/*
 * @brief The start function.
 */
int
main (int argc, char* argv[])
{
  ApplicationManagerData *appdata;
  GtkWidget       *window;
  GtkWidget       *menubox;
  GtkWidget       *menuitem;
  GtkWidget       *appmenu;
  GtkWidget       *filtermenu;
  GtkWidget       *selectmenu;
  GtkWidget       *navigation;
  GtkWidget       *toolbox;
  GtkWidget       *detail;

  //GtkWidget       *menubox;
  GtkWidget *vbox;

  gint             ret;

  g_debug ("application manager start up");

  gtk_init (&argc, &argv);
  g_set_application_name( _("Application Manager") );

  if (argc != 1)
    {
      /* Add init code. */
    }

  appdata = application_manager_data_new ();
  if (appdata == NULL)
    {
      g_debug ("Create main data struct error. Abort.");
      return -1;
    }

  init_pixbuf_list (appdata);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (gtk_main_quit), NULL);
  application_manager_data_set_main_window (appdata, GTK_WINDOW (window));

  /* main vbox */
  vbox = gtk_vbox_new (FALSE, 0);

  appmenu = application_menu_new (appdata);

  filtermenu = filter_menu_new (appdata);
  application_manager_data_set_filter_menu (appdata, GTK_MENU (filtermenu));

  selectmenu = moko_select_menu_new (appdata);
  application_manager_data_set_select_menu (appdata, GTK_MENU (selectmenu));


  /* Save the menubox to the application manager data */
  menubox = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), menubox, FALSE, FALSE, 0);

  application_manager_data_set_menubox (appdata, menubox);


  menuitem = gtk_menu_item_new_with_label ("Package");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), appmenu);
  gtk_menu_shell_append (GTK_MENU_SHELL (menubox), menuitem);

  menuitem = gtk_menu_item_new_with_label ("Filter");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), filtermenu);
  gtk_menu_shell_append (GTK_MENU_SHELL (menubox), menuitem);

  menuitem = gtk_menu_item_new_with_label ("Select");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), selectmenu);
  gtk_menu_shell_append (GTK_MENU_SHELL (menubox), menuitem);

  toolbox = tool_box_new (appdata);
  gtk_box_pack_start (GTK_BOX (vbox), toolbox, FALSE, FALSE, 0);
  //moko_paned_window_add_toolbox (window, toolbox);

  navigation = navigation_area_new (appdata);
  gtk_box_pack_start (GTK_BOX (vbox), navigation, TRUE, TRUE, 0);
  //moko_paned_window_set_navigation_pane (window, navigation);

  detail = detail_area_new (appdata);
  gtk_box_pack_start (GTK_BOX (vbox), detail, TRUE, TRUE, 0);
  //moko_paned_window_set_details_pane (window, detail);
  gtk_container_add (GTK_CONTAINER (appdata->mwindow), vbox);

  /* Load the list of all package in the memory */
  ret = init_package_list (appdata);
  if (ret != OP_SUCCESS)
    {
      g_debug ("Can not initialize libipkg, result was %d, aborting.", ret);
      return -1;
    }
  ret = package_list_build_index (appdata);
  if (ret != OP_SUCCESS)
    {
      g_debug ("Can not build index for packages, aborting.");
      return -1;
    }

  /* Add section list to the filter menu */
  package_list_add_section_to_filter_menu (appdata);

  /* Show the installed package list at initialization */
  filter_menu_show_install_list (appdata);

  gtk_widget_show_all (GTK_WIDGET (window));
  g_debug ("application manager enter main loop");
  gtk_main ();

  return 0;
}
