#include "kanjidic.h"

GdkCursor * cursor;

void kanjidic_init (kanjidic *kanjidic){
  const gchar * const * dirs = g_get_system_data_dirs();
  gchar* rest = g_strjoin(G_DIR_SEPARATOR_S,
			  PROJECT_NAME,
			  "ui",
			  UI_DEFINITIONS_FILE_KANJIDIC,
			  NULL);
  gchar* filename = get_file(dirs, rest);
  g_free(rest);

  GError *err = NULL;
  kanjidic->definitions = gtk_builder_new();
  gtk_builder_add_from_file (kanjidic->definitions,
                             filename, &err);
  if (err != NULL) {
    g_printerr
      ("Error while loading kanjidic definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }

  g_free(filename);

  gtk_builder_connect_signals (kanjidic->definitions, kanjidic);

  //init the configuration handler
  kanjidic->settings = conf_init_handler(SETTINGS_KANJIDIC);

  //load configuration
  kanjidic->conf = kanjidic_conf_load(kanjidic);

  //load the kanji dictionary
  dicfile_load(kanjidic->conf->kanjidic, NULL);

  //init the radical and kanji hash
  kanjidic->kanji_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                               (GEqualFunc)g_str_equal);
  kanjidic->rad_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                             (GEqualFunc)g_str_equal);

  //load radical and kanji from the radkfile
  kanjidic->rad_info_list = NULL;
  kanjidic->rad_info_list = load_radkfile(&kanjidic->rad_info_hash,
					  &kanjidic->kanji_info_hash,
					  kanjidic->rad_info_list);

  //init the kanji display style
  //filters default value
  kanjidic->filter_by_stroke = FALSE;
  kanjidic->filter_by_radical = TRUE;
  kanjidic->filter_by_key = FALSE;

  //init the UI
  //filters sensitivity
  set_ui_radical_filter_sensitivity(kanjidic->filter_by_radical, kanjidic);
  set_ui_stroke_filter_sensitivity(kanjidic->filter_by_stroke, kanjidic);
  set_ui_key_filter_sensitivity(kanjidic->filter_by_key, kanjidic);

  //check/uncheck checkbutton with default values
  GtkButton *checkbutton_filter_radicals = (GtkButton*)
    gtk_builder_get_object(kanjidic->definitions,
                           "checkbutton_filter_radicals");

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_radicals),
                               kanjidic->filter_by_radical);

  GtkButton *checkbutton_filter_strokes = (GtkButton*)
    gtk_builder_get_object(kanjidic->definitions, "checkbutton_filter_strokes");

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_strokes),
                               kanjidic->filter_by_stroke);

  GtkButton *checkbutton_filter_key = (GtkButton *)
    gtk_builder_get_object(kanjidic->definitions, "checkbutton_filter_key");

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_key),
                               kanjidic->filter_by_key);

  //init the radical window with the radical buttons
  radical_list_init(kanjidic);

  //init the preference window's widgets
  init_prefs_kanjidic(kanjidic);

  //set the mouse cursor
  GdkDisplay * display = gdk_display_get_default();
  cursor = gdk_cursor_new_for_display(display, GDK_LEFT_PTR);
}

void set_ui_radical_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){

  GtkEntry *entry_filter_radical = (GtkEntry *)
    gtk_builder_get_object(kanjidic->definitions,
                           "entry_filter_radical");

  GtkButton *button_clear = (GtkButton *)
    gtk_builder_get_object(kanjidic->definitions, "button_clear_radical");

  GtkButton *button_show_radical_list = (GtkButton *)
    gtk_builder_get_object(kanjidic->definitions, "button_show_radical_list");

  gtk_widget_set_sensitive(GTK_WIDGET(entry_filter_radical), sensitivity);
  gtk_widget_set_sensitive(GTK_WIDGET(button_clear), sensitivity);
  gtk_widget_set_sensitive(GTK_WIDGET(button_show_radical_list), sensitivity);
}

