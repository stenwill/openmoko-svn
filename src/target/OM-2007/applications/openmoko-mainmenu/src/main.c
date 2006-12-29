/*
 *  openmoko-mainmenu
 *
 *  Authored by Sun Zhiyong <sunzhiyong@fic-sh.com.cn>
 *
 *  Copyright (C) 2006 First International Computer Inc.
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

#include "callbacks.h"

#include "main.h"

int 
main( int argc, char** argv ) {
    MokoMainmenuApp *mma;
    int i;

    mma = g_malloc0 (sizeof (MokoMainmenuApp));
    if (!mma) {
    	fprintf (stderr, "openmoko-mainmenu application initialize FAILED");
    	exit (0);
    	}
    memset (mma, 0, sizeof (MokoMainmenuApp));

    gtk_init( &argc, &argv );
    
    /* application object */
    mma->app = MOKO_APPLICATION(moko_application_get_instance());
    g_set_application_name( "OpenMoko Main Menu" );
    
    /* finger based window */
    mma->window = MOKO_FINGER_WINDOW(moko_finger_window_new());
    gtk_widget_show (GTK_WIDGET (mma->window));

    /* finger wheel object*/
    mma->wheel = moko_finger_window_get_wheel (mma->window);

    /* finger toolbox object*/
    mma->toolbox = moko_finger_window_get_toolbox(mma->window);
    //initialize toolbox's buttons, which are MokoPixmapButton objects.
    for (i=0; i<4; i++)
    	{
    	    mma->history[i] =  moko_finger_tool_box_add_button_without_label (mma->toolbox);
           gtk_widget_show (mma->history[i]);
    	}

    /* MokoMainMenu object */
    mma->mm = moko_main_menu_new();

    /* MokoClosePage object */
    mma->close = moko_close_page_new ();

    /* signal connected*/
    //finger wheel object signals
    g_signal_connect (mma->wheel, "press_bottom",
    		G_CALLBACK ( moko_wheel_bottom_press_cb), mma);
    g_signal_connect (mma->wheel, "press_left_up",
    		G_CALLBACK ( moko_wheel_left_up_press_cb), mma);
    g_signal_connect (mma->wheel, "press_right_down",
    		G_CALLBACK ( moko_wheel_right_down_press_cb), mma);
    //MokoClosePage object signals
    g_signal_connect (mma->close->close_btn, "released",
    		G_CALLBACK (moko_close_page_close_btn_released_cb), mma);
    //MokoMainMenu:MokoIconView object signals
    g_signal_connect (mma->mm->icon_view, "selection-changed",
    		G_CALLBACK (moko_icon_view_selection_changed_cb), mma);
    g_signal_connect (mma->mm->icon_view, "item_activated",
    		G_CALLBACK (moko_icon_view_item_acitvated_cb), mma);

    /* put MokoMainMenu object and MokoClosePange object into the finger based window */
    moko_finger_window_set_contents (mma->window, GTK_WIDGET(mma->mm));
    moko_finger_window_set_contents (mma->window, GTK_WIDGET(mma->close));
    
    /* show everything and run main loop */
    gtk_widget_show_all (GTK_WIDGET(mma->window) );

    /*show wheel toolbox MokoMainMenu objects first, and hide the MokoClosePage object*/
    gtk_widget_show (GTK_WIDGET (mma->wheel));	
    gtk_widget_show (GTK_WIDGET (mma->toolbox));
    gtk_widget_show (GTK_WIDGET (mma->mm));
    gtk_widget_hide (GTK_WIDGET (mma->close));

    /*test code, delete later*/
    moko_sample_hisory_app_fill (mma->history[0]);

    gtk_main();

    if (mma)
    	  g_free (mma);
    return 0;
}
