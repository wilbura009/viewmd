/* viewmd-window.c
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

//#include "viewmd-config.h"
#include "viewmd-window.h"
#include "webkit2/webkit2.h"

struct _ViewmdWindow
{
  GtkApplicationWindow  parent_instance;
};

G_DEFINE_TYPE (ViewmdWindow, viewmd_window, GTK_TYPE_APPLICATION_WINDOW)

static void
viewmd_window_class_init (ViewmdWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/viewmd/viewmd-window.ui");
}

gchar *convert_md_to_html (gchar *path_md)
{
  //pandoc -s --highlight=zenburn -c src/css/webkit.css src/input/ipsum.md -t html -o gtk.html;
  gchar *html_content;
  gchar *highlighting = g_strdup_printf ("--highlight-style=%s", "zenburn");
  gchar *command = g_strdup_printf ("pandoc -s %s %s -t html",highlighting, path_md);
  g_spawn_command_line_sync (command, &html_content, NULL, NULL, NULL);
  g_free (command);
  return html_content;
}

// Connect to the 'changed' signal of the file monitor
static void
file_changed (GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data)
{
  if (event_type == G_FILE_MONITOR_EVENT_CHANGED)
  {
    gchar *path = g_file_get_path(file);
    gchar *html_content;
    html_content = convert_md_to_html (g_file_get_path (file));

    // Load the html content into the webview but first remove the old webview
    gtk_container_remove(GTK_CONTAINER(user_data), gtk_bin_get_child(GTK_BIN(user_data)));

    WebKitUserContentManager *manager = webkit_user_content_manager_new ();
    WebKitUserStyleSheet *style_sheet;

    gchar *path_css = g_strdup_printf ("%s/.config/viewmd/css/webkit.css", g_get_home_dir ());
    GFile *file_css = g_file_new_for_path (path_css);
    gchar *css_content;
    g_file_load_contents (file_css, NULL, &css_content, NULL, NULL, NULL);

    style_sheet = webkit_user_style_sheet_new (css_content, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES, WEBKIT_USER_STYLE_LEVEL_USER, NULL, NULL);

    webkit_user_content_manager_add_style_sheet (manager, style_sheet);

    // Load the html content into the webview
    WebKitWebView *webView = WEBKIT_WEB_VIEW (webkit_web_view_new_with_user_content_manager (manager));
    webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webView)), TRUE);
    webkit_web_view_load_html (webView, html_content, NULL);
    gtk_container_add(GTK_CONTAINER(user_data), GTK_WIDGET(webView));
    gtk_widget_show_all (GTK_WIDGET (user_data));
    g_free(path);
    g_free(html_content);
    //g_print("G_FILE_MONITOR_EVENT_CHANGED.event_type: %d\n", event_type);
  }
}

void
viewmd_window_open(ViewmdWindow *win, GFile *file)
{
  gchar *path = g_file_get_path (file);

  if (g_file_query_exists (file, NULL))
  {
    gchar *html_content;
    html_content = convert_md_to_html (path);

    WebKitUserContentManager *manager = webkit_user_content_manager_new ();
    WebKitUserStyleSheet *style_sheet;

    // Get the path to the .css file located in the user's home directory
    gchar *path_css = g_strdup_printf ("%s/.config/viewmd/css/webkit.css", g_get_home_dir ());
    GFile *file_css = g_file_new_for_path (path_css);
    gchar *css_content;
    g_file_load_contents (file_css, NULL, &css_content, NULL, NULL, NULL);

    style_sheet = webkit_user_style_sheet_new (css_content, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES, WEBKIT_USER_STYLE_LEVEL_USER, NULL, NULL);

    webkit_user_content_manager_add_style_sheet (manager, style_sheet);

    // Load the html content into the webview
    WebKitWebView *webView = WEBKIT_WEB_VIEW (webkit_web_view_new_with_user_content_manager (manager));
    webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webView)), TRUE);
    webkit_web_view_load_html (webView, html_content, NULL);
    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(webView));

    gtk_widget_show_all (GTK_WIDGET (win));

    // Watch the file for changes
    GFileMonitor *monitor;
    monitor = g_file_monitor_file (file, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect (monitor, "changed", G_CALLBACK (file_changed), win);
    printf ("Watching file: %s\n", path);
  }
  else
  {
    g_print ("[ERROR]: File '%s' does not exist\n", path);
    g_free (path);
    g_object_unref (file);
    exit (1);
  }
}

static void
viewmd_window_init (ViewmdWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
