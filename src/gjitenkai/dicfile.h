#ifndef DICFILE_H
#define DICFILE_H

#include <glib.h>

#include "conf.h"
#include "error.h"
#include <sys/stat.h>

enum dicfie_search_result
{
  SRCH_OK = 0,
  SRCH_FAIL,
  SRCH_START,
  SRCH_CONT
};

enum dicfile_status
{
  DICFILE_NOT_INITIALIZED = 0,
  DICFILE_BAD,
  DICFILE_OK
};

struct _GjitenDicfile
{
  gchar *path;
  gchar *name;
  gchar *mem;
  int file;
  enum dicfile_status status;
  struct stat o_stat;
  gint size;
};

typedef struct _GjitenDicfile GjitenDicfile;

gboolean dicfile_load(GjitenDicfile *dicfile, GjitenDicfile *mmaped_dicfile);
void dicutil_unload_dic(GjitenDicfile *dicfile);

gboolean dicfile_init(GjitenDicfile *dicfile);
void dicfile_close(GjitenDicfile *dicfile);
gint search_string(gint srchtype, GjitenDicfile *dicfile, const gchar *srchstrg, guintptr *res_index, gint *hit_pos, gint *res_len, gchar *res_str);

#endif
