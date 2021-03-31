#include "gjitenkai.h"
#include "../worddic/worddic.h"
#include "../kanjidic/kanjidic.h"

//create a regex expression with a kanji list in an 'alternative operator' []
static gboolean on_eval_expend_bushuu(const GMatchInfo *info, GString *res, gpointer data)
{
  kanjidic *kanjidic = data;
  gchar *match;

  match = g_match_info_fetch(info, 0);

  GSList *kanji_by_radical_list = NULL;
  kanji_by_radical_list = get_kanji_by_radical(match, kanjidic->rad_info_hash);
  GSList *l = NULL;

  GString *kanji_list = g_string_new("[");
  for (l = kanji_by_radical_list; l != NULL; l = l->next)
  {
    kanji_list = g_string_append(kanji_list, (gchar *)l->data);
  }
  kanji_list = g_string_append_c(kanji_list, ']');

  res = g_string_append(res, kanji_list->str);

  g_string_free(kanji_list, TRUE);
  g_free(match);

  return FALSE;
}

/**
   Search into kanjidic the radicals before searching in worddic
*/

G_MODULE_EXPORT void on_gjitenkai_search_expression_activate(GtkEntry *entry, gjitenkai *gjitenkai)
{
  const gchar *search_entry_text = gtk_entry_get_text(entry);
  if (!strcmp(search_entry_text, ""))
    return;

  //search for radicals in pairs of bracets (or fullwidth bracets) and replace
  //it with a list of kanji in a square bracets [] alternative operator
  GRegex *regex_bushuu_delim = g_regex_new("[<＜].+[＞>]", G_REGEX_UNGREEDY, 0, NULL);
  const gchar *search_text = g_regex_replace_eval(regex_bushuu_delim,
                                                  search_entry_text,
                                                  -1, 0, 0,
                                                  on_eval_expend_bushuu,
                                                  gjitenkai->kanjidic,
                                                  NULL);
  //free memory
  g_regex_unref(regex_bushuu_delim);

  //search in worrdic
  gboolean has_match = worddic_search(search_text, gjitenkai->worddic);

  //if there are results and the user allow record history
  if (has_match && gjitenkai->worddic->conf->record_history)
  {
    //append to the history menu
    gjitenkai->worddic->conf->history = g_slist_append(gjitenkai->worddic->conf->history,
                                                       g_strdup(search_entry_text));

    gjitenkai_menu_history_append(gjitenkai, search_entry_text);
  }
}

//History
////Clear
G_MODULE_EXPORT void on_gjitenkai_history_clear_activate(GtkMenuItem *menuitem, gjitenkai *gjitenkai)
{
  //free the history list
  g_slist_free_full(gjitenkai->worddic->conf->history, (GDestroyNotify)g_free);
  gjitenkai->worddic->conf->history = NULL;

  //remove history menuitem
  GtkWidget *submenu_history = (GtkWidget *)gtk_builder_get_object(gjitenkai->definitions, "menu_history");
  //remove the menu items from the history menu
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(submenu_history));
  for (iter = children; iter != NULL; iter = g_list_next(iter))
  {
    GtkWidget *w = GTK_WIDGET(iter->data);
    //remove the menuitem it's not the clear item
    if (GTK_MENU_ITEM(w) != menuitem)
      gtk_widget_destroy(w);
  }
  g_list_free(children);

  //reset the settings
  g_settings_reset(gjitenkai->worddic->settings, "history");
}

////History element
G_MODULE_EXPORT void on_gjitenkai_menuitem_history_click(GtkWidget *menuitem_history, gjitenkai *gjitenkai)
{
  //get the search entry
  GtkWidget *entry = (GtkWidget *)gtk_builder_get_object(gjitenkai->worddic->definitions, "search_expression");
  GtkWidget *child = gtk_bin_get_child(GTK_BIN(menuitem_history));
  const gchar *text = gtk_label_get_text(GTK_LABEL(child));
  gtk_entry_set_text(GTK_ENTRY(entry), text);
}

G_MODULE_EXPORT void on_gjitenkai_menuitem_prefs_activate(GtkButton *button,
                                                          gjitenkai *gjitenkai)
{
  //set size and display the preference window
  GtkDialog *prefs = (GtkDialog *)gtk_builder_get_object(gjitenkai->definitions, "prefs");
  gtk_window_set_default_size(GTK_WINDOW(prefs), 420, 280);
  gtk_widget_show_all((GtkWidget *)prefs);
}

