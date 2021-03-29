#include "dicentry.h"

void dicentry_free(GjitenDicentry* dicentry){
  g_slist_free_full(dicentry->jap_definition, g_free);
  dicentry->jap_definition = NULL;

  g_slist_free_full(dicentry->jap_reading, g_free);
  dicentry->jap_reading = NULL;

  g_slist_free_full(dicentry->sense, (GDestroyNotify)sense_free);
  dicentry->sense = NULL;

  g_free(dicentry->ent_seq);
  dicentry->ent_seq = NULL;

  g_free(dicentry);
}
