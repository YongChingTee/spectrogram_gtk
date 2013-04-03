/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) 2013 Ching <ching@ubuntu>
 * 
 * gtk-firstexample is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gtk-firstexample is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>


#include <glib/gi18n.h>



static GtkWidget*
create_window (void)
{
	GtkWidget *window;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "gtk-firstexample");
	
	/* Exit when the window is closed */
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	return window;
}


int
main (int argc, char *argv[])
{
 	GtkWidget *window;

	GdkPixbuf *pixbuf;
	
	pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, 0, 8, 10, 10);
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	
	gtk_init (&argc, &argv);

	window = create_window ();
	gdk_draw_lines(window, 
	gtk_widget_show (window);

	gtk_main ();

	return 0;
}
