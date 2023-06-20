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
#include "viewmd-window.h"
#include "webkit2/webkit2.h"
#include <stdio.h>
#include <stdlib.h>

struct _ViewmdWindow
{
  GtkApplicationWindow  parent_instance;
};

G_DEFINE_TYPE (ViewmdWindow, viewmd_window, GTK_TYPE_APPLICATION_WINDOW)

static void
viewmd_window_class_init (ViewmdWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/viewmd/viewmd-window.ui" );
}

static gchar*
replace_html_img_tag_with_file_url(gchar *path_md, gchar *html_content)
{
  gchar *path_md_dir = g_path_get_dirname (path_md);

  GRegex *regex_img_tag_all;
  GMatchInfo *match_info;

  // Match not http - this also covers the <p> wraps 
  gchar *img_tag_pattern = "<img src=\"(?!http)(.*)\" alt=\"(.*)\".*";
  gchar *img_tag_replacement = g_strdup_printf(
      "<img src=\"file:///%s/\\1\" alt=\"\\2\" />", path_md_dir);

  regex_img_tag_all = g_regex_new (img_tag_pattern, 0, 0, NULL);

  html_content = g_regex_replace (regex_img_tag_all, html_content, -1, 0,
                                  img_tag_replacement, 0, NULL);

  g_regex_unref (regex_img_tag_all);
  g_free (img_tag_replacement);

  return html_content;
}

static gchar*
convert_md_to_html (gchar *path_md)
{
  gchar *html_content;
  gchar *highlighting = g_strdup_printf ("--highlight-style=%s", "zenburn");
  gchar *command = g_strdup_printf ("pandoc -s %s %s -t html", highlighting, path_md);
  g_spawn_command_line_sync (command, &html_content, NULL, NULL, NULL);

  g_free (highlighting);
  g_free (command);

  return html_content;
}

// Connect to the 'changed' signal of the file monitor
static void
file_changed (GFileMonitor *monitor,
              GFile *file,
              GFile *other_file,
              GFileMonitorEvent event_type,
              gpointer user_data)
{
  if (event_type == G_FILE_MONITOR_EVENT_CHANGED)
  {
    gchar *css_uri = "resource:///org/gnome/viewmd/viewmd-window-webkit.css";
    gchar *path = g_file_get_path(file);
    gchar *html_content;
    html_content = convert_md_to_html (g_file_get_path (file));

    WebKitSettings *settings = webkit_settings_new_with_settings(
        "enable-developer-extras", TRUE, NULL,
        "auto-load-images", TRUE, NULL);

    gtk_container_remove(GTK_CONTAINER(user_data),
                         gtk_bin_get_child(GTK_BIN(user_data)));

    GFile *css_gfile = g_file_new_for_uri (css_uri);
    gchar *css_content;
    g_file_load_contents (css_gfile, NULL, &css_content, NULL, NULL, NULL);

    WebKitUserContentManager *manager = webkit_user_content_manager_new ();
    WebKitUserStyleSheet *style_sheet;

    style_sheet = webkit_user_style_sheet_new (css_content,
                                               WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
                                               WEBKIT_USER_STYLE_LEVEL_USER,
                                               NULL,
                                               NULL);

    webkit_user_content_manager_add_style_sheet (manager, style_sheet);

    WebKitWebView *webView;
    webView = WEBKIT_WEB_VIEW (webkit_web_view_new_with_user_content_manager (manager));

    webkit_web_view_set_settings (webView, settings);
    webkit_web_view_load_html (webView, html_content, NULL);
    gtk_container_add(GTK_CONTAINER(user_data), GTK_WIDGET(webView));
    gtk_widget_show_all (GTK_WIDGET (user_data));

    g_free (html_content);
    g_free (css_content);
    g_object_unref (css_gfile);
    g_object_unref (settings);
    g_object_unref (manager);
  }
}

void
print_properties(GObject *object)
{
  GParamSpec **properties;
  guint n_properties, i;

  properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_properties);
  for (i = 0; i < n_properties; i++)
  {
    GParamSpec *property = properties[i];
    const gchar *name = g_param_spec_get_name(property);
    GValue value = G_VALUE_INIT;
    g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(property));
    g_object_get_property(object, name, &value);
    gchar *str_value = g_strdup_value_contents(&value);
    g_print("%s: %s\n", name, str_value);
    g_free(str_value);
    g_value_unset(&value);
  }
  g_free(properties);
}

void
viewmd_window_open(ViewmdWindow *win, GFile *file)
{
  gchar *path = g_file_get_path (file);

  if (g_file_query_exists (file, NULL))
  {
    gchar *html_content;
    html_content = convert_md_to_html (path);
    html_content = replace_html_img_tag_with_file_url(path, html_content);
    g_print ("%s\n", html_content);

    WebKitSettings *settings = webkit_settings_new();
    webkit_settings_set_enable_developer_extras(settings, TRUE);
    webkit_settings_set_auto_load_images(settings, TRUE);
    webkit_settings_set_allow_file_access_from_file_urls(settings, TRUE);
    webkit_settings_set_allow_universal_access_from_file_urls(settings,TRUE);
    webkit_settings_set_allow_top_navigation_to_data_urls(settings, TRUE);

    //print_properties(G_OBJECT(settings));

    gchar *css_uri = "resource:///org/gnome/viewmd/viewmd-window-webkit.css";
    GFile *css_gfile = g_file_new_for_uri (css_uri);
    gchar *css_content;
    g_file_load_contents (css_gfile, NULL, &css_content, NULL, NULL, NULL);

    WebKitUserContentManager *manager = webkit_user_content_manager_new ();
    WebKitUserStyleSheet *style_sheet;

    style_sheet = webkit_user_style_sheet_new (css_content,
                                               WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
                                               WEBKIT_USER_STYLE_LEVEL_USER,
                                               NULL,
                                               NULL);

    webkit_user_content_manager_add_style_sheet (manager, style_sheet);

    // Load the html content into the webview
    WebKitWebView *webView;
    webView = WEBKIT_WEB_VIEW (webkit_web_view_new_with_user_content_manager (manager));

    // Set the webview settings
    webkit_web_view_set_settings (webView, settings);
    webkit_web_view_load_html (webView, html_content, "file:///");
    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(webView));
    gtk_widget_show_all (GTK_WIDGET (win));

    // Watch the file for changes
    GFileMonitor *monitor;
    monitor = g_file_monitor_file (file, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect (monitor, "changed", G_CALLBACK (file_changed), win);
    printf ("Watching file: %s\n", path);

    g_free (html_content);
    g_free (css_content);
    g_object_unref (css_gfile);
    g_object_unref (settings);
    g_object_unref (manager);
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
