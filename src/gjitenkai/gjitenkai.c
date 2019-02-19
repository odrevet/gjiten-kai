#include "gjitenkai.h"

extern G_MODULE_EXPORT void on_gjitenkai_menuitem_history_click(GtkWidget *menuitem_history,
                                                                gjitenkai *gjitenkai);

void gjitenkai_init (gjitenkai *gjitenkai){
  const gchar * const * dirs = g_get_system_data_dirs();
  gchar* rest = g_strjoin(G_DIR_SEPARATOR_S,
			  PROJECT_NAME,
			  "ui",
			  UI_DEFINITIONS_FILE_GJITENKAI,
			  NULL);
  gchar* filename = get_file(dirs, (const gchar*) rest);
  g_free(rest);

  GError *err = NULL;
  gjitenkai->definitions = gtk_builder_new ();
  gtk_builder_add_from_file (gjitenkai->definitions, filename, &err);

  if(!filename){
    g_printerr("Impossible to find UI definition file at %s", rest);
  }

  g_free(filename);

  if (err != NULL) {
    g_printerr
      ("Error while loading gjitenkai definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }
  gtk_builder_connect_signals (gjitenkai->definitions, gjitenkai);
}

void gjitenkai_menu_history_append(gjitenkai *p_gjitenkai, const gchar *text){
  GtkWidget *submenu_history = (GtkWidget *)gtk_builder_get_object(p_gjitenkai->definitions,
                                                                   "menu_history");
  GtkWidget *menuitem_search_expression = gtk_menu_item_new_with_label(text);
  g_signal_connect(menuitem_search_expression,
                   "activate",
                   G_CALLBACK(on_gjitenkai_menuitem_history_click),
                   p_gjitenkai);

  gtk_menu_shell_insert(GTK_MENU_SHELL(submenu_history),
                        GTK_WIDGET(menuitem_search_expression),
                        1);
  gtk_widget_show(menuitem_search_expression);
}
