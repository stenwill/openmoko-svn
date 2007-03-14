/**
 *  @file main.c
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

#include "callbacks.h"


#include "main.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define LOCK_FILE "/tmp/moko-mainmenu.lock"

static MokoMainmenuApp *mma;

static void 
handle_sigusr1 (int value)
{
  if (!mma)
       return;
  gtk_window_present (GTK_WINDOW(mma->window));
  moko_dbus_send_message ("Openmoko main menu");

  signal (SIGUSR1, handle_sigusr1);
}

static pid_t 
testlock (char *fname)
{
  int fd;
  struct flock fl;

  fd = open (fname, O_WRONLY, S_IWUSR);
  if (fd < 0)
    {
      if (errno == ENOENT)
        {
          return 0;
        }
      else
        {
          perror ("Test lock open file");
          return -1;
        }
    }

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;

  if (fcntl (fd, F_GETLK, &fl) < 0)
    {
      close (fd);
      return -1;
    }
  close (fd);

  if (fl.l_type == F_UNLCK)
    return 0;

  return fl.l_pid;
}

static void 
setlock (char *fname)
{
  int fd;
  struct flock fl;

  fd = open (fname, O_WRONLY|O_CREAT, S_IWUSR);
  if (fd < 0)
    {
      perror ("Set lock open file");
      return ;
    }

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;

  if (fcntl (fd, F_SETLK, &fl) < 0)
    {
      perror ("Lock file");
      close (fd);
    }
}

int 
main( int argc, char** argv ) 
{
    pid_t lockapp;

    lockapp = testlock (LOCK_FILE);

    if (lockapp > 0)
     {
        kill (lockapp, SIGUSR1);
        return 0;
     }

    setlock (LOCK_FILE);
    
    mma = g_malloc0 (sizeof (MokoMainmenuApp));
    if (!mma)
    {
        g_error ("openmoko-mainmenu application initialize FAILED.");
    	exit (0);
    }
    memset (mma, 0, sizeof (MokoMainmenuApp));

    if (!moko_dbus_connect_init ())
    {
        g_error ("Failed to initial dbus connection.");
		exit (0);
    }
    gtk_init( &argc, &argv );

    /* application object */
    mma->app = MOKO_APPLICATION(moko_application_get_instance());
    g_set_application_name( "OpenMoko Main Menu" );
    
    /* finger based window */
    mma->window = MOKO_FINGER_WINDOW(moko_finger_window_new());
    gtk_widget_show (GTK_WIDGET (mma->window));

    /* finger wheel object*/
    mma->wheel = MOKO_FINGER_WHEEL (moko_finger_window_get_wheel (mma->window));
    gtk_window_set_title (GTK_WINDOW (mma->wheel), "wheel");

    /* finger toolbox object*/
    mma->toolbox = MOKO_FINGER_TOOL_BOX (moko_finger_window_get_toolbox (mma->window));
    //initialize toolbox's buttons, which are MokoPixmapButton objects.
    mma->history = moko_app_history_init (mma->toolbox);
	if (!mma->history)
	{
        g_error ("Failed to get application history instance");
		exit (0);
    }

    /* MokoMainMenu object */
    mma->mm = MAINMENU (moko_main_menu_new ());

    /* signal connected*/
    //finger wheel object signals
    g_signal_connect (mma->wheel, "press_bottom",
    		G_CALLBACK ( moko_wheel_bottom_press_cb), mma);
    g_signal_connect (mma->wheel, "press_left_up",
    		G_CALLBACK ( moko_wheel_left_up_press_cb), mma);
    g_signal_connect (mma->wheel, "press_right_down",
    		G_CALLBACK ( moko_wheel_right_down_press_cb), mma);
    //MokoMainMenu:MokoIconView object signals
    g_signal_connect (mma->mm->icon_view, "selection-changed",
    		G_CALLBACK (moko_icon_view_selection_changed_cb), mma);
    g_signal_connect (mma->mm->icon_view, "item_activated",
    		G_CALLBACK (moko_icon_view_item_acitvated_cb), mma);

    signal (SIGUSR1, handle_sigusr1);

    moko_finger_window_set_contents (mma->window, GTK_WIDGET(mma->mm));
    gtk_widget_show_all (GTK_WIDGET(mma->window) );

    gtk_widget_show (GTK_WIDGET (mma->wheel));	
    gtk_widget_show (GTK_WIDGET (mma->toolbox));
    gtk_widget_show (GTK_WIDGET (mma->mm));

    gtk_main();

    if (mma)
	{
		if (mma->history)
			moko_app_history_free (mma->history);
    	g_free (mma);
	}
    return 0;
}