void set_ui_stroke_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkSpinButton *spinbutton_filter_stroke = (GtkSpinButton *)
    gtk_builder_get_object(kanjidic->definitions, "spinbutton_filter_stroke");

  GtkSpinButton *spinbutton_filter_stroke_diff = (GtkSpinButton *)
    gtk_builder_get_object(kanjidic->definitions,
                           "spinbutton_filter_stroke_diff");

  gtk_widget_set_sensitive(GTK_WIDGET(spinbutton_filter_stroke),
                           sensitivity);

  gtk_widget_set_sensitive(GTK_WIDGET(spinbutton_filter_stroke_diff),
                           sensitivity);
}

void set_ui_key_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkEntry *entry_filter_key = (GtkEntry*)
    gtk_builder_get_object(kanjidic->definitions, "entry_filter_key");

  gtk_widget_set_sensitive(GTK_WIDGET(entry_filter_key), sensitivity);
}

GSList *search_kanji(kanjidic *kanjidic){
  GSList *kanji_list=NULL;

  //filter by strokes
  if(kanjidic->filter_by_stroke){
    GtkSpinButton *spinbutton_filter_stroke = (GtkSpinButton*)
      gtk_builder_get_object(kanjidic->definitions,
                             "spinbutton_filter_stroke");

    GtkSpinButton *spinbutton_filter_stroke_diff = (GtkSpinButton*)
      gtk_builder_get_object(kanjidic->definitions,
                             "spinbutton_filter_stroke_diff");

    gint stroke_filter = gtk_spin_button_get_value_as_int(spinbutton_filter_stroke);
    gint stroke_filter_diff = gtk_spin_button_get_value_as_int(spinbutton_filter_stroke_diff);

    //get all kanji with the entered radicals
    GSList *kanji_by_stroke_list=NULL;
    kanji_by_stroke_list = get_kanji_by_stroke(stroke_filter, stroke_filter_diff,
                                               kanji_by_stroke_list,
                                               kanjidic->conf->kanjidic);

    kanji_list = list_merge_str(kanji_list, kanji_by_stroke_list);
  }

  //filter by radical
  if(kanjidic->filter_by_radical){
    GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions,
                                                                       "entry_filter_radical");
    const gchar *radicals = gtk_entry_get_text(entry_filter_radical);
    //if the entry is empty, ignore the filter
    if(strcmp(radicals, "")){
      //get all kanji with the entered radicals
      GSList *kanji_by_radical_list=NULL;
      kanji_by_radical_list = get_kanji_by_radical(radicals, kanjidic->rad_info_hash);
      kanji_list = list_merge_str(kanji_list, kanji_by_radical_list);
    }
  }

  //filter by key
  if(kanjidic->filter_by_key){
    GtkEntry *entry_filter_key = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions,
                                                                   "entry_filter_key");
    const gchar *key = gtk_entry_get_text(entry_filter_key);

    //if the entry is empty, ignore the filter
    if(strcmp(key, "")){
      GSList *kanji_by_key_list=NULL;
      kanji_by_key_list = get_kanji_by_key(key,
                                           kanji_by_key_list,
                                           kanjidic->conf->kanjidic);
      kanji_list = list_merge_str(kanji_list, kanji_by_key_list);
    }
  }

  return kanji_list;
}

