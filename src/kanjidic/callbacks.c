#include "kanjidic.h"
#include "kanjiutils.h"
#include "kanjifile.h"
#include "radical_window.h"
#include "kanji_item.h"


///////////////////
//Filters callbacks
G_MODULE_EXPORT void on_checkbutton_filter_strokes_toggled(GtkCheckButton *check_button,
							   kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set widgets sensitivity
  set_ui_stroke_filter_sensitivity(toggled, kanjidic);

  //set the boolean variable
  kanjidic->filter_by_stroke = toggled;

}

G_MODULE_EXPORT void on_checkbutton_filter_radicals_toggled(GtkCheckButton *check_button,
							    kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set the boolean variable
  kanjidic->filter_by_radical = toggled;

  //set widgets sensitivity
  set_ui_radical_filter_sensitivity(toggled, kanjidic);
}

G_MODULE_EXPORT void on_checkbutton_filter_key_toggled(GtkCheckButton *check_button,
						       kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set the boolean variable
  kanjidic->filter_by_key = toggled;

  //set widgets sensitivity
  set_ui_key_filter_sensitivity(toggled, kanjidic);
}
/**
   When a character is inserted in the filter by radical entry
   -Check if this is a kanji, if not, remove it
   -if this is a non-radical kanji, replace it by it's radicals
*/
G_MODULE_EXPORT void on_entry_filter_radical_insert_text(GtkEntry    *entry,
                                                         const gchar *text,
                                                         gint         length,
                                                         gint        *position,
                                                         kanjidic    *kanjidic){

  //if unicode special character, do nothing
  char carriage_return[] = "\x0D";
  char backspace[] = "\x08";
  char delete[] = "\x7f";

  if((!strcmp(text, carriage_return)) ||
     (!strcmp(text, backspace)) ||
     (!strcmp(text, delete))){
    return;
  }

  //get the entered character in utf8 format
  gunichar unichar = g_utf8_get_char(text);
  gchar utf8char[UTF8_MIN_SIZE];
  int at = g_unichar_to_utf8(unichar, utf8char);
  utf8char[at] = '\0';

  //get the editable object from the entry to edit the content
  GtkEditable *editable = GTK_EDITABLE(entry);

  //if the kanji is a radical (kanji is found in the rad_info_hash), quit
  GSList *all_radical_list = (GSList *)g_hash_table_get_keys(kanjidic->rad_info_hash);
  for (;
       all_radical_list != NULL;
       all_radical_list = g_slist_next(all_radical_list)) {
    if(!strcmp(all_radical_list->data, text)){
      return;
    }
  }


  //if this is not a kanji, do not insert
  if(!isKanjiChar(unichar)){
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");
    return;
  }

  //if the entered character is alderly in the list entry, do not insert it
  const gchar *radicals = gtk_entry_get_text(entry);
  const gchar* radstrg_ptr = radicals;
  gint radnum;                   //number of character in radstrg

  radnum = g_utf8_strlen(radicals, -1);
  if (radnum != 0){
    radstrg_ptr = radicals;

    //for every characters in the entry
    gunichar uniradical;
    while( (uniradical = g_utf8_get_char(radstrg_ptr)) ){

      gchar radical[UTF8_MIN_SIZE];
      int at = g_unichar_to_utf8(uniradical, radical);
      radical[at] = '\0';

      if(!strcmp(radical, text)){
        g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");
      }

      radstrg_ptr = g_utf8_next_char(radstrg_ptr);
    }
  }

  //get the radicals of the kanji and insert them into the entry
  GSList* kanji_radical_list = get_radical_of_kanji(unichar,
						    kanjidic->kanji_info_hash);

  //if the kanji_radical_list size if of only one character,
  //and the kanji inserted is the radical found, return.
  //(prevent infinit recursivity)
  if(g_slist_length(kanji_radical_list) == 1 &&
     !strcmp(utf8char, kanji_radical_list->data))return;

  //insert radicals into the entry
  for (;
       kanji_radical_list != NULL;
       kanji_radical_list = g_slist_next(kanji_radical_list)) {

    gtk_editable_insert_text(editable,
                             kanji_radical_list->data,
                             strlen(kanji_radical_list->data),
                             position);
  }

  //do not insert the kanji
  g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

}

