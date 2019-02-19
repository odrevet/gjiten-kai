#include "kanjifile.h"

char* get_line_from_dic(const gchar *kanji, GjitenDicfile *kanjidic) {
  gint roff, rlen;
  gchar *repstr = g_new0(gchar, 1024);
  guintptr respos;

  search_string(SRCH_START, kanjidic, kanji,
                &respos, &roff, &rlen, repstr);

  return repstr;
}

kanjifile_entry *kanjidic_dicfile_parse_line(const gchar *kstr){
  char word[KBUFSIZE];
  gint pos=0;

  kanjifile_entry *entry = g_new0(kanjifile_entry, 1);
  gchar *translation;

  //first character is the kanji
  pos = get_word(word, kstr, sizeof(word), pos);
  entry->kanji = strdup(word);

  while((pos = get_word(word, kstr, sizeof(word), pos))){
    //the first character of a word indicates it's purpose
    char first_char = word[0];

    switch(first_char){
    case 'S':
      //Stroke number
      sscanf(word, "S%d", &entry->stroke);
      break;
    case 'U':
      //Unicode number
      sscanf(word, "U%d", &entry->unicode);
      break;
    case 'G':
      //Grade (jouyou)
      sscanf(word, "G%d", &entry->jouyou);
      break;
    case '{':
      translation = strdup(word+1);  //+1 to skip the { character.
      entry->translations = g_slist_append(entry->translations, translation);
      break;
    default:
      //check if katakana (onyomi) or hiragana (kunyomi)
      if (hasKatakanaString(word)){
        entry->onyomi = g_slist_append(entry->onyomi, strdup(word));
      }
      else if (hasHiraganaString(word)){
        entry->kunyomi = g_slist_append(entry->kunyomi, strdup(word));
      }

      break;
    }
  }

  return entry;
}

void kanjifile_entry_free(kanjifile_entry* p_entry)
{
  g_free(p_entry->kanji);
  p_entry->kanji = NULL;

  g_slist_free_full(p_entry->translations, g_free);
  p_entry->translations = NULL;

  g_slist_free_full(p_entry->kunyomi, g_free);
  p_entry->kunyomi = NULL;

  g_slist_free_full(p_entry->onyomi, g_free);
  p_entry->onyomi = NULL;

  g_free(p_entry);
}
