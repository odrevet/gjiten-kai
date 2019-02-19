#ifndef DICRESULT_H
#define DICRESULT_H

#include <glib.h>

#include "dicentry.h"

/**
   A dicresult represent a match in the dictionary from a search
   as the match is differant from the searched expression when searching with a regex or an inflection.
 */
typedef struct dicresult_t{
  GjitenDicentry *entry;
  gchar *match;     //matched part (for highlighting)
  gchar *comment;   //comment on the result (inflection...)
}dicresult;

void dicresult_free(dicresult* p_dicresult);

#endif