void display_candidates(kanjidic *kanjidic, GSList *kanji_list){
  //anchor in the result textview where to append the 'kanji buttons'
  GtkTextChildAnchor *kanji_results_anchor;
  GtkTextIter kanji_results_iter;

  //DISPLAY
  //with the list of kanji found from the radicals and/or strokes, append a
  //button for each kanji, with the kanji as label

  //get the widget where to append the 'kanji button'
  GtkTextView *textview_kanji_result = (GtkTextView*)
    gtk_builder_get_object(kanjidic->definitions, "textview_kanji_result");
  GtkTextBuffer *textbuffer_kanji_result = (GtkTextBuffer*)
    gtk_builder_get_object(kanjidic->definitions, "textbuffer_kanji_result");

  //clear the results and set the iterator at the begining
  gtk_text_buffer_set_text(textbuffer_kanji_result, "", 0);
  gtk_text_buffer_get_start_iter(textbuffer_kanji_result, &kanji_results_iter);

  GdkWindow *gdk_window = gtk_text_view_get_window (textview_kanji_result,
                                                    GTK_TEXT_WINDOW_TEXT);
  gdk_window_set_cursor(gdk_window, cursor);

  //for each kanji in the list
  for (;
       kanji_list != NULL;
       kanji_list = g_slist_next(kanji_list)) {

    gchar *kanji = (gchar*)kanji_list->data;

    //create a 'candidate kanji' button
    //set markup
    const char *format = "<span font_desc=\"%s\">\%s</span>";
    gchar *markup = g_markup_printf_escaped (format,
                                             kanjidic->conf->kanji_result_font,
                                             kanji);
    GtkWidget *label_kanji = gtk_label_new("");
    gtk_label_set_markup (GTK_LABEL (label_kanji), markup);
    g_free (markup);

    GtkWidget *button_kanji = gtk_button_new();
    gtk_container_add (GTK_CONTAINER (button_kanji), label_kanji);

    g_signal_connect(button_kanji,
                     "clicked",
                     G_CALLBACK(on_button_kanji_clicked),
                     kanjidic);

    //add the button in the textview at the anchor position
    kanji_results_anchor = gtk_text_buffer_create_child_anchor(textbuffer_kanji_result,
                                                               &kanji_results_iter);

    gtk_text_view_add_child_at_anchor(textview_kanji_result,
                                      GTK_WIDGET(button_kanji),
                                      kanji_results_anchor);
  }

  //show what has been added
  gtk_widget_show_all(GTK_WIDGET(textview_kanji_result));
}

