#ifndef KANJIDIC_H
#define KANJIDIC_H

#include <gtk/gtk.h>

#include "../gjitenkai/conf.h"

#include "conf.h"
#include "constants.h"
#include "kanjiutils.h"
#include "kanji_item.h"
#include "../gjitenkai/dicfile.h"

#define UI_DEFINITIONS_FILE_KANJIDIC "kanjidic.ui"

typedef struct kanjidic_t
{
  GtkBuilder *definitions;

  GSettings *settings;
  struct _KanjidicConfig *conf;

  //with these two hash, search can be performed on kanji and radicals
  GHashTable *kanji_info_hash;  //kanji   -> list of kanjiinfo
  GHashTable *rad_info_hash;    //radical ->  list of radicalinfo
  GSList *rad_info_list;         //list of radicalinfo

  //search options
  gboolean filter_by_stroke;
  gboolean filter_by_radical;
  gboolean filter_by_key;

  //kanji tag style in the kanji display
  GtkTextTag *texttag_kanji;

  //history of displayed kanji
  GSList *history;
} kanjidic;


extern void on_button_kanji_clicked(GtkButton *button, kanjidic *kanjidic);
extern void radical_list_init(kanjidic *kanjidic);
extern void init_prefs_kanjidic(kanjidic *kanjidic);

void kanjidic_init (kanjidic * );

/**
   Set the sensitivity of the stroke filter widgets
 */
void set_ui_stroke_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Set the sensitivity of the radical filter widgets
 */
void set_ui_radical_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Set the sensitivity of the key filter widgets
 */
void set_ui_key_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Search kanji according to the search filter values
 */
GSList* search_kanji(kanjidic *kanjidic);
void display_candidates(kanjidic *kanjidic, GSList *kanji_list);

void display_kanji(kanjidic *kanjidic, const gchar* kanji);

#endif
