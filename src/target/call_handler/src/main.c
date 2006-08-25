/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <unistd.h>

extern gchar *operation;

int
main (int argc, char *argv[])
{
  GtkWidget *window1;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  /*initialize the rc file to color the widget */
  gtk_rc_parse("./openmoko.rc");

  /*display usage*/
  if(argc==1)
  {
	g_print("Usage:\n");
	g_print("\topenmoko-callhandlering ans/dial\n");
	return 0;
  }

  /* set the operation type inorder to display 
   * the corresponding interface*/
  operation = argv[1];

  /* get the infomation to display
   * from dialer module	*/
  if(argc == 3)
  {
	g_print(argv[2]);
	infoConstruction(argv[2]);
  }

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  window1 = create_window1 ();
  gtk_window_set_resizable(GTK_WINDOW(window1),FALSE);
  gtk_widget_show (window1);
  gtk_widget_set_name (window1, "main");
  gtk_main ();
  return 0;
}

