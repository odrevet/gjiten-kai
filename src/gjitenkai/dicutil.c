#include "dicutil.h"

gchar *get_EOL(gchar *ptr, gchar *end_ptr)
{
  static gchar *tmpptr;
  tmpptr = ptr;
  while (*tmpptr != '\n')
  {
    if (end_ptr == tmpptr)
      return NULL;
    tmpptr++;
  }
  tmpptr++;
  return tmpptr;
}

int get_word(char *dest, const char *src, int size, int pos)
{
  int k, j;

  k = pos;
  while (src[k] == ' ')
    k++;
  if ((int)(strlen(src) - 1) <= k)
    return (0);

  j = 0;
  if (src[k] == '{')
  {
    while ((src[k] != '}') && (j < size))
    {
      dest[j] = src[k];
      j++;
      k++;
    }
  }
  else
    while ((src[k] != ' ') && (j < size))
    {
      dest[j] = src[k];
      j++;
      k++;
    }
  if (j == size)
    dest[size - 1] = 0;
  else
    dest[j] = 0;

  return k;
}

void to_utf8(gunichar c, char *utf8_c)
{
  int at = g_unichar_to_utf8(c, utf8_c);
  utf8_c[at] = '\0';
}

// Compares strg1 with strg2.
// If strg1 == strg3|strg2 then returns TRUE (End of strg1 matches strg2)
int strg_end_compare(const gchar *strg1, const gchar *strg2)
{
  int i = 0;
  int matching = TRUE;
  const gchar *strg1_end, *strg2_end;

  if (strlen(strg1) < strlen(strg2))
    return FALSE;

  strg1_end = strg1 + strlen(strg1);
  strg2_end = strg2 + strlen(strg2);

  for (i = 0; i < g_utf8_strlen(strg2, -1); i++)
  {
    strg1_end = g_utf8_prev_char(strg1_end);
    strg2_end = g_utf8_prev_char(strg2_end);
    if (g_utf8_get_char(strg1_end) != g_utf8_get_char(strg2_end))
      matching = FALSE;
  }
  return matching;
}

gboolean is_kanji_only(const gchar *line)
{
  const gchar *currentchar = line;
  const gchar *line_end;

  currentchar = line;
  line_end = line + strlen(line);

  while (g_unichar_isspace(*currentchar) == FALSE)
  { // find first space
    if (currentchar == line_end)
      break;
    if (isKanjiChar(g_utf8_get_char(currentchar)) == FALSE)
      return FALSE;
    currentchar = g_utf8_next_char(currentchar);
  }

  return TRUE;
}

gboolean isJPChar(const gunichar c)
{
  if (isKanaChar(c) == TRUE)
    return TRUE;
  if (isKanjiChar(c) == TRUE)
    return TRUE;
  if (isOtherChar(c) == TRUE)
    return TRUE;
  return FALSE;
}
gboolean isKanaChar(const gunichar c)
{
  if (isKatakanaChar(c) == TRUE)
    return TRUE;
  if (isHiraganaChar(c) == TRUE)
    return TRUE;
  return FALSE;
}
gboolean isKatakanaChar(const gunichar c)
{
  if ((c >= 0x30A0) && (c <= 0x30FF))
    return TRUE; // Full and half Katakana
  if ((c >= 0xFF65) && (c <= 0xFF9F))
    return TRUE; // Narrow Katakana
  return FALSE;
}
gboolean isHiraganaChar(const gunichar c)
{
  if ((c >= 0x3040) && (c <= 0x309F))
    return TRUE; // Hiragana
  return FALSE;
}
gboolean isKanjiChar(const gunichar c)
{
  if ((c >= 0x3300) && (c <= 0x33FF))
    return TRUE; //cjk compatibility
  if ((c >= 0x3400) && (c <= 0x4DBF))
    return TRUE; //cjk ext A
  if ((c >= 0x4E00) && (c <= 0x9FAF))
    return TRUE; // cjk unified
  if ((c >= 0x20000) && (c <= 0x2A6DF))
    return TRUE; //cjk ext B
  if ((c >= 0x2F800) && (c <= 0x2FA1F))
    return TRUE; //cjk supplement
  return FALSE;
}

gboolean isFullChar(const gunichar c)
{
  if ((c >= 0xFF00) && (c <= 0xFFEF))
    return TRUE;
  return FALSE;
}

