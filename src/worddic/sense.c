#include "sense.h"

void sense_set_GI_flags_from_code(sense *p_sense)
{
  p_sense->GI = GINONE; //set all GI Flags to 0
  GSList *unit = NULL;

  for (unit = p_sense->general_informations;
       unit != NULL;
       unit = unit->next)
  {
    gchar *GI_text_code = (gchar *)unit->data;

    if (!strcmp(GI_text_code, "v1"))
    {
      p_sense->GI = V1;
    }
    else if (!strcmp(GI_text_code, "n"))
    {
      p_sense->GI = NOUN;
    }
    else if (g_str_has_prefix(GI_text_code, "v5"))
    {
      p_sense->GI = V5;
    }
    else if (!strcmp(GI_text_code, "adj-i"))
    {
      p_sense->GI = ADJI;
    }
  }
}

void sense_free(sense *p_sense)
{
  g_slist_free_full(p_sense->gloss, (GDestroyNotify)gloss_free);
  p_sense->gloss = NULL;
  g_slist_free_full(p_sense->general_informations, g_free);
  p_sense->general_informations = NULL;
  g_free(p_sense);
}

void gloss_free(gloss *p_gloss)
{
  g_free(p_gloss->content);
  g_free(p_gloss);
}
