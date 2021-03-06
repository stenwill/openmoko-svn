/**
 *  @file callbacks.c
 *  @brief The Main Menu in the Openmoko
 *
 *  Authored by Sun Zhiyong <sunzhiyong@fic-sh.com.cn>
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
 */

#include <libmokoui/moko-application.h>
#include <libmokoui/moko-window.h>
#include <libmokoui/moko-finger-window.h>
#include <libmokoui/moko-finger-wheel.h>
#include <libmokoui/moko-finger-tool-box.h>
#include <libmokoui/moko-pixmap-button.h>

#include "mainmenu.h"
#include "callbacks.h"
#include "mokoiconview.h"
#include "mokodesktop_item.h"
#include "fingermenu.h"

static void moko_cb_run_app (const char * data);

void
moko_wheel_bottom_press_cb (GtkWidget *self, MokoFingerMenu *fm)
{
  MokoDesktopItem *current = NULL;

  current = moko_finger_menu_get_current_item (fm);

  if (current->type != ITEM_TYPE_ROOT)
  {
    current = mokodesktop_item_get_parent(current);
    moko_finger_menu_update_content (fm, current);
  }
  else
  {
    //gtk_window_iconify (GTK_WINDOW (mma->window));
	moko_finger_menu_hide(fm);
	g_debug ("Call moko finger menu hide");
	moko_dbus_send_message ("");
  }
}

void
moko_wheel_left_up_press_cb (GtkWidget *self, MokoFingerMenu *fm)
{
    //moko_icon_view_move_cursor_line_up (mma->fm->mm->icon_view);
  moko_finger_menu_move_cursor_up(fm);
}

void
moko_wheel_right_down_press_cb (GtkWidget *self, MokoFingerMenu *fm)
{
 //   moko_icon_view_move_cursor_line_down (mma->mm->icon_view);
  moko_finger_menu_move_cursor_down(fm);
}

void
moko_icon_view_item_acitvated_cb(MokoIconView *icon_view,
				GtkTreePath *path, MokoFingerMenu *fm)
{
    MokoDesktopItem *selected_item = NULL;
	GtkTreeModel *tree_model;
	GtkTreeIter iter;

	tree_model = moko_icon_view_get_model (icon_view);
	gtk_tree_model_get_iter (tree_model, &iter, path);
	gtk_tree_model_get (tree_model, &iter, OBJECT_COLUMN, &selected_item, -1);

    if (selected_item->type == ITEM_TYPE_FOLDER)
    {
      moko_finger_menu_update_content (fm, selected_item);
    }
    else if (selected_item->type == ITEM_TYPE_DOTDESKTOP_ITEM ||selected_item->type == ITEM_TYPE_APP)
    {
		GdkPixbuf *pixbuf;
		moko_cb_run_app (selected_item->data);

		gtk_tree_model_get (tree_model, &iter, PIXBUF_COLUMN, &pixbuf, -1);

		if (pixbuf)
	      moko_finger_menu_set_app_history(fm, pixbuf, selected_item);
    }
}

void
moko_icon_view_selection_changed_cb(MokoIconView *iconview,
		MokoFingerMenu *fm)
{
    GList *selected_item;
    GtkTreeIter iter;
    GtkTreeModel *icon_view_model;
    gchar *text;

    selected_item = moko_icon_view_get_selected_items (iconview);

    if (!selected_item)
    {
        moko_window_set_status_message (fm->window, "Openmoko main menu");
        return;
    }

    icon_view_model = moko_icon_view_get_model (iconview);
    gtk_tree_model_get_iter (icon_view_model, &iter, selected_item->data);
    gtk_tree_model_get (icon_view_model, &iter, TEXT_COLUMN , &text, -1);

    if (text)
	{
		moko_window_set_status_message (fm->window, text);
	}

	g_list_foreach (selected_item, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (selected_item);

    moko_main_menu_update_item_total_label (fm->mm);
}

void
moko_tool_box_btn_clicked_cb (GtkButton *btn, MokoAppHistory *history)
{
	MokoDesktopItem *selected_item = NULL;
	gint i = 0;

    if (!btn || !history)
		return;

	for ( ; i<MAX_RECORD_APP; i++)
	{
		if (history->btn[i] == (MokoPixmapButton *)btn)
		{
			selected_item = history->item[i];
			break;
		}
		else
			continue;
	}

	if (selected_item)
	{
		moko_cb_run_app (selected_item->data);
	}
}

void
moko_stylus_menu_activate_item (GtkMenuItem *menuitem, gpointer data)
{
  moko_cb_run_app (data);
}

void
moko_cb_run_app (const char * data)
{
	switch (fork())
	{
		case 0 :
			system (data);
			exit (1);
		case -1 :
			break;
	}
}
