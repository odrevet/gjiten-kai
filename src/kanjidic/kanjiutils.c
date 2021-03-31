#include "kanjiutils.h"

GSList *load_radkfile(GHashTable **pp_rad_info_hash,
                      GHashTable **pp_kanji_info_hash,
                      GSList *rad_info_list)
{
  gint rad_cnt = 0;
  gchar *radkfile_ptr;
  gchar *radkfile_end;
  RadInfo *rad_info = NULL;
  KanjiInfo *kanji_info;

  GHashTable *rad_info_hash = *pp_rad_info_hash;
  GHashTable *kanji_info_hash = *pp_kanji_info_hash;

  const gchar *const *dirs = g_get_system_data_dirs();
  gchar *rest = g_strjoin(G_DIR_SEPARATOR_S, PROJECT_NAME, RADKFILE_NAME, NULL);
  gchar *filename = get_file(dirs, rest);
  g_free(rest);

  gchar *radkfile = NULL;
  long unsigned int length;
  g_file_get_contents(filename, &radkfile, &length, NULL);

  if (radkfile == NULL)
  {
    g_printf("failed to read radkfile '%s'\n", filename);

    rad_info_hash = NULL;
    kanji_info_hash = NULL;
    return NULL;
  }

  g_free(filename);

  radkfile_end = radkfile + strlen(radkfile);
  radkfile_ptr = radkfile;

  //parse the content of the file
  while ((radkfile_ptr < radkfile_end) && (radkfile_ptr != NULL))
  {

    //if comment (first char on this line is #), skip this line
    if (*radkfile_ptr == '#')
    {
      radkfile_ptr = get_EOL(radkfile_ptr, radkfile_end);
      continue;
    }

    //if radical info line (first char on this line is $)
    if (*radkfile_ptr == '$')
    {
      rad_cnt++; //Increase number of radicals found
      radkfile_ptr = g_utf8_next_char(radkfile_ptr);

      //move the pointer forward until the character is wide (kanji)
      while (g_unichar_iswide(g_utf8_get_char(radkfile_ptr)) == FALSE)
      {
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }

      //new rad_info to be stored in the rad_info_hash and rad_info_list
      rad_info = g_new0(RadInfo, 1);
      rad_info->kanji_info_list = NULL;
      rad_info_list = g_slist_prepend(rad_info_list, rad_info);

      //store radical character
      //the characters in the file are in UTF8 format. We need unicode.
      gunichar utf8radical = g_utf8_get_char(radkfile_ptr);
      gchar *p_str_radical = g_new0(gchar, UTF8_MIN_SIZE);
      g_unichar_to_utf8(utf8radical, p_str_radical);
      rad_info->radical = p_str_radical;

      //Find stroke number (move until digit detected)
      while (g_ascii_isdigit(*radkfile_ptr) == FALSE)
      {
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }

      //Store the stroke number
      rad_info->strokes = atoi(radkfile_ptr);

      //insert this radical as key and the info as value
      g_hash_table_insert(rad_info_hash, (gpointer)rad_info->radical, rad_info);

      //Goto next line
      radkfile_ptr = get_EOL(radkfile_ptr, radkfile_end);
    }
    else
    {
      //search the kanji to be stored in the list of the kanji key / radical info list
      //the kanji are located between radical $ markers and the radical info
      while ((*radkfile_ptr != '$') && (radkfile_ptr < radkfile_end))
      {
        if (*radkfile_ptr == '\n')
        {
          radkfile_ptr++;
          continue;
        }

        gunichar utf8kanji = g_utf8_get_char(radkfile_ptr);

        gchar *kanji = g_new0(gchar, UTF8_MIN_SIZE);
        g_unichar_to_utf8(utf8kanji, kanji);

        //search in the kanji infohash if this kanji is alderly present,
        //if not, create a new kanji and add it
        kanji_info = g_hash_table_lookup(kanji_info_hash, kanji);
        if (kanji_info == NULL)
        {
          kanji_info = g_new0(KanjiInfo, 1);
          kanji_info->rad_info_list = NULL;
          kanji_info->kanji = kanji;

          //insert this kanji as a key and the kanji info as value
          g_hash_table_insert(kanji_info_hash, (gpointer)kanji, (gpointer)kanji_info);
        }

        //add the kanji and the radical info in their respective lists
        kanji_info->rad_info_list = g_slist_prepend(kanji_info->rad_info_list, rad_info);
        rad_info->kanji_info_list = g_slist_prepend(rad_info->kanji_info_list, kanji_info);

        //navigate to next character
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }
    }
  }

  return rad_info_list;
}

GSList *get_radical_of_kanji(gunichar kanji, GHashTable *kanji_info_hash)
{
  GSList *kanji_info_list = NULL;
  GSList *radical_list = NULL; //list of radical to be returned

  //convert to UTF8
  gchar utf8kanji[UTF8_MIN_SIZE];
  int at = g_unichar_to_utf8(kanji, utf8kanji);
  utf8kanji[at] = '\0';

  //lookup in the kanji info hash and get the kanji info
  KanjiInfo *kanji_info = g_hash_table_lookup(kanji_info_hash, utf8kanji);

  if (kanji_info == NULL)
  {
    return NULL;
  }

  //puts the radical field of the kanji_info into a list
  for (kanji_info_list = kanji_info->rad_info_list;
       kanji_info_list != NULL;
       kanji_info_list = kanji_info_list->next)
  {
    radical_list = g_slist_prepend(radical_list,
                                   (gpointer)((RadInfo *)kanji_info_list->data)->radical);
  }

  return radical_list;
}

