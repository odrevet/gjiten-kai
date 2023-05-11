#include "worddic_dicfile.h"

#ifdef MINGW
#include <windows.h>
#include <urlmon.h>
#include <OleAuto.h>
#endif

#ifdef MINGW
//from https://gist.github.com/cthrall/762195
const char *get_content_type(char *path)
{
  int hr = S_OK;

  LPWSTR swContentType = 0;

  FILE *fp = NULL;
  char *sBuffer[256];
  int iBytesRead = 0;
  int iUnicodeLen = 0;

  // Create buffer to hold ANSI translated content-type string.
  char *sANSIContentType = (char *)malloc(1024 * sizeof(char));

  if (!sANSIContentType)
  {
    return NULL;
  }

  memset(sANSIContentType, 0, sizeof(char) * 1024);

  // Read at most 256 bytes from the file.
  fp = fopen(path, "r");

  if (!fp)
  {
    printf("Failed to open file.\n");
    return NULL;
  }

  memset(sBuffer, 0, sizeof(char) * 256);
  iBytesRead = fread(sBuffer, sizeof(char), 256, fp);

  // Try to deduce the content-type from the buffer.
  if ((hr = FindMimeFromData(NULL, NULL, sBuffer,
                             iBytesRead, NULL, 0, &swContentType, 0)))
  {
    printf("Failed to find mime type from buffer.\n");
    return NULL;
  }

  // Convert the content-type to ANSI.
  iUnicodeLen = WideCharToMultiByte(CP_ACP, 0, swContentType,
                                    SysStringLen(swContentType),
                                    sANSIContentType, 1023, NULL, FALSE);
  return sANSIContentType;
}
#else
const char *get_content_type(char *path)
{
  GError *error = NULL;
  GFile *gf = g_file_new_for_path(path);
  GFileInfo *file_info = g_file_query_info(gf,
                                           "standard::*",
                                           0,
                                           NULL,
                                           &error);
  g_object_unref(gf);

  if (!file_info)
  {
    gjiten_print_error(error->message);
    printf("%s\n", error->message);
    return NULL;
  }

  const char *content_type = g_file_info_get_content_type(file_info);
  //g_object_unref(file_info);

  return content_type;
}

#endif

gboolean worddic_dicfile_open_edict(WorddicDicfile *dicfile, FILE *fp)
{
  //first line is informations (date, author, copyright, ...)
  //It will also be used to check encoding
  gchar *informations = NULL;
  size_t len = 0;
  ssize_t read;

  read = getline(&informations, &len, fp);

  //check if utf8, convert it on the fly if this is not the case
  dicfile->utf8 = g_utf8_validate(informations, read, NULL);

  if (!dicfile->utf8)
  {
    dicfile->informations = g_convert(informations, -1, "UTF-8", "EUC-JP",
                                      NULL, NULL, NULL);
    g_free(informations);
  }
  else
  {
    if (!dicfile->informations)
    {
      dicfile->informations = informations;
    }
  }

  // check the dictionary magic number
  const gchar *magic = "　？？？";
  if (!dicfile->informations ||
      !g_str_has_prefix(dicfile->informations, magic))
  {
    g_printf("Invalid EDICT file (wrong magic number) for file %s\n", dicfile->path);
    dicfile->informations = g_strdup("Invalid EDICT file");
    dicfile->type = NULL;
    dicfile->copyright = NULL;
    dicfile->creation_date = NULL;
    dicfile->is_valid = FALSE;
    return FALSE;
  }

  /*gchar **information_v = g_strsplit(dicfile->informations, "/", 4);
    dicfile->type = information_v[1];
    dicfile->copyright = information_v[2];
    dicfile->creation_date = information_v[3];*/

  dicfile->is_valid = TRUE;
  return TRUE;
}

#define CHUNK 0x4000
#define windowBits 15
#define ENABLE_ZLIB_GZIP 32

