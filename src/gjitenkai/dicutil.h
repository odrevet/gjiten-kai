#ifndef DICUTIL_H
#define DICUTIL_H

#include <glib.h>
#include <glib/gprintf.h>
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dicfile.h"
#include "error.h"

#define UTF8_MIN_SIZE 6

gchar *get_EOL(gchar *ptr, gchar *end_ptr);
int get_word(char *dest, const char *src, int size, int pos);
void to_utf8(gunichar c, char* utf8_c);

gchar *regex_full_to_half(const gchar *str);
gchar *hira_to_kata(const gchar *hirastr);
gchar *kata_to_hira(const gchar *hirastr);
gchar *full_to_half(const gchar *full);
gchar *ideographical_full_stop_to_full_stop(const gchar *c);
gboolean isKanaChar(const gunichar c);
gboolean isKatakanaChar(const gunichar c);
gboolean isHiraganaChar(const gunichar c);
gboolean isKanjiChar(const gunichar c);
gboolean isJPChar(const gunichar c);
gboolean isFullChar(const gunichar c);
gboolean isOtherChar(const gunichar c);

int strg_end_compare(const gchar *strg1, const gchar *strg2);
gboolean is_kanji_only(const gchar *line);
gboolean isHiraganaString(const gchar *strg);
gboolean isKatakanaString(const gchar *strg);
gboolean hasHiraganaString(const gchar *strg);
gboolean hasKatakanaString(const gchar *strg);
gboolean detect_japanese(const gchar *srchstrg);

#ifdef MINGW
char* strtok_r(char *str, const char *delim, char **nextp);
size_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

/**
   Search for filename in dirs and return the path of the first match or `NULL`
   if the file was not found
   If the file is found relative to the current posiiton then the relative path
   will be returned

   The result is dynamically allocated and should be freed
 */
gchar* get_file(const gchar* const* dirs, const gchar* filename);

#endif