GSList *get_kanji_by_key(const gchar *srchkey, GSList *list, GjitenDicfile *dicfile)
{
  gint srch_resp = 0, roff = 0, rlen = 0;
  gchar repstr[1024];
  guintptr respos, oldrespos;
  gint loopnum = 0;
  gint srchpos = 0;

  srch_resp = search_string(SRCH_START, dicfile, srchkey, &respos, &roff, &rlen, repstr);

  if (srch_resp != SRCH_OK)
    return NULL;
  oldrespos = srchpos = respos;

  kanjifile_entry *entry = kanjidic_dicfile_parse_line(repstr);
  list = g_slist_prepend(list, entry->kanji);

  while (roff != 0)
  {
    oldrespos = respos;
    srchpos++;
    loopnum++;
    srch_resp = search_string(SRCH_CONT, dicfile, srchkey, &respos, &roff, &rlen, repstr);

    if (srch_resp != SRCH_OK)
      break;
    if (oldrespos == respos)
      continue;

    gchar word[4];
    get_word(word, repstr, sizeof(word), 0);
    word[3] = '\0';

    list = g_slist_prepend(list, strdup(word));
  }

  return list;
}

GSList *get_kanji_by_stroke(int stroke, int plusmin, GSList *list, GjitenDicfile *dicfile)
{
  static char srchkey[14];
  int i, lowerlim, upperlim;

  upperlim = stroke + plusmin;
  if (upperlim > 30)
    upperlim = 30;
  lowerlim = stroke - plusmin;
  if (lowerlim < 1)
    lowerlim = 1;

  //find the kanji with the 'S' key (strokes)
  for (i = lowerlim; i <= upperlim; i++)
  {
    snprintf(srchkey, 14, " S%d ", i);
    list = get_kanji_by_key(srchkey, list, dicfile);
  }

  return list;
}

GSList *get_kanji_by_radical(const gchar *radstrg, GHashTable *rad_info_hash)
{
  gint radnum; //number of character in radstrg
  RadInfo *rad_info;
  GSList *kanji_info_list = NULL;
  GSList *result = NULL;    //list of matched kanji to return
  const gchar *radstrg_ptr; //pointer to browse radstrg

  radnum = g_utf8_strlen(radstrg, -1);
  if (radnum == 0)
    return NULL; //no character in radstrg

  //to navigate in the string
  int i;
  radstrg_ptr = radstrg;

  //for every characters in the string
  for (i = 0; i < radnum; i++)
  {
    //get the radical in utf8 format
    gunichar uniradical = g_utf8_get_char(radstrg_ptr);
    gchar radical[UTF8_MIN_SIZE];
    int at = g_unichar_to_utf8(uniradical, radical);
    radical[at] = '\0';

    //lookup the radical (key) and get the radical info (value)
    rad_info = g_hash_table_lookup(rad_info_hash, radical);

    if (rad_info)
    {
      //contains all the kanji of the current radical
      GSList *radical_kanji_list = NULL;

      //add all the kanji from the radical info list to the tmp list
      for (kanji_info_list = rad_info->kanji_info_list;
           kanji_info_list != NULL;
           kanji_info_list = kanji_info_list->next)
      {
        radical_kanji_list = g_slist_prepend(radical_kanji_list,
                                             (gpointer)((KanjiInfo *)kanji_info_list->data)->kanji);
      }

      //if the result list is empty (first iteration)
      if (result == NULL)
      {
        //the result is all the kanji for current radical
        result = radical_kanji_list;
      }
      else
      {
        //the result list is not empty, remove the kanji in result that are not
        //in the radical_kanji_list
        result = list_merge(result, radical_kanji_list);
      }
    }

    radstrg_ptr = g_utf8_next_char(radstrg_ptr);
  }

  return result;
}

GSList *list_merge(GSList *list_a, GSList *list_b)
{
  GSList *ptr1, *ptr2, *nextptr;
  int found;

  if (!list_a)
    return list_b;
  if (!list_b)
    return list_a;

  ptr1 = list_a;
  while (ptr1 != NULL)
  {
    nextptr = g_slist_next(ptr1);
    found = FALSE;
    ptr2 = list_b;
    while (ptr2 != NULL)
    {
      if ((gpointer)(ptr1->data) == (gpointer)(ptr2->data))
      {
        found = TRUE;
        break;
      }
      ptr2 = g_slist_next(ptr2);
    }
    if (found == FALSE)
    {
      list_a = g_slist_remove(list_a, ptr1->data);
    }
    ptr1 = nextptr;
  }
  g_slist_free(list_b);
  list_b = NULL;

  return list_a;
}

GSList *list_merge_str(GSList *list_a, GSList *list_b)
{
  GSList *ptr1, *ptr2, *nextptr;
  int found;

  if (!list_a)
    return list_b;
  if (!list_b)
    return list_a;

  ptr1 = list_a;
  while (ptr1 != NULL)
  {
    nextptr = g_slist_next(ptr1);
    found = FALSE;
    ptr2 = list_b;
    while (ptr2 != NULL)
    {
      if (!strcmp(ptr1->data, ptr2->data))
      {
        found = TRUE;
        break;
      }
      ptr2 = g_slist_next(ptr2);
    }
    if (found == FALSE)
    {
      list_a = g_slist_remove(list_a, ptr1->data);
    }
    ptr1 = nextptr;
  }
  g_slist_free(list_b);
  list_b = NULL;

  return list_a;
}
