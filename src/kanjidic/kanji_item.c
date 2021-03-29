#include "kanji_item.h"

void kanji_item_list_init(){
  GSList * kanji_item_list = NULL;
  
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Radicals", "radical"));
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Stroke count", "strokes"));
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Translation", "translation"));
  /*kanji_item_list = g_slist_append(kanji_item_list,
    kanji_item_load("Frequancy", "freq"));*/
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Jouyou", "jouyou"));
/*  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Korean", "korean"));
  kanji_item_list = g_slist_append(kanji_item_list,
  kanji_item_load("Pinyin", "pinyin"));*/
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("On yomi", "onyomi"));
  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Kun yomi", "kunyomi"));
/*  kanji_item_list = g_slist_append(kanji_item_list,
				   kanji_item_load("Nanori", "nanori"));
  kanji_item_list = g_slist_append(kanji_item_list,
  kanji_item_load("Bushu", "bushu"));*/


  
};

kanji_item* kanji_item_load(gchar *name, gchar *gsettings_name){
  kanji_item* k_item = g_new0(kanji_item, 1);
  GSettings*  kanjidic_settings = g_settings_new(SETTINGS_KANJIDIC);

  k_item->name =name;
  k_item->gsettings_name = gsettings_name;
  k_item->active = g_settings_get_boolean(kanjidic_settings, gsettings_name);
  k_item->position = 0;
  
  return k_item;
  
}

void kanji_item_save(kanji_item* ki){
  GSettings*  kanjidic_settings = g_settings_new(SETTINGS_KANJIDIC);
  g_settings_set_boolean(kanjidic_settings, ki->gsettings_name, ki->active);
}
