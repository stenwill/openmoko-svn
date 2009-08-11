/*
 * gui.c - Editor GUI core
 *
 * Written 2009 by Werner Almesberger
 * Copyright 2009 by Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

#include "util.h"
#include "inst.h"
#include "obj.h"
#include "dump.h"
#include "kicad.h"
#include "postscript.h"
#include "gui_util.h"
#include "gui_style.h"
#include "gui_status.h"
#include "gui_canvas.h"
#include "gui_tool.h"
#include "gui_frame.h"
#include "gui.h"

#include "icons/stuff.xpm"
#include "icons/stuff_off.xpm"
#include "icons/meas.xpm"
#include "icons/meas_off.xpm"


extern char *save_file;

GtkWidget *root;
int show_stuff = 1;
int show_meas = 1;


static GtkWidget *frames_box;
static GtkWidget *ev_stuff, *ev_meas;
static GtkWidget *stuff_image[2], *meas_image[2];


/* ----- save/write operations --------------------------------------------- */


static char *set_extension(const char *name, const char *ext)
{
	char *s = stralloc(name);
	char *slash, *dot;
	char *res;

	slash = strrchr(s, '/');
	dot = strrchr(slash ? slash : s, '.');
	if (dot)
		*dot = 0;
	res = stralloc_printf("%s.%s", s, ext);
	free(s);
	return res;
}


static int save_to(const char *name, int (*fn)(FILE *file))
{
	FILE *file;

	file = fopen(name, "w");
	if (!file) {
		perror(name);
		return 0;
	}
	if (!fn(file)) {
		perror(name);
		return 0;
	}
	if (fclose(file) == EOF) {
		perror(name);
		return 0;
	}
	return 1;
}


static void save_with_backup(const char *name, int (*fn)(FILE *file))
{
	char *s = stralloc(name);
	char *back, *tmp;
	char *slash, *dot;
	int n;
	struct stat st;

	/* save to temporary file */

	slash = strrchr(s, '/');
	if (!slash)
		tmp = stralloc_printf("~%s", s);
	else {
		*slash = 0;
		tmp = stralloc_printf("%s/~%s", s, slash+1);
		*slash = '/';
	}

	if (!save_to(tmp, fn))
		return;

	/* move existing file out of harm's way */

	dot = strrchr(slash ? slash : s, '.');
	if (dot)
		*dot = 0;
	n = 0;
	while (1) {
		back = stralloc_printf("%s~%d%s%s",
		    s, n, dot ? "." : "", dot ? dot+1 : "");
		if (stat(back, &st) < 0) {
			if (errno == ENOENT)
				break;
			perror(back);
			free(back);
			return;
		}
		free(back);
		n++;
	}
	if (rename(name, back) < 0) {
		if (errno != ENOENT) {
			perror(name);
			free(back);
			return;
		}
	} else {
		fprintf(stderr, "renamed %s to %s\n", name, back);
	}
	free(back);

	/* rename to final name */

	if (rename(tmp, name) < 0) {
		perror(name);
		free(tmp);
		return;
	}
	free(tmp);

	fprintf(stderr, "saved to %s\n", name);
}


static void menu_save(void)
{
	if (save_file)
		save_with_backup(save_file, dump);
	else {
		if (!dump(stdout))
			perror("stdout");
	}
}


static void menu_write_kicad(void)
{
	char *name;

	if (save_file) {
		name = set_extension(save_file, "mod");
		save_to(name, kicad);
		free(name);
	} else {
		if (!kicad(stdout))
			perror("stdout");
	}
}


static void menu_write_ps(void)
{
	char *name;

	if (save_file) {
		name = set_extension(save_file, "ps");
		save_to(name, postscript);
		free(name);
	} else {
		if (!postscript(stdout))
			perror("stdout");
	}
}


/* ----- menu bar ---------------------------------------------------------- */


static GtkItemFactoryEntry menu_entries[] = {
	{ "/File",		NULL,	NULL,	 	0, "<Branch>" },
	{ "/File/Save",		NULL,	menu_save,	0, "<Item>" },
        { "/File/sep0",		NULL,	NULL,		0, "<Separator>" },
        { "/File/Write KiCad",	NULL,	menu_write_kicad, 0, "<Item>" },
        { "/File/Write Postscript",
				NULL,	menu_write_ps,	0, "<Item>" },
        { "/File/sep2",		NULL,	NULL,		0, "<Separator>" },
        { "/File/Quit",		NULL,	gtk_main_quit,	0, "<Item>" },
};