G_MODULE_EXPORT void on_gjitenkai_button_prefs_OK_clicked(GtkButton *button,
                                                          gjitenkai *gjitenkai)
{
  GtkDialog *prefs = (GtkDialog *)gtk_builder_get_object(gjitenkai->definitions, "prefs");
  gtk_widget_hide(GTK_WIDGET(prefs));
}

G_MODULE_EXPORT void on_menuitem_view_worddic_toggled(GtkCheckMenuItem *menu_item,
                                                      gjitenkai *gjitenkai)
{
  GtkWidget *box_worddic = (GtkWidget *)gtk_builder_get_object(gjitenkai->worddic->definitions, "box_toplevel");

  if (gtk_check_menu_item_get_active(menu_item))
  {
    gtk_widget_show(box_worddic);
  }
  else
  {
    gtk_widget_hide(box_worddic);
  }
}

G_MODULE_EXPORT void on_menuitem_view_kanjidic_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai)
{
  GtkWidget *box_kanjidic = (GtkWidget *)gtk_builder_get_object(gjitenkai->kanjidic->definitions, "box_toplevel");
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item)))
  {
    gtk_widget_show(box_kanjidic);
  }
  else
  {
    gtk_widget_hide(box_kanjidic);
  }
}

G_MODULE_EXPORT void on_menuitem_paned_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai)
{
  GtkWidget *box_worddic = (GtkWidget *)gtk_builder_get_object(gjitenkai->worddic->definitions,
                                                               "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget *)gtk_builder_get_object(gjitenkai->kanjidic->definitions,
                                                                "box_toplevel");
  GtkWindow *box_top = (GtkWindow *)gtk_builder_get_object(gjitenkai->definitions,
                                                           "box_top");
  //remove the worddic and kanjidic boxes from the notebook pages'box
  GtkWidget *page_worddic = gtk_notebook_get_nth_page(gjitenkai->notebook, 0);
  GtkWidget *page_kanjidic = gtk_notebook_get_nth_page(gjitenkai->notebook, 1);

  g_object_ref(box_worddic);
  g_object_ref(box_kanjidic);
  gtk_container_remove(GTK_CONTAINER(page_worddic), GTK_WIDGET(box_worddic));
  gtk_container_remove(GTK_CONTAINER(page_kanjidic), GTK_WIDGET(box_kanjidic));

  //remove the notebook from the window top box
  g_object_ref(gjitenkai->notebook);
  gtk_container_remove(GTK_CONTAINER(box_top), GTK_WIDGET(gjitenkai->notebook));

  //add worddic and kanjidic to the pane
  gtk_paned_add1(gjitenkai->paned, box_worddic);
  gtk_paned_add2(gjitenkai->paned, box_kanjidic);

  //add the pane to the box top
  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai->paned), TRUE, TRUE, 0);

  //show
  gtk_widget_show_all(GTK_WIDGET(box_top));
}

G_MODULE_EXPORT void on_menuitem_notebook_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai)
{

  GtkWidget *box_worddic = (GtkWidget *)gtk_builder_get_object(gjitenkai->worddic->definitions, "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget *)gtk_builder_get_object(gjitenkai->kanjidic->definitions, "box_toplevel");
  GtkWindow *box_top = (GtkWindow *)gtk_builder_get_object(gjitenkai->definitions, "box_top");

  //remove the worddic and kanjidic from the pane
  g_object_ref(box_worddic);
  g_object_ref(box_kanjidic);
  gtk_container_remove(GTK_CONTAINER(gjitenkai->paned), box_worddic);
  gtk_container_remove(GTK_CONTAINER(gjitenkai->paned), box_kanjidic);

  //add the worddic and kanjidic box to the notebook pages' box
  GtkWidget *page_worddic = gtk_notebook_get_nth_page(gjitenkai->notebook, 0);
  GtkWidget *page_kanjidic = gtk_notebook_get_nth_page(gjitenkai->notebook, 1);

  gtk_box_pack_start(GTK_BOX(page_worddic), box_worddic, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(page_kanjidic), box_kanjidic, TRUE, TRUE, 0);

  //remove the pane from the window top box
  g_object_ref(gjitenkai->paned);
  gtk_container_remove(GTK_CONTAINER(box_top), GTK_WIDGET(gjitenkai->paned));

  //add the notebook to the top box
  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai->notebook), TRUE, TRUE, 0);

  gtk_widget_show_all(GTK_WIDGET(box_top));
}