void display_kanji(kanjidic *kanjidic, const gchar* kanji){
  //add a button in the history box
  //if the last added kanji is not the same as the kanji to add
  gboolean same_kanji = FALSE;
  if(kanjidic->history){
    const gchar *last_kanji = kanjidic->history->data;
    if(!strcmp(kanji, last_kanji)){
      same_kanji = TRUE;
    }
  }

  if(!same_kanji){
    //add the kanji in the history list
    kanjidic->history = g_slist_prepend(kanjidic->history, g_strdup(kanji));

    //add the kanji in the history widget
    GtkWidget *button_history = gtk_button_new_with_label(kanji);
    g_signal_connect(button_history,
                     "clicked",
                     G_CALLBACK(on_button_kanji_clicked),
                     kanjidic);

    GtkGrid *grid_history = (GtkGrid*)gtk_builder_get_object(kanjidic->definitions,
                                                             "grid_history");
    gtk_grid_attach_next_to(grid_history, button_history, NULL, GTK_POS_TOP, 1, 1);

    gtk_widget_show_all(GTK_WIDGET(grid_history));
  }

  //get the area where to display the kanji
  GtkGrid *grid_kanji_display = (GtkGrid*)
    gtk_builder_get_object(kanjidic->definitions, "grid_kanji_display");

  //clear the previously displayed kanji
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(grid_kanji_display));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);

  //get the  kanji informations from kdic
  ///get the line from the kanji
  gchar *kanji_info_line = get_line_from_dic(kanji, kanjidic->conf->kanjidic);
  ////get the entry from the line
  kanjifile_entry *kanji_data= kanjidic_dicfile_parse_line(kanji_info_line);

  //free line
  g_free(kanji_info_line);


  //Display the kanji and the kanji related informations
  GtkWidget *label_kanji = gtk_label_new(kanji);
  gtk_label_set_selectable (GTK_LABEL(label_kanji), TRUE);

  //set markup
  const char *format = "<span font_desc=\"%s\">\%s</span>";
  gchar *markup = g_markup_printf_escaped (format,
                                           kanjidic->conf->kanji_font,
                                           kanji);
  gtk_label_set_markup (GTK_LABEL (label_kanji), markup);
  g_free (markup);

    //attach
  gtk_grid_attach (grid_kanji_display,
                   (GtkWidget*)label_kanji,
                   0, 0, 2, 1);


  gint i=1;
  GSList *kanji_item_head;  //browse thought the kanji items
  for (kanji_item_head = kanji_item_list;
       kanji_item_head != NULL;
       kanji_item_head = g_slist_next(kanji_item_head)){

    GString *ki_string = g_string_new("");
    GSList *item = NULL;   //the kanji item content to list

    kanji_item *ki = kanji_item_head->data;
    if(ki->active){
      //display the name of the kanji info
      GtkWidget *label_kanji_info_name = gtk_label_new(ki->name);
      gtk_widget_set_halign(label_kanji_info_name, GTK_ALIGN_START);

      gtk_grid_attach (grid_kanji_display,
                       (GtkWidget*)label_kanji_info_name,
                       0, i, 1, 1);

      if(!strcmp(ki->gsettings_name, "radical") && kanjidic->rad_info_list){
        //list radicals without separation chars
        KanjiInfo *kanji_info = g_hash_table_lookup(kanjidic->kanji_info_hash, kanji);
	if(kanji_info){
	  GSList *kanji_info_list;

	  for (kanji_info_list = kanji_info->rad_info_list;
	       kanji_info_list != NULL;
	       kanji_info_list = kanji_info_list->next) {
	    const char* str_radical = (const char*)((RadInfo *)kanji_info_list->data)->radical;
	    ki_string = g_string_append(ki_string, str_radical);
	  }
	}
      }
      else if(!strcmp(ki->gsettings_name, "strokes")){
        //strokes count
        gchar *tmp_entry = g_strdup_printf("%d", kanji_data->stroke);
        ki_string = g_string_append(ki_string, tmp_entry);
        g_free(tmp_entry);
      }
      else if(!strcmp(ki->gsettings_name, "jouyou")){
        //grade level
        gchar *tmp_entry = g_strdup_printf("%d", kanji_data->jouyou);
        ki_string = g_string_append(ki_string, tmp_entry);
        g_free(tmp_entry);
      }
      else if(!strcmp(ki->gsettings_name, "onyomi")){
        item = kanji_data->onyomi;
      }
      else if(!strcmp(ki->gsettings_name, "kunyomi")){
        item = kanji_data->kunyomi;
      }
      else if(!strcmp(ki->gsettings_name, "translation")){
        item = kanji_data->translations;
      }

      //item point to one of the kanji_entry's list to display or NULL
      for (;
           item != NULL;
           item = g_slist_next(item)){

        ki_string = g_string_append(ki_string, item->data);

        //if there is another entry for this definition, append a separation char
        if(g_slist_next(item) != NULL){
          ki_string = g_string_append(ki_string, kanjidic->conf->separator);
        }
      }

      GtkWidget *label_kanji_info = gtk_label_new(ki_string->str);
      gtk_label_set_selectable (GTK_LABEL(label_kanji_info), TRUE);
      gtk_widget_set_halign(GTK_WIDGET(label_kanji_info), GTK_ALIGN_START);
      gtk_label_set_line_wrap(GTK_LABEL(label_kanji_info), TRUE);
      gtk_grid_attach (grid_kanji_display,
                       (GtkWidget*)label_kanji_info,
                       1, i, 1, 1);
      i++;
      g_string_free(ki_string, TRUE);


    }//end if kanji info active
  }
  gtk_widget_show_all(GTK_WIDGET(grid_kanji_display));

  //free memory
  kanjifile_entry_free(kanji_data);
}
