#include "conf.h"

void worddic_conf_load_unit_style(GSettings *settings,
                                  unit_style *us,
                                  const gchar *name){
  gchar *key = NULL;

  key = g_strconcat(name, "-start", NULL);
  us->start = g_settings_get_string(settings, key);

  g_free(key);key=NULL;

  key = g_strconcat(name, "-end", NULL);
  us->end = g_settings_get_string(settings, key);
  g_free(key);key=NULL;

  key = g_strconcat(name, "-font", NULL);
  us->font = g_settings_get_string(settings, key);
  g_free(key);key=NULL;

  key = g_strconcat(name, "-color", NULL);
  gchar *color_str = g_settings_get_string(settings, key);
  g_free(key);key=NULL;

  us->color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(us->color, color_str);
}

void worddic_conf_save_unit_style(GSettings *settings,
                                  unit_style *us,
                                  const gchar *name){
  gchar *key = NULL;

  key = g_strconcat(name, "-start", NULL);
  g_settings_set_string(settings, key, us->start);
  g_free(key);key=NULL;

  key = g_strconcat(name, "-end", NULL);
  g_settings_set_string(settings, key, us->end);
  g_free(key);key=NULL;

  key = g_strconcat(name, "-font", NULL);
  g_settings_set_string(settings, key, us->font);
  g_free(key);key=NULL;

  key = g_strconcat(name, "-color", NULL);
  char *str_gloss_color = gdk_rgba_to_string(us->color);
  g_settings_set_string(settings, key, str_gloss_color);
  g_free(str_gloss_color);
  g_free(key);key=NULL;
}


WorddicConfig *worddic_conf_load(GSettings *settings){
  WorddicConfig *conf;

  conf = g_new0(WorddicConfig, 1);

  //load the font familly for the results
  conf->resultsfont = g_settings_get_string(settings, "resultsfont");

  //load the results highlight color from string
  char *str_results_highlight_color = g_settings_get_string(settings, "results-highlight-color");

  //load the dictionary entries units styles
  worddic_conf_load_unit_style(settings, &conf->jap_def, "japanese-definition");
  worddic_conf_load_unit_style(settings, &conf->jap_reading, "japanese-reading");
  worddic_conf_load_unit_style(settings, &conf->sense, "sense");
  worddic_conf_load_unit_style(settings, &conf->gloss, "gloss");
  worddic_conf_load_unit_style(settings, &conf->notes, "notes");


  //dark theme
  conf->dark_theme = g_settings_get_boolean(settings, "dark-theme");

  //highlight color
  conf->results_highlight_color = g_new0(GdkRGBA, 1);
  gdk_rgba_parse(conf->results_highlight_color, str_results_highlight_color);

  //load the dictionaries
  GVariantIter iter;
  GVariant *dictionaries;

  dictionaries = g_settings_get_value(settings, "dictionaries");
  g_variant_iter_init(&iter, dictionaries);

  gchar *dicpath, *dicname;
  gboolean dicactive;

  while (g_variant_iter_next (&iter, "(&s&sb)", &dicpath, &dicname, &dicactive)) {
    if (dicpath != NULL) {
      WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
      dicfile->path = g_strdup(dicpath);
      dicfile->name = g_strdup(dicname);
      dicfile->is_active = dicactive;

      //add the dictionary to the list
      conf->dicfile_list = g_slist_append(conf->dicfile_list, dicfile);
    }
  }
  g_variant_unref(dictionaries);


  //country code and name
  GVariant *lang_variant = g_settings_get_value(settings, "lang");
  g_variant_iter_init(&iter, lang_variant);

  gchar *lang_code, *lang_name;
  gboolean lang_active;
  conf->langs = NULL;

  while (g_variant_iter_next (&iter, "(&s&sb)", &lang_name, &lang_code, &lang_active)) {
    lang *p_lang = g_new0(lang, 1);
    g_strlcpy(p_lang->code, lang_code, COUNTRY_CODE_SIZE);
    p_lang->name = strdup(lang_name);
    p_lang->active = lang_active;
    conf->langs = g_slist_append(conf->langs, p_lang);
  }
  g_variant_unref(lang_variant);


  //load the search options
  conf->search_kata_on_hira = g_settings_get_boolean(settings, "search-kata-on-hira");
  conf->search_hira_on_kata = g_settings_get_boolean(settings, "search-hira-on-kata");
  conf->verb_deinflection   = g_settings_get_boolean(settings, "deinflection-enabled");
  conf->record_history      = g_settings_get_boolean(settings, "record-history");

  //load the history
  GVariantIter history_iter;
  GVariant *history_variant;
  history_variant = g_settings_get_value(settings, "history");
  g_variant_iter_init(&history_iter, history_variant);

  gchar *searched_expression = NULL;

  while (g_variant_iter_next (&history_iter, "(&s)", &searched_expression)) {
    //add to the history list
    conf->history = g_slist_append(conf->history, g_strdup(searched_expression));
  }
  g_variant_unref(history_variant);

  return conf;
}