gboolean isOtherChar(const gunichar c)
{
  if ((c >= 0x2E80) && (c <= 0x2EFF))
    return TRUE; //cjk radical
  if ((c >= 0x2F00) && (c <= 0x2FDF))
    return TRUE; //cjk kangxi radicals
  if ((c >= 0x2FF0) && (c <= 0x2FFF))
    return TRUE; //ideographic
  if ((c >= 0x3000) && (c <= 0x303F))
    return TRUE; //punctuation
  if ((c >= 0x3200) && (c <= 0x32FF))
    return TRUE; //enclosed letters
  if ((c >= 0xFE30) && (c <= 0xFE4F))
    return TRUE; //compatibility forms
  if ((c >= 0xFF00) && (c <= 0xFF64))
    return TRUE; //compatibility forms2
  if ((c >= 0xFFA0) && (c <= 0xFFEF))
    return TRUE; //compatibility forms3
  return FALSE;
}

//replace fullwidth REGEX characters with halfwidth REGEX characters
//so the user can use REGEX with japanese characters (avoid changing input
//method back and forth constently)
gchar *regex_full_to_half(const gchar *str)
{
  const gchar *strptr = str;
  gchar *half = g_new0(gchar, strlen(str) + 6);
  gchar *halfptr = half;
  gint length = 0;

  while (*strptr != 0)
  {
    gunichar unicodechar = g_utf8_get_char(strptr);
    gchar utf8char[sizeof(gunichar)];
    to_utf8(unicodechar, utf8char);
    if (!g_strcmp0(utf8char, "＋") ||
        !g_strcmp0(utf8char, "？") ||
        !g_strcmp0(utf8char, "＊") ||
        !g_strcmp0(utf8char, "！") ||
        !g_strcmp0(utf8char, "＼") ||
        !g_strcmp0(utf8char, "｛") ||
        !g_strcmp0(utf8char, "｝") ||
        !g_strcmp0(utf8char, "（") ||
        !g_strcmp0(utf8char, "）") ||
        !g_strcmp0(utf8char, "（") ||
        !g_strcmp0(utf8char, "１") ||
        !g_strcmp0(utf8char, "２") ||
        !g_strcmp0(utf8char, "３") ||
        !g_strcmp0(utf8char, "４") ||
        !g_strcmp0(utf8char, "５") ||
        !g_strcmp0(utf8char, "６") ||
        !g_strcmp0(utf8char, "７") ||
        !g_strcmp0(utf8char, "８") ||
        !g_strcmp0(utf8char, "９") ||
        !g_strcmp0(utf8char, "０"))
    {
      gchar *halfchar = full_to_half(utf8char);
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, halfchar, length);
      g_free(halfchar);
    }
    else if (!g_strcmp0(utf8char, "。"))
    {
      gchar *halfchar = ideographical_full_stop_to_full_stop(utf8char);
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, halfchar, length);
      g_free(halfchar);
    }
    else if (!g_strcmp0(utf8char, "、"))
    {
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, ",", length);
    }
    else if (!g_strcmp0(utf8char, "」"))
    {
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, "]", length);
    }
    else if (!g_strcmp0(utf8char, "「"))
    {
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, "[", length);
    }
    else if (!g_strcmp0(utf8char, "＄"))
    {
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, "$", length);
    }
    else if (!g_strcmp0(utf8char, "＾"))
    {
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, "^", length);
    }
    else
    {
      //no modification, copy from strptr
      length = g_utf8_next_char(strptr) - strptr;
      strncat(halfptr, strptr, length);
    }

    halfptr = g_utf8_next_char(halfptr);
    strptr = g_utf8_next_char(strptr);
    if (strptr == NULL)
    {
      break;
    }
  }

  halfptr[length + 1] = '\0';
  return half;
}

/* Convert Hiragana -> Katakana.*/
gchar *hira_to_kata(const gchar *hirastr)
{
  const gchar *hiraptr = hirastr;

  //create a new char with at least the required size for a UTF8 char
  gchar *kata = g_new0(gchar, strlen(hirastr) + UTF8_MIN_SIZE);
  gchar *kataptr = kata;
  gint length = 0;

  while (*hiraptr != 0)
  {
    if (isHiraganaChar(g_utf8_get_char(hiraptr)) == TRUE)
    {
      g_unichar_to_utf8(g_utf8_get_char(hiraptr) + 96, kataptr);
    }
    else
    {
      length = g_utf8_next_char(hiraptr) - hiraptr;
      strncat(kataptr, hiraptr, length);
    }
    kataptr = g_utf8_next_char(kataptr);
    hiraptr = g_utf8_next_char(hiraptr);
    if (hiraptr == NULL)
      break;
  }

  kataptr[length + 1] = 0;
  return kata;
}

