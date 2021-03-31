#ifndef KANJI_FILE
#define KANJI_FILE

#include <glib.h>

#include "../gjitenkai/dicfile.h"

#define KBUFSIZE 500

typedef struct kanjifile_entry_t
{
  gchar *kanji;
  gint stroke;
  gint unicode;
  gint jouyou;
  GSList *translations;
  GSList *kunyomi;
  GSList *onyomi;
} kanjifile_entry;

/**
   Search in the kanjidic the line corresponding of the given kanji
 */
gchar *get_line_from_dic(const gchar *kanji, GjitenDicfile *kanjidic);

/**
   Parse a line of the kdic
   @param line of the kdic
 */
kanjifile_entry *kanjidic_dicfile_parse_line(const gchar *kstr);

void kanjifile_entry_free(kanjifile_entry *p_entry);

#endif
