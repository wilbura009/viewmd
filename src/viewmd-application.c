/* viewmd-application.c
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

//#include "config.h"

#include "viewmd-application.h"
#include "viewmd-window.h"

struct _ViewmdApplication
{
  GtkApplication parent_instance;
};

G_DEFINE_TYPE (ViewmdApplication, viewmd_application, GTK_TYPE_APPLICATION)

ViewmdApplication *
viewmd_application_new (const char        *application_id,
                             GApplicationFlags  flags)
{
  g_return_val_if_fail (application_id != NULL, NULL);

  return g_object_new (VIEWMD_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       NULL);
}

static void
viewmd_application_activate (GApplication *app)
{
  GtkWindow *window;

  g_assert (VIEWMD_IS_APPLICATION (app));

  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window == NULL)
    window = g_object_new (VIEWMD_TYPE_WINDOW,
                           "application", app,
                           NULL);
  gtk_window_set_title (window, "Viewmd");
  gtk_window_present (window);
}

static void
viewmd_application_open (GApplication  *app,
                  GFile        **files,
                  int            n_files,
                  const char    *hint)
{
  GtkWindow *window;
  int i;

  g_assert (VIEWMD_IS_APPLICATION (app));

  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window == NULL)
    window = g_object_new (VIEWMD_TYPE_WINDOW,
                           "application", app,
                           NULL);
  gtk_window_set_title (window, "Viewmd");
  gtk_window_present (window);

  for (i = 0; i < n_files; i++)
    viewmd_window_open (VIEWMD_WINDOW (window), files[i]);
}

static void
viewmd_application_class_init (ViewmdApplicationClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);
  app_class->activate = viewmd_application_activate;
  app_class->open = viewmd_application_open;
}

/*
static void
viewmd_application_about_action (GSimpleAction *action,
                                      GVariant      *parameter,
                                      gpointer       user_data)
{
  static const char *authors[] = {"wb", NULL};
  ViewmdApplication *self = user_data;
  GtkWindow *window = NULL;

  g_assert (VIEWMD_IS_APPLICATION (self));

  window = gtk_application_get_active_window (GTK_APPLICATION (self));

  gtk_show_about_dialog (window,
                         "program-name", "viewmd",
                         "logo-icon-name", "org.gnome.viewmd",
                         "authors", authors,
                         "version", "0.1.0",
                         "copyright", "© 2023 wb",
                         NULL);
}

static void
viewmd_application_quit_action (GSimpleAction *action,
                                     GVariant      *parameter,
                                     gpointer       user_data)
{
  ViewmdApplication *self = user_data;

  g_assert (VIEWMD_IS_APPLICATION (self));

  g_application_quit (G_APPLICATION (self));
}

static const GActionEntry app_actions[] = {
  { "quit", viewmd_application_quit_action },
  { "about", viewmd_application_about_action },
};
*/

static void
viewmd_application_init (ViewmdApplication *self)
{
  /*
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   app_actions,
                                   G_N_ELEMENTS (app_actions),
                                   self);
  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.quit",
                                         (const char *[]) { "<primary>q", NULL });
                                         */
}
