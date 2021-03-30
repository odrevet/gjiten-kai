#include "worddic.h"

void worddic_init (worddic *p_worddic){
  const gchar * const * dirs = g_get_system_data_dirs();
  gchar *rest = g_strjoin(G_DIR_SEPARATOR_S,
			  PROJECT_NAME,
			  "ui",
			  UI_DEFINITIONS_FILE_WORDDIC,
			  NULL);
  gchar* filename = get_file(dirs, rest);
  g_free(rest);

  GError *err = NULL;
  p_worddic->definitions = gtk_builder_new ();
  gtk_builder_add_from_file (p_worddic->definitions,
                             filename, &err);
  if (err != NULL) {
    g_printerr
      ("Error while loading worddic definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }

  g_free(filename);

  gtk_builder_connect_signals (p_worddic->definitions, p_worddic);

  //set the loading dictionary thread to NULL
  p_worddic->thread_load_dic = NULL;

  //init the configuration handler
  p_worddic->settings = conf_init_handler(SETTINGS_WORDDIC);

  //load configuration
  p_worddic->conf = worddic_conf_load(p_worddic->settings);

  //by default search everything
  p_worddic->match_criteria_jp = ANY_MATCH;
  p_worddic->match_criteria_lat = ANY_MATCH;

  //set the number of entries to display par page result
  p_worddic->entries_per_page = 512;

  init_search_menu(p_worddic);

  //dark theme
  GtkSettings *gsettings = gtk_settings_get_default ();
  g_object_set (G_OBJECT (gsettings),
                "gtk-application-prefer-dark-theme", p_worddic->conf->dark_theme,
                NULL);

  //highlight style of the result text buffer
  GtkTextBuffer *textbuffer_search_results = (GtkTextBuffer*)
    gtk_builder_get_object(p_worddic->definitions,
                           "textbuffer_search_results");

  GtkTextTag *highlight = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "results_highlight",
                                                      "background-rgba",
                                                      p_worddic->conf->results_highlight_color,
                                                      NULL);
  p_worddic->conf->highlight = highlight;

  //japanese definition
  GtkTextTag *jap_def_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                        "japanese_definition",
                                                        "foreground-rgba",
                                                        p_worddic->conf->jap_def.color,
                                                        "font",
                                                        p_worddic->conf->jap_def.font,
                                                        NULL);
  p_worddic->conf->jap_def.tag = jap_def_tag;

  //japanese reading
  GtkTextTag *jap_reading_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                            "japanese_reading",
                                                            "foreground-rgba",
                                                            p_worddic->conf->jap_reading.color,
                                                            "font",
                                                            p_worddic->conf->jap_reading.font,
                                                            NULL);
  p_worddic->conf->jap_reading.tag = jap_reading_tag;

  //sense
  GtkTextTag *sense_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "sense",
                                                      "foreground-rgba",
                                                      p_worddic->conf->sense.color,
                                                      "font",
                                                      p_worddic->conf->sense.font,
                                                      NULL);
  p_worddic->conf->sense.tag = sense_tag;

  //gloss
  GtkTextTag *gloss_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
						      "gloss",
						      "foreground-rgba",
						      p_worddic->conf->gloss.color,
						      "font",
						      p_worddic->conf->gloss.font,
						      NULL);
  p_worddic->conf->gloss.tag = gloss_tag;

  //notes
  GtkTextTag *notes_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "notes",
                                                      "foreground-rgba",
                                                      p_worddic->conf->notes.color,
                                                      "font",
                                                      p_worddic->conf->notes.font,
                                                      NULL);
  p_worddic->conf->notes.tag = notes_tag;

  //init the verb de-inflection mechanism
  p_worddic->vinfl_list = init_inflection();

  //Init the preference window's widgets
  init_prefs_window(p_worddic);

  //init cursors
  GdkDisplay * display = gdk_display_get_default();

  cursor_selection = gdk_cursor_new_for_display(display, GDK_ARROW);
  cursor_default = gdk_cursor_new_for_display(display, GDK_XTERM);

  //init download location
  //gchar *download_location = g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);

  if(!p_worddic->conf->dicfile_list){
    GtkDialog *dialog = (GtkDialog*)gtk_builder_get_object(p_worddic->definitions,
                                                           "dialog_welcome");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }

  //add a callback when scrolling to the edge of the result only if GTK >= 3.16
#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
  GtkScrolledWindow *scrolledwindow_search_result = (GtkScrolledWindow*)
    gtk_builder_get_object(p_worddic->definitions, "scrolledwindow_search_result");
  g_signal_connect(scrolledwindow_search_result,
		   "edge-reached",
		   G_CALLBACK(on_worddic_search_results_edge_reached),
		   p_worddic);
#endif

}

