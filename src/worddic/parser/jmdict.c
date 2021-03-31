#include "jmdict.h"

GjitenDicentry *parse_entry_jmdict(xmlDocPtr doc, xmlNodePtr cur)
{
	GjitenDicentry *dicentry = g_new0(GjitenDicentry, 1);
	cur = cur->xmlChildrenNode;

	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"sense")))
		{
			xmlNodePtr child = cur->xmlChildrenNode;
			sense *p_sense = g_new0(sense, 1);
			dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);

			while (child)
			{
				if ((!xmlStrcmp(child->name, (const xmlChar *)"gloss")))
				{
					gchar *lang = (gchar *)xmlGetProp(child, (const xmlChar *)"lang");
					gloss *p_gloss = g_new0(gloss, 1);
					if (lang)
					{
						g_strlcpy(p_gloss->lang, lang, COUNTRY_CODE_SIZE);
						xmlFree(lang);
					}
					else
					{
						g_strlcpy(p_gloss->lang, "eng\0", COUNTRY_CODE_SIZE);
					}

					gchar *content = (gchar *)xmlNodeGetContent(child);
					p_gloss->content = content;

					p_sense->gloss = g_slist_prepend(p_sense->gloss, p_gloss);
				}
				else if ((!xmlStrcmp(child->name, (const xmlChar *)"pos")))
				{
					//get the content without entity subtitution
					gchar *raw = (gchar *)xmlNodeListGetRawString(doc, child->xmlChildrenNode, 0);
					//remove first and last character of the Entity macro
					size_t len = strlen(raw);
					memmove(raw, raw + 1, len - 2);
					raw[len - 2] = 0;
					p_sense->general_informations = g_slist_prepend(p_sense->general_informations, raw);
				}
				child = child->next;
			}
			sense_set_GI_flags_from_code(p_sense);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"k_ele")))
		{
			xmlNodePtr child = cur->xmlChildrenNode;

			while (child)
			{
				if ((!xmlStrcmp(child->name, (const xmlChar *)"keb")))
				{
					gchar *content = (gchar *)xmlNodeGetContent(child);
					dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition, content);
				}
				child = child->next;
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"r_ele")))
		{
			xmlNodePtr child = cur->xmlChildrenNode;

			while (child)
			{
				if ((!xmlStrcmp(child->name, (const xmlChar *)"reb")))
				{
					gchar *content = (gchar *)xmlNodeGetContent(child);
					dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading, content);
				}
				child = child->next;
			}
		}

		cur = cur->next;
	}

	return dicentry;
}
