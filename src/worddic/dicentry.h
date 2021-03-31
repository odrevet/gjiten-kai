#ifndef DICENTRY_H
#define DICENTRY_H

#include <glib.h>

#include "sense.h"

/**
entry in an dictonary file
in an EDICT file it's a line
in a JMdict file it's an <entry> tag
*/

typedef struct _GjitenDicentry
{
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *sense;          //sense (struct sense)
  gchar *ent_seq;         //EntLnnnnnnnnX
  gboolean priority;
} GjitenDicentry;

void dicentry_free(GjitenDicentry *dicentry);

#endif