static void make_menu_bar(GtkWidget *hbox)
{
	GtkItemFactory *factory;
	GtkWidget *bar;

	factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<FpedMenu>", NULL);
        gtk_item_factory_create_items(factory,
	    sizeof(menu_entries)/sizeof(*menu_entries), menu_entries, NULL);

	bar = gtk_item_factory_get_widget(factory, "<FpedMenu>");
	gtk_box_pack_start(GTK_BOX(hbox), bar, TRUE, TRUE, 0);
}


static gboolean toggle_stuff(GtkWidget *widget, GdkEventButton *event,
    gpointer data)
{
	switch (event->button) {
	case 1:
		show_stuff = !show_stuff;
		set_image(ev_stuff, stuff_image[show_stuff]);
		inst_deselect();
		redraw();
		break;
	}
        return TRUE;
}


static gboolean toggle_meas(GtkWidget *widget, GdkEventButton *event,
    gpointer data)
{
	switch (event->button) {
	case 1:
		show_meas = !show_meas;
		set_image(ev_meas, meas_image[show_meas]);
		inst_deselect();
		redraw();
		break;
	}
        return TRUE;
}


static void make_tool_bar(GtkWidget *hbox, GdkDrawable *drawable)
{
	GtkWidget *bar;

	bar = gtk_toolbar_new();
	gtk_box_pack_end(GTK_BOX(hbox), bar, TRUE, TRUE, 0);
	//gtk_box_pack_end(GTK_BOX(hbox), bar, FALSE, FALSE, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

	ev_stuff = tool_button(bar, drawable, NULL, toggle_stuff, NULL);
	ev_meas = tool_button(bar, drawable, NULL, toggle_meas, NULL);

	stuff_image[0] = gtk_widget_ref(make_image(drawable, xpm_stuff_off));
	stuff_image[1] = gtk_widget_ref(make_image(drawable, xpm_stuff));
	meas_image[0] = gtk_widget_ref(make_image(drawable, xpm_meas_off));
	meas_image[1] = gtk_widget_ref(make_image(drawable, xpm_meas));

	set_image(ev_stuff, stuff_image[show_stuff]);
	set_image(ev_meas, meas_image[show_meas]);
}


static void make_top_bar(GtkWidget *vbox)
{
	GtkWidget *hbox;

	hbox = gtk_hbox_new(FALSE, 0);
	make_menu_bar(hbox);
	make_tool_bar(hbox, root->window);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
}


/* ----- central screen area ----------------------------------------------- */


static void make_center_area(GtkWidget *vbox)
{
	GtkWidget *hbox, *frames_area, *paned;
	GtkWidget *tools;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	paned = gtk_hpaned_new();
	gtk_box_pack_start(GTK_BOX(hbox), paned, TRUE, TRUE, 0);
	
	/* Frames */

	frames_area = gtk_scrolled_window_new(NULL, NULL);
	gtk_paned_add1(GTK_PANED(paned), frames_area);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frames_area),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(frames_area, 250, 100);

	frames_box = gtk_vbox_new(FALSE, 0);
	build_frames(frames_box);

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(frames_area),
	    frames_box);

	/* Canvas */

	gtk_paned_add2(GTK_PANED(paned), make_canvas());

	/* Icon bar */

	tools = gui_setup_tools(root->window);
	gtk_box_pack_end(GTK_BOX(hbox), tools, FALSE, FALSE, 0);
}


/* ----- GUI construction -------------------------------------------------- */


void change_world(void)
{
	inst_deselect();
	status_begin_reporting();
	instantiate();
	label_in_box_bg(active_frame->label, COLOR_FRAME_SELECTED);
	build_frames(frames_box);
	redraw();
}


static void make_screen(GtkWidget *window)
{
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	make_top_bar(vbox);
	make_center_area(vbox);
	make_status_area(vbox);
}


int gui_init(int *argc, char ***argv)
{
	gtk_init(argc, argv);
	return 0;
}


int gui_main(void)
{
	root = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(root), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(root), 620, 460);
	gtk_window_set_title(GTK_WINDOW(root), "fped");

	/* get root->window */
	gtk_widget_show_all(root);

	g_signal_connect_swapped(G_OBJECT(root), "destroy",
	    G_CALLBACK(gtk_main_quit), NULL);

	make_screen(root);

	gtk_widget_show_all(root);

	gui_setup_style(root->window);
	init_canvas();
	edit_nothing();
	select_frame(root_frame);
	make_popups();

	gtk_main();

	return 0;
}
