//gcc replace_radical.c ../src/kanjidic/callbacks.c  ../src/kanjidic/kanjifile.c ../src/kanjidic/conf.c ../src/kanjidic/kanji_item.c  ../src/kanjidic/preferences.c ../src/kanjidic/kanjidic.c ../src/kanjidic/kanjiutils.c  ../src/kanjidic/radical_window.c ../src/common/*.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/kanjidic/ -o replace_radical

#include <gtk/gtk.h>

#include "../config.h"
#include "../src/kanjidic/kanjidic.h"

void init(kanjidic *kanjidic)
{
  //init the radical and kanji hash
  kanjidic->kanji_info_hash = g_hash_table_new((GHashFunc)g_str_hash, (GEqualFunc)g_str_equal);
  kanjidic->rad_info_hash = g_hash_table_new((GHashFunc)g_str_hash, (GEqualFunc)g_str_equal);

  //load radical and kanji from the radkfile
  kanjidic->rad_info_list = NULL;
  kanjidic->rad_info_list = load_radkfile(&kanjidic->rad_info_hash,
                                          &kanjidic->kanji_info_hash,
                                          kanjidic->rad_info_list);
}

static gboolean eval_cb(const GMatchInfo *info,
                        GString *res,
                        gpointer data)
{
  kanjidic *kanjidic = data;
  gchar *match;
  gchar *r;

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

int main(int argc, char **argv)
{
  gchar *text = argv[1];
  GRegex *reg = NULL;
  gchar *res = NULL;
  kanjidic k;

  init(&k);

  reg = g_regex_new("＜.+＞", G_REGEX_UNGREEDY, 0, NULL);
  res = g_regex_replace_eval(reg, text, -1, 0, 0, eval_cb, &k, NULL);

  g_printf("%s\n", res);
  return 1;
}