G_MODULE_EXPORT void on_button_clear_radical_clicked(GtkButton* button, kanjidic *kanjidic){
  //get the radical entry
  GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions,
                                                                     "entry_filter_radical");

  gtk_entry_set_text(entry_filter_radical, "");

  radical_buttons_update(kanjidic);

}

G_MODULE_EXPORT void on_entry_filter_radical_activate(GtkWidget *entry, kanjidic *kanjidic){
  GSList *kanji_list = search_kanji(kanjidic);
  display_candidates(kanjidic, kanji_list);
}

//click on a 'kanji button': display the kanji information of this kanji
G_MODULE_EXPORT void on_button_kanji_clicked(GtkButton *button, kanjidic *kanjidic) {
  //the label of the button is the kanji to be searched/displayed
  GtkWidget *child = gtk_bin_get_child(GTK_BIN(button));
  const gchar* kanji = gtk_label_get_text(GTK_LABEL(child));
  display_kanji(kanjidic, kanji);
}

G_MODULE_EXPORT void on_button_search_clicked(GtkWidget *widget, kanjidic *kanjidic) {
  GSList *kanji_list = search_kanji(kanjidic);
  display_candidates(kanjidic, kanji_list);
}

G_MODULE_EXPORT void on_button_show_radical_list_clicked(GtkButton *button, kanjidic *kanjidic){
  radical_buttons_update(kanjidic);

  GtkWindow *radicals = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions,
							   "radical_list");

  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(radicals), 320, 220);
  gtk_widget_show_all ((GtkWidget*)radicals);

  GdkWindow *gdk_radicals = gtk_widget_get_window(GTK_WIDGET(radicals));
  gdk_window_raise(gdk_radicals);
}

//Radical list callbacks
//prevent delete and hide
G_MODULE_EXPORT gboolean on_radical_list_delete_event(GtkWindow *window, kanjidic *kanjidic){
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;
}

//a radical button in the radical button list has been clicked
G_MODULE_EXPORT void on_radical_button_clicked(GtkButton *button, kanjidic *kanjidic){

  //get the clicked kanji
  const gchar* radical = gtk_button_get_label(button);

  //add it to the entry filter
  GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions,
                                                                     "entry_filter_radical");
  GtkEditable *editable = GTK_EDITABLE(entry_filter_radical);

  gint position = 0;

  //Check if the clicked radical is alderly present in the radical list
  gboolean alderly_present = FALSE;
  const gchar *kptr = gtk_entry_get_text(entry_filter_radical);
  gunichar radical_in_searchentry;
  gunichar radical_clicked = g_utf8_get_char(radical);
  while ((radical_in_searchentry = g_utf8_get_char(kptr))){
    if(radical_clicked == radical_in_searchentry){
      alderly_present = TRUE;
      break;
    }
    position++;
    kptr = g_utf8_next_char(kptr);
  }

  if(alderly_present){
    gtk_editable_delete_text(editable, position, position + 1);
  }
  else{
    gtk_editable_insert_text(editable,
			     radical,
			     strlen(radical),
			     &position);
  }

  //update the radical list window button
  radical_buttons_update(kanjidic);

}

//about
G_MODULE_EXPORT void on_kanjidic_menuitem_help_about_activate(GtkMenuItem *menuitem,
                                                              kanjidic *kanjidic){
  GtkWindow *window_about = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions,
                                                               "aboutdialog");
  gtk_dialog_run(GTK_DIALOG(window_about));
  gtk_widget_hide (GTK_WIDGET(window_about));
}