gboolean worddic_dicfile_open(WorddicDicfile *dicfile)
{
  gchar *path = NULL;
  if (dicfile->path_tmp)
    path = dicfile->path_tmp;
  else
    path = dicfile->path;

  const char *content_type = get_content_type(path);

  if (content_type == NULL)
  {
    printf("Cannot open file\n");
    dicfile->is_valid = FALSE;
    dicfile->loaded = TRUE;
    return FALSE;
  }

  if (!strcmp("application/gzip", content_type) ||
      !strcmp("application/x-gzip-compressed", content_type))
  {
    const char *file_name = path;
    z_stream strm = {0};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    char *id = g_strdup_printf("%d", rand());
    gchar *tmp_dir = g_strdup(g_get_tmp_dir());
    gchar *outpath = g_strjoin(G_DIR_SEPARATOR_S, tmp_dir, id, NULL);
    g_free(id);
    g_free(tmp_dir);
    FILE *file_output = fopen(outpath, "w");

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = in;
    strm.avail_in = 0;

    int status;
    status = inflateInit2(&strm, windowBits | ENABLE_ZLIB_GZIP);
    if (status < 0)
    {
      exit(EXIT_FAILURE);
    }

    FILE *file_input = fopen(file_name, "rb");

    while (1)
    {
      int bytes_read;
      int zlib_status;

      bytes_read = fread(in, sizeof(char), sizeof(in), file_input);

      strm.avail_in = bytes_read;
      strm.next_in = in;
      do
      {
        unsigned have;
        strm.avail_out = CHUNK;
        strm.next_out = out;
        zlib_status = inflate(&strm, Z_NO_FLUSH);
        switch (zlib_status)
        {
        case Z_OK:
        case Z_STREAM_END:
        case Z_BUF_ERROR:
          break;

        default:
          inflateEnd(&strm);
          fprintf(stderr, "Gzip error %d in '%s'.\n",
                  zlib_status, file_name);
          return -1;
        }
        have = CHUNK - strm.avail_out;
        fwrite(out, sizeof(unsigned char), have, file_output);
      } while (strm.avail_out == 0);
      if (feof(file_input))
      {
        inflateEnd(&strm);
        break;
      }
    }
    fclose(file_output);

    //open the inflated tmp file
    dicfile->path_tmp = outpath;
    worddic_dicfile_open(dicfile);
  }
  else
  {
    //uncompressed file
    if (!strcmp("application/xml", content_type) ||
        !strcmp("text/xml", content_type))
    {
      dicfile_parse_xml(dicfile);
    }
    else if (!(strcmp("text/plain", content_type)))
    {
      FILE *fp = fopen(path, "r");
      worddic_dicfile_open_edict(dicfile, fp);
      gboolean has_line = TRUE;
      while (has_line)
      {
        has_line = worddic_dicfile_parse_next_line(dicfile, fp);
      }
      dicfile->entries = g_slist_reverse(dicfile->entries);
      fclose(fp);
    }
    else
    {
      gjiten_print_error("unknwown content type %s\n", content_type);
      g_printf("unknwown content type %s\n", content_type);
    }

    dicfile->loaded = TRUE;
  }

  //the mingw version of get_content_type allocate memory
#ifdef MINGW
  g_free(content_type);
#endif

  return TRUE;
}

gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile, FILE *fp)
{
  size_t len = 0;
  ssize_t read;

  gchar *line = NULL;
  read = getline(&line, &len, fp);

  //if no more characters to read, return false
  if (read == -1)
  {
    g_free(line);
    return FALSE;
  }

  gchar *utf_line = NULL;
  if (!dicfile->utf8)
  {
    //if not utf8 convert the line (assum it's EUC-JP)
    utf_line = g_convert(line, -1, "UTF-8", "EUC-JP", NULL, NULL, NULL);
    g_free(line);
  }
  else
  {
    utf_line = line;
  }

  if (utf_line)
  {
    GjitenDicentry *dicentry = parse_line(utf_line);
    dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
    g_free(utf_line);
  }

  return TRUE;
}

gboolean worddic_dicfile_have_entry(WorddicDicfile *dicfile)
{
  return dicfile->entries == NULL ? FALSE : TRUE;
}

void dicfile_parse_jmdict(WorddicDicfile *dicfile)
{
  gchar *path = NULL;
  if (dicfile->path_tmp)
    path = dicfile->path_tmp;
  else
    path = dicfile->path;

  xmlDocPtr doc = xmlParseFile(path);
  xmlNodePtr cur;

  if (doc == NULL)
  {
    fprintf(stderr, "Document not parsed successfully. \n");
    dicfile->is_valid = FALSE;
    return;
  }
  cur = xmlDocGetRootElement(doc);

  if (xmlStrcmp(cur->name, (const xmlChar *)"JMdict"))
  {
    fprintf(stderr, "Root node is neither JMdict or JMnedict");
    xmlFreeDoc(doc);
    return;
  }

  cur = cur->xmlChildrenNode;
  while (cur != NULL)
  {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry")))
    {
      GjitenDicentry *dicentry = parse_entry_jmdict(doc, cur);
      dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
    }

    cur = cur->next;
  }

  dicfile->is_valid = TRUE;
  xmlFreeDoc(doc);
}

