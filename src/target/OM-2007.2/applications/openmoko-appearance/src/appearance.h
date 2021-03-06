/*
 * OpenMoko Appearance - Change various appearance related settings
 *
 * Copyright (C) 2007 by OpenMoko, Inc.
 * Written by OpenedHand Ltd <info@openedhand.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef APPEARANCE_H
#define APPEARANCE_H

#define GCONF_POKY_INTERFACE_PREFIX "/desktop/poky/interface"
#define GCONF_POKY_WALLPAPER "/wallpaper"

typedef struct
{
  GtkWidget *window;

  /* colours page */
  GtkWidget *colors_combo;

  /* background page */
  GtkWidget *bg_ebox;
  GtkWidget *bg_chooser;
  GdkPixmap *wallpaper;

} AppearanceData;

#endif
