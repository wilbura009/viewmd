/* main.c
 *
 * Copyright 2023 wb
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib/gi18n.h>

#include "viewmd-application.h"
#include "viewmd-window.h"

int
main (int   argc,
      char *argv[])
{
	g_autoptr(ViewmdApplication) app = NULL;
	int ret;

  /*
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
  */

	app = viewmd_application_new ("org.gnome.viewmd", G_APPLICATION_HANDLES_OPEN);
	ret = g_application_run (G_APPLICATION (app), argc, argv);

	return ret;
}
