#ifndef KANJI_ITEM_H
#define KANJI_ITEM_H

#include <gtk/gtk.h>
#include "../gjitenkai/constants.h"

/**
   define a property of a kanji (bushuu, stroke count, grade)
 */

typedef struct kanji_item{
  const gchar *name;
  gchar *gsettings_name;
  gshort position;
  gboolean active;
}kanji_item;

extern GSList *kanji_item_list;

void kanji_item_list_init();

kanji_item* kanji_item_load(gchar *name, gchar *gsettings_name);

void kanji_item_save(kanji_item* ki);

#endif
