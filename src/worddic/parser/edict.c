#include "edict.h"

GjitenDicentry *parse_line(const gchar *p_line)
{
  gchar *line = g_strdup(p_line);
  //new entry to return
  GjitenDicentry *dicentry = g_new0(GjitenDicentry, 1);

  //cut until the first '/', separating definiton,reading in the first chunk and
  //sensees in the second chunk
  gchar *saveptr_chunk = NULL;
  gchar *chunk = (gchar *)strtok_r(line, "/", &saveptr_chunk);

  ////////
  //read sensees (sub sense) in the second chunk (one sub sense per /)
  gchar *gloss_str = (gchar *)strtok_r(NULL, "/", &saveptr_chunk);

  //is it the first parenthese (among all of the sensees)
  gboolean first_parentheses = TRUE;

  //if the last iteration was a GI (detect begining of new sense/new sub sense)
  gboolean start_new_sense = TRUE;

  //pointer to hold a sense before it's added to the entry
  sense *p_sense = NULL;

  do
  {
    if (gloss_str && strcmp(gloss_str, "\n") && strcmp(gloss_str, " "))
    {
      //check if this is an edict2 EntL sequance or a gloss_str
      if (g_str_has_prefix(gloss_str, "EntL"))
      {
        dicentry->ent_seq = g_strdup(gloss_str);
      }
      else
      {
        char *start = gloss_str;
        char *end = gloss_str;
        gboolean in_token = FALSE;
        while (in_token || *gloss_str == '(' || *gloss_str == ' ')
        {

          if (*gloss_str == '(')
          {
            in_token = TRUE;
            start = gloss_str;
          }
          else if (*gloss_str == ')')
          {
            in_token = FALSE;
            end = gloss_str;
          }

          if (start < end && *start)
          {
            *end = 0;
            gchar *GI = start + 1;

            if (!first_parentheses)
            {
              if (start_new_sense)
              {
                //if new sense create a new sense struct
                p_sense = g_new0(sense, 1);
                dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);
              }

              //Gloss_Str' General Informations: one per pair of parentheses
              //add this GI in the sense
              p_sense->general_informations = g_slist_append(p_sense->general_informations,
                                                             g_strdup(GI));
              if (!g_strcmp0(GI, "P"))
              {
                dicentry->priority = TRUE;
              }

              //if a GI is detected next, add it in the same sense
              start_new_sense = FALSE;
            }
            else
            {
              //in edict the general information are per entry
              //dicentry->general_informations = g_slist_prepend(dicentry->general_informations, g_strdup(GI));
              //gchar *saveptr_sense_GI=NULL;
              //gchar *sense_GI = (gchar*)strtok_r(GI, ",", &saveptr_sense_GI);
              //sense_set_GI_flags_from_code(dicentry);
              first_parentheses = FALSE;
            }

            start = gloss_str = end;
          }
          gloss_str++;
        } //end () token

        //in case there was no GI for this sense
        if (!p_sense)
        {
          p_sense = g_new0(sense, 1);
          dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);
        }

        //the rest of the string is the sub sense (gloss_str point at the end
        //of the last pair of parentheses of this sub sense)
        gloss *p_gloss = g_new0(gloss, 1);
        p_gloss->content = g_strdup(gloss_str);
        g_strlcpy(p_gloss->lang, "eng\0", COUNTRY_CODE_SIZE);
        p_sense->gloss = g_slist_prepend(p_sense->gloss,
                                         p_gloss);

        //create a new sense at new GI encounter
        start_new_sense = TRUE;
      } //end if entl or sense
    }   //end if sense sub not empty

    //get part of line after next /
    gloss_str = (gchar *)strtok_r(NULL, "/", &saveptr_chunk);

    //reverse the prepended data
    //p_sense->general_informations = g_slist_reverse(p_sense->general_informations);
    p_sense->gloss = g_slist_reverse(p_sense->gloss);
  } while (gloss_str);

  //reverse the prepended data
  //dicentry->general_informations = g_slist_reverse(dicentry->general_informations);  //moved to sense
  dicentry->sense = g_slist_reverse(dicentry->sense);

  ////////
  //read definitions in the first chunk
  char *saveptr_jap_definition;
  gchar *jap_definitions = (gchar *)strtok_r(chunk, " ", &saveptr_chunk);

  ////////
  //read the japanese reading in the first chunk
  gchar *jap_readings = (gchar *)strtok_r(NULL, " ", &saveptr_chunk);

  //cut jap definitions and jap readings into a list
  //japanese definition
  gchar *jap_definition = (gchar *)strtok_r(jap_definitions, ";", &saveptr_jap_definition);
  do
  {
    if (jap_definition && strcmp(jap_definition, "\n"))
    {
      //optional trailing parentheses with GI for japanese definition
      gchar **jap_definition_GI = g_strsplit(jap_definition, "(", -1);
      dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition,
                                                 g_strdup(jap_definition_GI[0]));
      g_strfreev(jap_definition_GI);
    }
    jap_definition = (gchar *)strtok_r(NULL, ";", &saveptr_jap_definition);
  } while (jap_definition);
  dicentry->jap_definition = g_slist_reverse(dicentry->jap_definition);

  //optional japanese reading
  if (jap_readings)
  {
    //trim the bracets []
    size_t len = strlen(jap_readings);
    memmove(jap_readings, jap_readings + 1, len - 2);
    jap_readings[len - 2] = 0;

    char *saveptr_jap_reading;
    gchar *jap_reading = (gchar *)strtok_r(jap_readings, ";", &saveptr_jap_reading);
    do
    {
      if (jap_reading && strcmp(jap_reading, "\n"))
      {
        //optional trailing parentheses with GI for japanese reading
        gchar **jap_reading__GI = g_strsplit(jap_reading, "(", -1);

        dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading,
                                                g_strdup(jap_reading__GI[0]));
        g_strfreev(jap_reading__GI);
      }
      //next jap reading
      jap_reading = (gchar *)strtok_r(NULL, ";", &saveptr_jap_reading);
    } while (jap_reading);
    dicentry->jap_reading = g_slist_reverse(dicentry->jap_reading);
  }

  g_free(line);
  return dicentry;
}
