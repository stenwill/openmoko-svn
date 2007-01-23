/*
 *  openmoko-mainmenu
 *
 *  Authored by Sun Zhiyong <sunzhiyong@fic-sh.com.cn>
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
 */

#include <libmokoui/moko-application.h>
#include <libmokoui/moko-finger-window.h>
#include <libmokoui/moko-finger-wheel.h>
#include <libmokoui/moko-finger-tool-box.h>
#include <libmokoui/moko-pixmap-button.h>

#include "mainmenu.h"
#include "callbacks.h"
#include "mokoiconview.h"
#include "mokodesktop_item.h"
#include "app-history.h"


void
moko_wheel_bottom_press_cb (GtkWidget *self, MokoMainmenuApp *mma)
{
  if (mma->mm->current->type != ITEM_TYPE_ROOT)
  {
    mma->mm->current = mokodesktop_item_get_parent(mma->mm->current);
    moko_main_menu_update_content (mma->mm, mma->mm->current);
    gtk_window_present (mma->window);
  }
  else 
  {
    gtk_widget_hide (GTK_WIDGET (mma->wheel));
    gtk_widget_hide (GTK_WIDGET (mma->toolbox));
    gtk_widget_hide (GTK_WIDGET (mma->window));
  }
}

void
moko_wheel_left_up_press_cb (GtkWidget *self, MokoMainmenuApp *mma)
{
 
  g_signal_emit_by_name (G_OBJECT(mma->mm->icon_view), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, -1);
  //gtk_window_present (mma->window);
  //gtk_widget_grab_focus (mma->mm->icon_view);

}

void
moko_wheel_right_down_press_cb (GtkWidget *self, MokoMainmenuApp *mma)
{
  //gtk_widget_grab_focus (mma->mm->icon_view);
  g_signal_emit_by_name (G_OBJECT(mma->mm->icon_view), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, 1);
}

void 
moko_up_btn_cb (GtkButton *button, MokoMainMenu *mm)
{
  gtk_widget_grab_focus (mm->icon_view);
  g_signal_emit_by_name (G_OBJECT(mm->icon_view), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, -1);
}

void 
moko_down_btn_cb (GtkButton *button, MokoMainMenu *mm)
{
  gtk_widget_grab_focus (mm->icon_view);
  g_signal_emit_by_name (G_OBJECT(mm->icon_view), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, 1);
}

void
moko_icon_view_item_acitvated_cb(GtkIconView *icon_view, 
						GtkTreePath *path, MokoMainmenuApp *mma) 
{
  g_debug ("call moko_item_acitvated_cb");
  MokoDesktopItem *select_item = mokodesktop_item_get_child (mma->mm->current);
  gint index, i;
  
  index = moko_icon_view_get_cursor_positon (icon_view);

  for (i = 1; i < index; i++)
  {
    select_item = mokodesktop_item_get_next_sibling (select_item);
  }

  g_debug ("select_item name %s TYPE is %d", select_item->name, select_item->type);

  if (select_item->type == ITEM_TYPE_FOLDER)
  {
    mma->mm->current = select_item;
    g_debug ("current name %s------------------", mma->mm->current->name);
    moko_main_menu_update_content (mma->mm, select_item);
  }
  else if (select_item->type == ITEM_TYPE_DOTDESKTOP_ITEM ||select_item->type == ITEM_TYPE_APP)
  {
   switch (fork())
    {
        case 0:
      mb_exec((char *)select_item->data);
      fprintf(stderr, "exec failed, cleaning up child\n");
      exit(1);
    case -1:
      fprintf(stderr, "can't fork\n");
      break;
    }
     char path[512];
     snprintf (path, 512, "%s/%s", PIXMAP_PATH, select_item->icon_name);
     g_debug ("-------select_item path: %s", path);
      moko_hisory_app_fill(mma->history, path);
  }

moko_icon_view_selection_changed_cb(mma->mm->icon_view, mma);  

}

void
moko_icon_view_selection_changed_cb(GtkIconView *iconview, 
								MokoMainmenuApp *mma) 
{
    moko_main_menu_update_item_total_label (mma->mm);
}

