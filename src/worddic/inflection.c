#include "inflection.h"

GSList *init_inflection() {
  GSList *vinfl_list = NULL;
  const gchar * const * dirs = g_get_system_data_dirs();
  gchar *rest = g_strjoin(G_DIR_SEPARATOR_S, PROJECT_NAME, VINFL_FILENAME, NULL);
  gchar* path = get_file(dirs, rest);
  g_free(rest);

  xmlDocPtr doc = xmlParseFile(path);
  xmlNodePtr cur;

  if (doc == NULL ) {
    fprintf(stderr,"Document not parsed successfully. \n");
    return NULL;
  }
  cur = xmlDocGetRootElement(doc);

  if (xmlStrcmp(cur->name, (const xmlChar *) "inflections")) {
    fprintf(stderr,"document of the wrong type, root node != inflections");
    xmlFreeDoc(doc);
    return NULL;
  }

  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry"))){
      struct vinfl_struct *p_vinfl = g_new0 (struct vinfl_struct, 1);
      xmlNodePtr child = cur->xmlChildrenNode;
      while(child){
	if (!xmlStrcmp(child->name, (const xmlChar *)"df")){
	  p_vinfl->infl = (gchar *)xmlNodeGetContent(child);
	}
	else if (!xmlStrcmp(child->name, (const xmlChar *)"conj")){
	  p_vinfl->conj = (gchar *)xmlNodeGetContent(child);
	}
	else if (!xmlStrcmp(child->name, (const xmlChar *)"label")){
	  p_vinfl->itype = atoi((gchar *)xmlGetProp(child, (const xmlChar *)"type"));
	  p_vinfl->type = (gchar *)xmlNodeGetContent(child);
	}


	child = child->next;
      }
      vinfl_list = g_slist_prepend(vinfl_list, p_vinfl);
    }
    cur = cur->next;
  }

  xmlFreeDoc(doc);

  return vinfl_list;
}

GList* search_inflections(GSList *vinfl_list,
			  WorddicDicfile *dicfile,
                          const gchar *srchstrg) {
  //list to return
  GList *results = NULL;

  //remember previous searches to avoid duplicates
  GSList *previous_search = NULL;

  //declare a search expression variable
  search_expression search_expr;
  search_expr.search_criteria_jp = EXACT_MATCH; //inflection needs to be exact
  search_expr.search_criteria_lat = ANY_MATCH;  //latin search is irrelevent

  //for all the inflections
  GSList *vinfl_list_browser = NULL;
  for(vinfl_list_browser = vinfl_list;
      vinfl_list_browser != NULL;
      vinfl_list_browser = g_slist_next(vinfl_list_browser)){

    struct vinfl_struct * tmp_vinfl_struct = NULL;
    tmp_vinfl_struct = (struct vinfl_struct *) vinfl_list_browser->data;

    //if the inflected conjugaison match the end of the string to search
    if(!g_str_has_suffix(srchstrg, tmp_vinfl_struct->conj)){continue;}

    //create a new GString to modify
    GString *deinflected = g_string_new(NULL);

    // create deinflected string with the searched expression
    deinflected = g_string_append(deinflected, srchstrg);

    //replace the inflection by the conjonction
    gint radical_pos = strlen(srchstrg) - strlen(tmp_vinfl_struct->conj);
    deinflected = g_string_truncate (deinflected, radical_pos);
    deinflected = g_string_append(deinflected, tmp_vinfl_struct->infl);

    //check if deinflected was previously searched
    GSList *l=NULL;
    for(l=previous_search;
        l != NULL;
        l = l->next){
      if(!strcmp(l->data, deinflected->str)){
        //free memory
        g_string_free(deinflected, TRUE);
        deinflected = NULL;
        break;
      }
    }

    //deinflected has been freed because the same string existed in a previous
    //search. skip this iteration
    if(!deinflected)continue;

    //comment that explains which inflection was searched
    gchar *comment = g_strdup_printf("%s %s -> %s",
                                     tmp_vinfl_struct->type,
                                     tmp_vinfl_struct->conj,
                                     tmp_vinfl_struct->infl);

    //if the inflection type is from an adj-i, only search for adj-i
    //if not adji-i, assume it's a verbe
    enum sense_GI entry_type;
    if(tmp_vinfl_struct->itype == ADJ_TO_ADVERB ||
       tmp_vinfl_struct->itype == ADJ_PAST ||
       tmp_vinfl_struct->itype == ADJ_NEGATIVE_PAST ||
       tmp_vinfl_struct->itype == ADJ_PAST_KATTA){
      entry_type = ADJI;
    }
    else{
      entry_type = V1 | V5;
    }

    //search in the dictionary
    search_expr.search_text = deinflected->str;
    GList *results_infl = dicfile_search(dicfile,
                                         &search_expr,
                                         comment,
                                         entry_type,
                                         1);

    results = g_list_concat(results, results_infl);

    // add the string to history
    previous_search = g_slist_append(previous_search, deinflected->str);

    //free memory
    g_free(comment);

    // str is still needed in previous_search
    g_string_free(deinflected, FALSE);
  }

  //free history
  g_slist_free_full(previous_search, g_free);

  return results;
}

void free_vinfl(struct vinfl_struct *vinfl){
  g_free(vinfl->conj);
  vinfl->conj = NULL;
  g_free(vinfl->infl);
  vinfl->infl = NULL;
  g_free(vinfl->type);
  vinfl->type = NULL;
  g_free(vinfl);
}
