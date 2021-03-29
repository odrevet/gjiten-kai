#include "kanjidic.h"
#include "kanji_item.h"

GSList *kanji_item_list = NULL;

gint cmp_name(gconstpointer a,
              gconstpointer b){
  const kanji_item *ki1 = a;
  const kanji_item *ki2 = b;
  return strcmp(ki1->name, ki2->name);
}

G_MODULE_EXPORT void on_kanji_item_toggled(GtkCheckButton* checkbutton, kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton));
  const gchar* name = gtk_button_get_label(GTK_BUTTON(checkbutton));

  //search for the kanji item from the button name
  kanji_item tmp_ki;
  tmp_ki.name = name;
  GSList* kanji_item_head = g_slist_find_custom (kanji_item_list, &tmp_ki, cmp_name);

  if(kanji_item_head){
    kanji_item *ki = kanji_item_head->data;
    ki->active = toggled;
    kanji_item_save(ki);
  }
}

//init
void init_prefs_kanjidic(kanjidic *kanjidic){
  //init the kdic file chooser button title with the path of the dict
  GtkFileChooserButton *filechooserbutton = (GtkFileChooserButton*)
    gtk_builder_get_object(kanjidic->definitions, "filechooserbutton_kdic");

  gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(filechooserbutton),
                                   kanjidic->conf->kanjidic->path);

  //init the kanji font chooser
  GtkFontButton *font_button = (GtkFontButton*)gtk_builder_get_object(kanjidic->definitions,
                                                                      "fontbutton_kanji");
  gtk_font_chooser_set_font(GTK_FONT_CHOOSER(font_button), kanjidic->conf->kanji_font);

  //init the separator entry
  GtkEntry *entry_separator = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions,
                                                                "entry_separator");
  gtk_entry_set_text(entry_separator, kanjidic->conf->separator);

  //init the kanji_result font chooser
  GtkFontButton *kanji_result_font_button = (GtkFontButton*)
    gtk_builder_get_object(kanjidic->definitions, "fontbutton_kanji_result");

  gtk_font_chooser_set_font (GTK_FONT_CHOOSER(kanji_result_font_button), kanjidic->conf->kanji_result_font);

  //init the item list, expose what must be displayed in the kanji area
  GtkBox *box_items = (GtkBox*)gtk_builder_get_object(kanjidic->definitions,
                                                      "box_kdic_items");

  kanji_item_list_init();
  GSList* kanji_item_head = kanji_item_list;
  while (kanji_item_head != NULL) {
    //add the kanji item with a checkbox
    GtkBox *box_item = (GtkBox*)gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    kanji_item *kanji_item = kanji_item_head->data;
    GtkCheckButton *display_item = (GtkCheckButton*)gtk_check_button_new_with_label(kanji_item->name);
    gtk_toggle_button_set_active ((GtkToggleButton*)display_item, kanji_item->active);

    gtk_box_pack_start(box_item, GTK_WIDGET(display_item), TRUE, FALSE, 0);


    g_signal_connect(display_item,
                     "toggled",
                     G_CALLBACK(on_kanji_item_toggled),
                     kanjidic);
    gtk_widget_set_halign(GTK_WIDGET(box_item), GTK_ALIGN_START);

    //insert the box into the box of items
    gtk_box_pack_start(box_items, GTK_WIDGET(box_item), TRUE, FALSE, 0);

    kanji_item_head = g_slist_next(kanji_item_head);
  }
}

//callback
G_MODULE_EXPORT void on_entry_separator_changed(GtkEntry *entry, kanjidic *kanjidic){
  kanjidic->conf->separator = gtk_entry_get_text(entry);
  kanjidic_conf_save(kanjidic->conf, kanjidic->settings);
}

G_MODULE_EXPORT void on_kanjidic_button_OK_clicked(GtkButton* button, kanjidic *kanjidic){
  GtkDialog *dialog_prefs = (GtkDialog*)gtk_builder_get_object(kanjidic->definitions,
							       "dialog_preferences");
  gtk_widget_hide (GTK_WIDGET(dialog_prefs));

}

G_MODULE_EXPORT void on_fontbutton_kanji_font_activated(GtkFontChooser *self,
							kanjidic *kanjidic){
  gchar *font = gtk_font_chooser_get_font(self);
  g_free(kanjidic->conf->kanji_font);
  kanjidic->conf->kanji_font = font;
  g_object_set(kanjidic->texttag_kanji, "font",
               kanjidic->conf->kanji_font, NULL);
  kanjidic_conf_save(kanjidic->conf, kanjidic->settings);
}

G_MODULE_EXPORT void on_fontbutton_kanji_result_font_activated(GtkFontChooser *self,
							       kanjidic *kanjidic){
  g_free(kanjidic->conf->kanji_result_font);
  gchar *font = gtk_font_chooser_get_font(self);
  kanjidic->conf->kanji_result_font = font;
  kanjidic_conf_save(kanjidic->conf, kanjidic->settings);
}

G_MODULE_EXPORT void on_filechooserbutton_kdic_file_set(GtkFileChooserButton *filechooserbutton,
							kanjidic *kanjidic){
  GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(filechooserbutton));

  //free the previously used kanjidic
  dicutil_unload_dic(kanjidic->conf->kanjidic);
  //reload the conf
  g_free(kanjidic->conf->kanjidic);
  kanjidic->conf->kanjidic = g_new0(GjitenDicfile, 1);
  kanjidic->conf->kanjidic->path = g_file_get_path(file);
  kanjidic->conf->kanjidic->name = g_file_get_basename(file);

  //init the kanjidic with the newly selected path
  dicfile_load(kanjidic->conf->kanjidic, NULL);

  kanjidic_conf_save(kanjidic->conf, kanjidic->settings);

}

//hide and prevent deletion
G_MODULE_EXPORT gboolean on_dialog_preferences_delete_event(GtkWindow *window,
                                                            kanjidic *kanjidic){
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;
}