void init_search_menu(worddic *p_worddic){
  //get the search options
  gint match_criteria_jp = p_worddic->match_criteria_jp;
  gint match_criteria_lat = p_worddic->match_criteria_lat;

  GtkRadioMenuItem* radio_jp = NULL;
  GtkRadioMenuItem* radio_lat = NULL;

  switch(match_criteria_lat){
  case EXACT_MATCH:
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                          "menuitem_search_whole_expression");
    break;
  case WORD_MATCH:
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                          "menuitem_search_latin_word");
    break;
  case ANY_MATCH:
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                          "menuitem_search_latin_any");
    break;
  }

  switch(match_criteria_jp){
  case EXACT_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                         "menuitem_search_japanese_exact");
    break;
  case START_WITH_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                         "menuitem_search_japanese_start");
    break;
  case END_WITH_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                         "menuitem_search_japanese_end");
    break;
  case ANY_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(p_worddic->definitions,
                                                         "menuitem_search_japanese_any");
    break;
  }

  if(radio_jp)
    gtk_check_menu_item_set_active((GtkCheckMenuItem *)radio_jp, TRUE);

  if(radio_lat)
    gtk_check_menu_item_set_active((GtkCheckMenuItem *)radio_lat, TRUE);
}

gboolean worddic_search(const gchar *search_text, worddic *worddic){
  //wait if a dictionary is being loaded in a thread
  if(worddic->thread_load_dic){
    g_thread_join(worddic->thread_load_dic);
  }

  //search expression instance
  search_expression search_expr;
  search_expr.search_criteria_jp = worddic->match_criteria_jp;
  search_expr.search_criteria_lat = worddic->match_criteria_lat;
  search_expr.langs = worddic->conf->langs;

  //free the previous search results
  worddic->results = g_list_first(worddic->results);
  g_list_free_full(worddic->results, (GDestroyNotify)dicresult_free);
  worddic->results = NULL;

  //detect is the search is in japanese
  gboolean is_jp = detect_japanese(search_text);

  //convert fullwidth regex punctuation to halfwidth regex puncutation
  gchar *search_text_half = regex_full_to_half(search_text);

  //search in the dictionaries
  GSList *dicfile_node = worddic->conf->dicfile_list;
  WorddicDicfile *dicfile;

  GList *results=NULL;  //matched dictionary entries to return in a dicresult

  //get the search result text entry to display matches
  GtkTextBuffer *textbuffer_search_results =
    (GtkTextBuffer*)gtk_builder_get_object(worddic->definitions,
                                           "textbuffer_search_results");

  //clear the display result buffer
  gtk_text_buffer_set_text(textbuffer_search_results, "", 0);

  if(!worddic->conf->dicfile_list){
    GtkDialog *dialog = (GtkDialog*)gtk_builder_get_object(worddic->definitions,
                                                           "dialog_welcome");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }

  //in each dictionaries
  while (dicfile_node != NULL){
    dicfile = dicfile_node->data;

    //do not search in this dictionary if it's not active
    if(!dicfile->is_active){
      dicfile_node = g_slist_next(dicfile_node);
      continue;
    }

    //if this dictionary was not loaded, parse it now
    if(!dicfile->entries && worddic_dicfile_open(dicfile)){
      //update the UI in the preference pane
      GtkListStore *model = (GtkListStore*)gtk_builder_get_object(worddic->definitions,
                                                                  "liststore_dic");
      gint i = g_slist_position(worddic->conf->dicfile_list, dicfile_node);
      GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
      //set the model
      GtkTreeIter  iter;
      gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
      gtk_list_store_set (GTK_LIST_STORE (model), &iter, 3, TRUE, -1);
      gtk_tree_path_free (path);
    }

    //search only on japanese text
    if(is_jp){
      //search for deinflections
      if(worddic->conf->verb_deinflection && worddic->vinfl_list){
        results = g_list_concat(results, search_inflections(worddic->vinfl_list, dicfile, search_text_half));
      }

      //search hiragana on katakana
      if (worddic->conf->search_hira_on_kata &&
          hasKatakanaString(search_text_half)) {
        gchar *hiragana = kata_to_hira(search_text_half);
        search_expr.search_text = hiragana;

        results = g_list_concat(results, dicfile_search(dicfile,
                                                        &search_expr,
                                                        "from katakana",
                                                        GIALL,
                                                        1));
        g_free(hiragana);  //free memory
      }

      //search katakana on hiragana
      if (worddic->conf->search_kata_on_hira &&
          hasHiraganaString(search_text_half)) {
        gchar *katakana = hira_to_kata(search_text_half);
        search_expr.search_text = katakana;

        results = g_list_concat(results, dicfile_search(dicfile,
                                                        &search_expr,
                                                        "from hiragana",
                                                        GIALL,
                                                        1));
        g_free(katakana); //free memory
      }
    }

    //standard search
    search_expr.search_text = search_text_half;
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    &search_expr,
                                                    NULL,
                                                    GIALL,
                                                    is_jp));

    //get the next node in the dic list
    dicfile_node = g_slist_next(dicfile_node);
  }

  //assign the result list to the worddic result list
  worddic->results = results;

  //print the first page
  print_entries(textbuffer_search_results, worddic);

  //free memory
  g_free(search_text_half);

  if(results){
    return TRUE;
  }
  else{
    return FALSE;
  }
}