//about
G_MODULE_EXPORT void on_gjiten_menuitem_help_about_activate(GtkMenuItem *menuitem, gjitenkai *gjitenkai)
{
  GtkWindow *window_about = (GtkWindow *)gtk_builder_get_object(gjitenkai->definitions, "aboutdialog");
  gtk_dialog_run(GTK_DIALOG(window_about));
  gtk_widget_hide(GTK_WIDGET(window_about));
}

G_MODULE_EXPORT void on_gjiten_menuitem_help_download_dic_activate(GtkMenuItem *menuitem,
                                                                   gjitenkai *gjitenkai)
{

  GtkDialog *dialog = (GtkDialog *)gtk_builder_get_object(gjitenkai->worddic->definitions, "dialog_dic_download");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_hide(GTK_WIDGET(dialog));
}

//hide and prevent deletion
G_MODULE_EXPORT gboolean on_gjitenkai_prefs_delete_event(GtkWindow *window, gjitenkai *gjitenkai)
{
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;
}

/**
   callback when the mouse button is released
 */
G_MODULE_EXPORT gboolean on_gjitenkai_search_results_button_release_event(GtkWidget *text_view, GdkEventButton *event, gjitenkai *gjitenkai)
{
  GtkTextIter mouse_iter;
  gint x, y;
  gint trailing;
  gunichar kanji;

  if (event->button != 1)
    return FALSE;

  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(text_view),
                                        GTK_TEXT_WINDOW_WIDGET,
                                        event->x, event->y, &x, &y);

  gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(text_view), &mouse_iter, &trailing, x, y);
  kanji = gtk_text_iter_get_char(&mouse_iter);
  if ((kanji != 0xFFFC) && (kanji != 0) && (isKanjiChar(kanji) == TRUE))
  {
    gchar str_kanji[UTF8_MIN_SIZE];
    to_utf8(kanji, str_kanji);
    display_kanji(gjitenkai->kanjidic, str_kanji);
  }

  return FALSE;
}

/*
  Callback function called when a character is inserted in the entry widget
  check if the entered character is equals to the unicode character of a special
  character (CARRIAGE RETURN, BACKSPACE, DELETE), if so prevent these characters
  to be added in the entry box (happend when using SCIM-Anthy with GTK3)
*/
G_MODULE_EXPORT void on_text_insert_detect_unicode(GtkEntry *entry,
                                                   const gchar *text,
                                                   gint length,
                                                   gint *position,
                                                   gpointer data)
{
  GtkEditable *editable = GTK_EDITABLE(entry);

  //unicode representation of special characters
  char carriage_return[] = "\x0D";
  char backspace[] = "\x08";
  char delete[] = "\x7f";

  if (!g_strcmp0(text, carriage_return))
  {
    //unicode character #u0D (CARRIAGE RETURN) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name(G_OBJECT(editable), "insert_text");

    //emit the activate signal for the entry widget
    g_signal_emit_by_name(editable, "activate");
  }
  else if (!g_strcmp0(text, backspace))
  {
    //unicode character #u08 (BACKSPACE) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name(G_OBJECT(editable), "insert_text");

    //emit the backspace signal for the entry widget
    //and adjust the cursor position
    g_signal_emit_by_name(editable, "backspace");
    (*position)--;
  }
  else if (!g_strcmp0(text, delete))
  {
    //unicode character #u7f (DELETE) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name(G_OBJECT(editable), "insert_text");

    //emit the delete signal of one character for the entry widget
    g_signal_emit_by_name(editable, "delete-from-cursor", GTK_DELETE_CHARS, 1);
  }
}

//Search
///Japanese
G_MODULE_EXPORT void on_menuitem_search_japanese_exact_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_jp = EXACT_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_start_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_jp = START_WITH_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_end_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_jp = END_WITH_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_any_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_jp = ANY_MATCH;
}

///Latin
G_MODULE_EXPORT void on_menuitem_search_latin_whole_expressions_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_lat = EXACT_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_latin_whole_words_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_lat = WORD_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_latin_any_matches_activate(GtkMenuItem *menuitem, gjitenkai *p_gjitenkai)
{
  p_gjitenkai->worddic->match_criteria_lat = ANY_MATCH;
}