void worddic_conf_save(GSettings *settings,
                       WorddicConfig *conf,
                       worddic_save fields){
  if(fields & WSE_HISTORY){
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(s)"));
    GSList *l=NULL;
    for(l=conf->history;l!=NULL;l = l->next){
      gchar *searched_expression = (gchar*)l->data;
      g_variant_builder_add(&builder,
                            "(s)",
                            searched_expression);
    }
    g_settings_set_value(settings, "history", g_variant_builder_end(&builder));
  }

  if(fields & WSE_HIGHLIGHT_COLOR){
    //save the result highlight color as a string
    char *str_results_highlight_color = gdk_rgba_to_string(conf->results_highlight_color);
    g_settings_set_string(settings, "results-highlight-color",
                          str_results_highlight_color);
  }

  if(fields & WSE_SEARCH_OPTION){
    g_settings_set_boolean(settings, "search-kata-on-hira",
                           conf->search_kata_on_hira);
    g_settings_set_boolean(settings, "search-hira-on-kata",
                           conf->search_hira_on_kata);
    g_settings_set_boolean(settings, "deinflection-enabled",
                           conf->verb_deinflection);
    g_settings_set_boolean(settings, "record-history",
			   conf->record_history);
  }

  if(fields & WSE_DICFILE){
    GSList *diclist = NULL;
    WorddicDicfile *dicfile = NULL;

    //Save dicfiles [path and name seperated with linebreak]
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ssb)"));
    diclist = conf->dicfile_list;
    while (diclist != NULL) {
      if (diclist->data == NULL) break;
      dicfile = diclist->data;
      g_variant_builder_add(&builder,
                            "(ssb)",
                            dicfile->path,
                            dicfile->name,
                            dicfile->is_active);
      diclist = g_slist_next(diclist);
    }
    g_settings_set_value(settings, "dictionaries", g_variant_builder_end(&builder));
  }

  if(fields & WSE_JAPANESE_DEFINITION){
    worddic_conf_save_unit_style(settings, &conf->jap_def, "japanese-definition");
  }

  if(fields & WSE_JAPANESE_READING){
    worddic_conf_save_unit_style(settings, &conf->jap_reading, "japanese-reading");
  }

  if(fields & WSE_SENSE){
    worddic_conf_save_unit_style(settings, &conf->sense, "sense");
    worddic_conf_save_unit_style(settings, &conf->gloss, "gloss");
  }

  if(fields & WSE_NOTES){
    worddic_conf_save_unit_style(settings, &conf->notes, "notes");
  }

  if(fields & WSE_DARK_THEME){
    g_settings_set_boolean(settings, "dark-theme", conf->dark_theme);
  }

  if(fields & WSE_LANG){
    GSList *lang_list = conf->langs;
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a(ssb)"));

    while (lang_list != NULL) {
      if (lang_list->data == NULL) break;
      lang *p_lang = lang_list->data;
      g_variant_builder_add(&builder,
                            "(ssb)",
                            p_lang->name,
                            p_lang->code,
                            p_lang->active);
      lang_list = g_slist_next(lang_list);
    }
    g_settings_set_value(settings, "lang", g_variant_builder_end(&builder));
    }
}