void print_unit(GtkTextBuffer *textbuffer,
                gchar *text,
                unit_style *style){
  gtk_text_buffer_insert_at_cursor(textbuffer,
                                   style->start,
                                   strlen(style->start));

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (textbuffer, &iter);
  gtk_text_buffer_insert_with_tags(textbuffer,
                                   &iter,
                                   text,
                                   strlen(text),
                                   style->tag,
                                   NULL);

  gtk_text_buffer_insert_at_cursor(textbuffer,
                                   style->end,
                                   strlen(style->end));
}

void print_entries(GtkTextBuffer *textbuffer, worddic *p_worddic){
  gint entry_number=0;

  while(p_worddic->results &&
	entry_number <= p_worddic->entries_per_page){

    dicresult *p_dicresult = p_worddic->results->data;

    GjitenDicentry *entry = p_dicresult->entry;

    gchar *match = p_dicresult->match;
    gchar *comment = p_dicresult->comment;

    //browse list
    GSList *unit = NULL;

    //text to print
    gchar* text = NULL;

    GtkTextIter iter_from;
    gtk_text_buffer_get_end_iter(textbuffer, &iter_from);

    //create a mark, indicating the start where the new entry is writed
    GtkTextMark *start_mark =
      gtk_text_buffer_create_mark (textbuffer, NULL, &iter_from, TRUE);

    gtk_text_buffer_insert_at_cursor(textbuffer, "\n", strlen("\n"));

    //Japanese definition
    for(unit = entry->jap_definition; unit != NULL; unit = unit->next){
      text = (gchar*)unit->data;
      print_unit(textbuffer, text, &p_worddic->conf->jap_def);
    }

    //reading
    if(entry->jap_reading){
      for(unit = entry->jap_reading;unit != NULL;unit = unit->next){
        text = (gchar*)unit->data;
        print_unit(textbuffer, text, &p_worddic->conf->jap_reading);
      }
    }

    //comment
    if(comment)print_unit(textbuffer, comment, &p_worddic->conf->notes);

    //sense
    for(unit = entry->sense; unit != NULL; unit = unit->next){
      sense *p_sense = unit->data;
      GSList *gloss_list = NULL;
      GSList *gloss_to_print = NULL;
      for(gloss_list = p_sense->gloss;
          gloss_list != NULL;
          gloss_list = gloss_list->next){
	gloss *p_gloss = (gloss*)gloss_list->data;
	GSList *p_lang_node =  p_worddic->conf->langs;

	gboolean lang_activated = FALSE;
	while(p_lang_node && lang_activated == FALSE){
	  lang *p_lang = p_lang_node->data;
	  if(!strcmp(p_gloss->lang, p_lang->code) && p_lang->active){
	    lang_activated = p_lang->active;
	    break;
	  }
	  p_lang_node = p_lang_node->next;
	}

	if(lang_activated)gloss_to_print = g_slist_prepend(gloss_to_print, p_gloss);
      }

      if(gloss_to_print){
	gtk_text_buffer_insert_at_cursor(textbuffer, p_worddic->conf->sense.start,
					 strlen(p_worddic->conf->sense.start));
	////General Informations
	GSList *GI = NULL;
	for(GI = p_sense->general_informations;
	    GI != NULL;
	    GI = GI->next){
	  text = (gchar*)GI->data;
	  print_unit(textbuffer, text, &p_worddic->conf->notes);
	}

	GSList *gloss_to_print_node = NULL;
	for(gloss_to_print_node = gloss_to_print;
	    gloss_to_print_node != NULL;
	    gloss_to_print_node = gloss_to_print_node->next){
	  gloss *p_gloss = gloss_to_print_node->data;
	  print_unit(textbuffer, p_gloss->content, &p_worddic->conf->gloss);
	}

	gtk_text_buffer_insert_at_cursor(textbuffer, p_worddic->conf->sense.end,
					 strlen(p_worddic->conf->sense.end));
      }
    }

    //set the iter from where to search text to highlight from the start mark
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter_from, start_mark);

    //search and highlight the matched expression from the iter_from
    highlight_result(textbuffer,
                     p_worddic->conf->highlight,
                     match,
                     &iter_from);

    p_worddic->results = p_worddic->results->next;
    entry_number++;
  }
}

void highlight_result(GtkTextBuffer *textbuffer,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight,
                      GtkTextIter *iter_from){
  gboolean has_iter;
  GtkTextIter match_start, match_end;

  do{
    //search where the result string is located in the result buffer
    has_iter = gtk_text_iter_forward_search (iter_from,
                                             text_to_highlight,
                                             GTK_TEXT_SEARCH_VISIBLE_ONLY,
                                             &match_start,
                                             &match_end,
                                             NULL);

    if(has_iter){
      //highlight at this location
      gtk_text_buffer_apply_tag (textbuffer,
                                 highlight,
                                 &match_start,
                                 &match_end);

      //next iteration starts from the end of this iteration
      *iter_from = match_end;
    }

  }while(has_iter);
}


void worddic_free(worddic *p_worddic){
  g_list_free_full(p_worddic->results, (GDestroyNotify)dicresult_free);
  g_slist_free_full(p_worddic->vinfl_list, (GDestroyNotify)free_vinfl);
  g_free(p_worddic);
}