/* Convert Katakana to Hiragana*/
gchar *kata_to_hira(const gchar *katastr)
{
  const gchar *kataptr = katastr;
  gchar *hira = g_new0(gchar, strlen(katastr) + UTF8_MIN_SIZE);
  gchar *hiraptr = hira;
  gint length = 0;

  while (*kataptr != 0)
  {
    if (isKatakanaChar(g_utf8_get_char(kataptr)) == TRUE)
    {
      g_unichar_to_utf8(g_utf8_get_char(kataptr) - 96, hiraptr);
    }
    else
    {
      length = g_utf8_next_char(kataptr) - kataptr;
      strncat(hiraptr, kataptr, length);
    }
    hiraptr = g_utf8_next_char(hiraptr);
    kataptr = g_utf8_next_char(kataptr);
    if (kataptr == NULL)
      break;
  }

  hiraptr[length + 1] = 0;
  return hira;
}

gchar *full_to_half(const gchar *full)
{
  gchar *half = g_new0(gchar, UTF8_MIN_SIZE);
  g_unichar_to_utf8(g_utf8_get_char(full) - 0xFEE0, half);
  return half;
}

gchar *ideographical_full_stop_to_full_stop(const gchar *c)
{
  gchar *r = g_new0(gchar, UTF8_MIN_SIZE);
  g_unichar_to_utf8(g_utf8_get_char(c) - 0x2FD4, r);
  return r;
}

gboolean isHiraganaString(const gchar *strg)
{
  const gchar *hiraptr = strg;

  while (*hiraptr != 0)
  {
    if (isHiraganaChar(g_utf8_get_char(hiraptr)) == FALSE)
      return FALSE;
    hiraptr = g_utf8_next_char(hiraptr);
  }
  return TRUE;
}

gboolean isKatakanaString(const gchar *strg)
{
  const gchar *kataptr = strg;

  while (*kataptr != 0)
  {
    if (isKatakanaChar(g_utf8_get_char(kataptr)) == FALSE)
      return FALSE;
    kataptr = g_utf8_next_char(kataptr);
  }
  return TRUE;
}

gboolean hasHiraganaString(const gchar *strg)
{
  const gchar *hiraptr = strg;

  while (*hiraptr != 0)
  {
    if (isHiraganaChar(g_utf8_get_char(hiraptr)) == TRUE)
      return TRUE;
    hiraptr = g_utf8_next_char(hiraptr);
  }
  return FALSE;
}

gboolean hasKatakanaString(const gchar *strg)
{
  const gchar *kataptr = strg;

  while (*kataptr != 0)
  {
    if (isKatakanaChar(g_utf8_get_char(kataptr)) == TRUE)
      return TRUE;
    kataptr = g_utf8_next_char(kataptr);
  }
  return FALSE;
}

gboolean detect_japanese(const gchar *srchstrg)
{
  const gchar *currchar = srchstrg;
  do
  {
    //FIXME: this doesn't detect all Japanese
    if (g_unichar_iswide(g_utf8_get_char(currchar)) == TRUE)
    {
      return TRUE;
    }
  } while ((currchar = g_utf8_find_next_char(currchar, srchstrg + strlen(srchstrg))) != NULL);
  return FALSE;
}

#ifdef MINGW
char *strtok_r(
    char *str,
    const char *delim,
    char **nextp)
{
  char *ret;

  if (str == NULL)
  {
    str = *nextp;
  }

  str += strspn(str, delim);

  if (*str == '\0')
  {
    return NULL;
  }

  ret = str;

  str += strcspn(str, delim);

  if (*str)
  {
    *str++ = '\0';
  }

  *nextp = str;

  return ret;
}

#ifdef BUFSIZ
#undef BUFSIZ
#endif
#define BUFSIZ 1600

size_t getline(char **lineptr, size_t *n, FILE *stream)
{
  if (!*lineptr)
    *lineptr = malloc(BUFSIZ * sizeof(char));
  char *has_more = fgets(*lineptr, BUFSIZ, stream);
  if (!has_more)
    return -1;
  *n = strlen(*lineptr);
  return *n;
}

#endif

gchar *get_file(const gchar *const *dirs, const gchar *rest)
{
  if (g_file_test(rest, G_FILE_TEST_EXISTS))
    return g_strdup(rest);
  gchar *res = NULL;
  for (gint i = 0; dirs[i]; i++)
  {
    res = g_strjoin(G_DIR_SEPARATOR_S, dirs[i], rest, NULL);
    if (g_file_test(res, G_FILE_TEST_EXISTS))
      return res;
    else
      g_free(res);
  }
  return NULL;
}