void dicfile_parse_xml(WorddicDicfile *dicfile)
{
  gchar *path = NULL;
  if (dicfile->path_tmp)
    path = dicfile->path_tmp;
  else
    path = dicfile->path;

  xmlDocPtr doc = xmlParseFile(path);
  xmlNodePtr cur;

  if (doc == NULL)
  {
    gjiten_print_error("Document not parsed successfully");
    dicfile->is_valid = FALSE;
    return;
  }
  cur = xmlDocGetRootElement(doc);

  if (!xmlStrcmp(cur->name, (const xmlChar *)"JMdict"))
  {
    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry")))
      {
        GjitenDicentry *dicentry = parse_entry_jmdict(doc, cur);
        dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
      }
      cur = cur->next;
    }
    dicfile->is_valid = TRUE;
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *)"JMnedict"))
  {
    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry")))
      {
        GjitenDicentry *dicentry = parse_entry_jmnedict(doc, cur);
        dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
      }
      cur = cur->next;
    }
    dicfile->is_valid = TRUE;
  }
  else
  {
    gjiten_print_error("Provided dictionary is not a valid JMdict or JMnedict file");
    dicfile->is_valid = FALSE;
  }

  xmlFreeDoc(doc);
}

GList *dicfile_search(WorddicDicfile *dicfile,
                      search_expression *p_search_expression,
                      gchar *comment,
                      enum sense_GI itype,
                      gint is_jp)
{
  //variable from search expression structure
  const gchar *search_text = p_search_expression->search_text;
  enum dicfile_search_criteria search_criteria_jp = p_search_expression->search_criteria_jp;
  enum dicfile_search_criteria search_criteria_lat = p_search_expression->search_criteria_lat;

  //list of matched dictonnary entries
  GList *results = NULL;

  //detect is the search expression is in japanese or latin char
  if (is_jp <= -1)
    is_jp = detect_japanese(search_text);

  /////////////////////////////////////////////////////////////////
  //modify the expression with anchors according to the search criteria
  //create a GString to do so
  GString *entry_string = g_string_new(search_text);
  if (is_jp)
  {
    if (search_criteria_jp == EXACT_MATCH)
    {
      entry_string = g_string_prepend_c(entry_string, '^');
      entry_string = g_string_append_c(entry_string, '$');
    }
    else if (search_criteria_jp == START_WITH_MATCH)
    {
      entry_string = g_string_prepend_c(entry_string, '^');
    }

    else if (search_criteria_jp == END_WITH_MATCH)
    {
      entry_string = g_string_append_c(entry_string, '$');
    }
  }
  else
  {
    if (search_criteria_lat == EXACT_MATCH)
    {
      entry_string = g_string_prepend_c(entry_string, '^');
      entry_string = g_string_append_c(entry_string, '$');
    }
    else if (search_criteria_lat == WORD_MATCH)
    {
      entry_string = g_string_prepend(entry_string, "\\b");
      entry_string = g_string_append(entry_string, "\\b");
    }
  }

  //regex variables
  GError *error = NULL;
  gint start_position = 0;
  gboolean has_matched = FALSE;
  GMatchInfo *match_info = NULL;

  GRegex *regex = g_regex_new(entry_string->str,
                              G_REGEX_OPTIMIZE |
                                  G_REGEX_NO_AUTO_CAPTURE |
                                  G_REGEX_CASELESS,
                              start_position,
                              &error);
  //free memory
  g_string_free(entry_string, TRUE);

  //cannot continue the search if the regex is invalid
  if (!regex)
    return NULL;

  if (is_jp)
  {
    //search matches in the japanese definition or japanese reading
    //if the search expression contains at least a japanese character

    //check if there if the japanese characters are not hiragana or katakana, meaning
    //that there are only kanji except for regex characters. this variable can be used
    //to ignore the reading unit to improve the speed a bit
    gboolean only_kanji = (!hasKatakanaString(search_text) &&
                           !hasHiraganaString(search_text));

    GSList *list_dicentry = NULL;
    for (list_dicentry = dicfile->entries;
         list_dicentry != NULL;
         list_dicentry = list_dicentry->next)
    {

      GjitenDicentry *dicentry = list_dicentry->data;

      has_matched = FALSE;

      //search in the definition
      GSList *jap_definition = dicentry->jap_definition;
      while (jap_definition && !has_matched)
      {
        has_matched = g_regex_match(regex, jap_definition->data, 0, &match_info);

        //if there is a match, copy the entry into the result list
        if (has_matched)
        {
          results = add_match(match_info, comment, dicentry, results);
        }
        else
        {
          jap_definition = jap_definition->next;
        }

        g_match_info_unref(match_info);
      }

      //search in the japanese reading (if any)
      //if no match in the definition and if the search string is not only kanji
      if (!has_matched && dicentry->jap_reading && !only_kanji)
      {
        GSList *jap_reading = dicentry->jap_reading;
        while (jap_reading && !has_matched)
        {
          has_matched = g_regex_match(regex, jap_reading->data, 0, &match_info);

          if (has_matched)
          {
            results = add_match(match_info, comment, dicentry, results);
          }
          else
          {
            jap_reading = jap_reading->next;
          }

          g_match_info_unref(match_info);
        }
      }
    } //for all dictionary entries
  }
  else
  {
    //if there are no japanese characters, search matches in the sensees
    //(latin characters search)
    GSList *list_dicentry = NULL;
    for (list_dicentry = dicfile->entries;
         list_dicentry != NULL;
         list_dicentry = list_dicentry->next)
    {

      has_matched = FALSE;
      GjitenDicentry *dicentry = list_dicentry->data;
      GSList *sense_list = dicentry->sense;
      //search in the sense list
      while (sense_list && !has_matched)
      {
        sense *sense = sense_list->data;

        //check if the type match what we are searching
        //if(!(sense->GI & itype))break;

        GSList *gloss_list = sense->gloss;
        //search in the sub sensees
        while (gloss_list && !has_matched)
        {
          gloss *p_gloss = (gloss *)gloss_list->data;
          //if the gloss lang is set as activated in search option, performe the search
          GSList *p_lang_node = p_search_expression->langs;

          gboolean lang_activated = FALSE;
          while (p_lang_node && lang_activated == FALSE)
          {

            lang *p_lang = p_lang_node->data;
            if (!strcmp(p_gloss->lang, p_lang->code) && p_lang->active)
            {
              lang_activated = TRUE;
              break;
            }
            p_lang_node = p_lang_node->next;
          }

          if (lang_activated)
          {
            has_matched = g_regex_match(regex, p_gloss->content, 0, &match_info);
            if (has_matched)
            {
              results = add_match(match_info, comment, dicentry, results);
            }
            else
            {
              gloss_list = gloss_list->next;
            }

            g_match_info_unref(match_info);
          }
          else
          {
            gloss_list = gloss_list->next;
          }
        }
        sense_list = sense_list->next;
      }
    }
  }

  //free memory
  g_regex_unref(regex);

  return results;
}

void worddic_dicfile_free(WorddicDicfile *dicfile)
{
  g_free(dicfile->name);
  dicfile->name = NULL;

  g_free(dicfile->path);
  dicfile->path = NULL;

  g_free(dicfile->path_tmp);
  dicfile->path_tmp = NULL;

  g_free(dicfile->informations);
  dicfile->informations = NULL;

  g_free(dicfile->type);
  dicfile->informations = NULL;

  g_free(dicfile->copyright);
  dicfile->informations = NULL;

  g_free(dicfile->creation_date);
  dicfile->informations = NULL;

  worddic_dicfile_unload(dicfile);

  g_free(dicfile);
}

void worddic_dicfile_free_entries(WorddicDicfile *dicfile)
{
  g_slist_free_full(dicfile->entries, (GDestroyNotify)dicentry_free);
  dicfile->entries = NULL;
}

void worddic_dicfile_unload(WorddicDicfile *dicfile)
{
  worddic_dicfile_free_entries(dicfile);
  dicfile->loaded = FALSE;

  //if the dicfile has an uncompressed file in the tmp directory
  if (dicfile->path_tmp)
  {
    g_remove(dicfile->path_tmp);
    g_free(dicfile->path_tmp);
    dicfile->path_tmp = NULL;
  }
}
