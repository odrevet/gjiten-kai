#include "print_entry.h"

void print_entry(GjitenDicentry *entry)
{
  GSList *unit = NULL;

  g_print("Definition\n");
  for (unit = entry->jap_definition;
       unit != NULL;
       unit = unit->next)
  {
    g_printf("\t%s\n", unit->data);
  }

  g_printf("Reading (optional): \n");
  if (!entry->jap_reading)
    g_printf("\t<NO READING>\n");
  for (unit = entry->jap_reading;
       unit != NULL;
       unit = unit->next)
  {
    g_printf("\t%s\n", unit->data);
  }

  /*  g_printf("Entry General Informations: \n");
  for(unit = entry->general_informations;
      unit != NULL;
      unit = unit->next){
    g_printf("\t%s\n", unit->data);
    }*/

  //g_printf("General Information ENUM TYPE :\n\t%d\n", entry->GI);

  for (unit = entry->sense; unit != NULL; unit = unit->next)
  {
    g_printf("SENSE\n");

    sense *p_sense = unit->data;
    g_printf("\tGeneral Informations: \n");
    GSList *GI;
    for (GI = p_sense->general_informations;
         GI != NULL;
         GI = GI->next)
    {
      gchar *text = GI->data;
      g_printf("\t\t%s\n", text);
    }

    g_printf("\tSub sense: \n");
    GSList *p_gloss_list = NULL;
    for (p_gloss_list = p_sense->gloss;
         p_gloss_list != NULL;
         p_gloss_list = p_gloss_list->next)
    {
      gloss *p_gloss = p_gloss_list->data;
      g_printf("\t\t%s", p_gloss->content);
      if (p_gloss->lang[0] != '\0')
      {
        g_printf(" (lang '%s')\n", p_gloss->lang);
      }
      else
      {
        g_printf(" | No lang specified (defaults to eng)\n");
      }
    }
  }
}
