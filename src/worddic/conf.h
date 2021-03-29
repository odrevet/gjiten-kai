#ifndef WORDDIC_CONF_H
#define WORDDIC_CONF_H

#include <glib.h>
#include <gio/gio.h>
#include <pango/pango-font.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../config.h"

#include "worddic.h"
#include "worddic_dicfile.h"
#include "unit_style.h"

#include "../gjitenkai/error.h"
#include "../gjitenkai/conf.h"
#include "../gjitenkai/constants.h"
#include "../gjitenkai/dicfile.h"
#include "../gjitenkai/dicutil.h"

/**
   When saving the configuration
   limit what to save or save all with WSE_ALL
 */
typedef enum worddic_save_enum{
  WSE_HIGHLIGHT_COLOR     = 1 << 0,
  WSE_SEARCH_OPTION       = 1 << 1,
  WSE_DICFILE             = 1 << 2,
  WSE_JAPANESE_DEFINITION = 1 << 3,
  WSE_JAPANESE_READING    = 1 << 4,
  WSE_SENSE               = 1 << 5,
  WSE_NOTES               = 1 << 6,
  WSE_HISTORY             = 1 << 7,
  WSE_DARK_THEME          = 1 << 8,
  WSE_LANG                = 1 << 9,
  WSE_ALL                 = 11111111
}worddic_save;

typedef struct _WorddicConfig {
  gchar *version;
  GSList *dicfile_list;
  guint maxwordmatches;

  //font
  //default
  gchar *resultsfont;

  //highlight
  GtkTextTag *highlight;
  GdkRGBA *results_highlight_color;

  ////japanese definition
  unit_style jap_def;
  unit_style jap_reading;
  unit_style sense;
  unit_style gloss;
  unit_style notes;

  //dark theme
  gboolean dark_theme;

  //search options
  gboolean search_kata_on_hira;
  gboolean search_hira_on_kata;
  gboolean verb_deinflection;
  gboolean record_history;

  //search history is a list of search_expression
  GSList *history;

  //List of lang, to be listed in the preference pane.
  //If active, search gloss with the lang code
  GSList *langs;
}WorddicConfig;

void worddic_conf_load_unit_style(GSettings *settings,
                                  unit_style *us,
                                  const gchar *name);
WorddicConfig *worddic_conf_load(GSettings *settings);
void worddic_conf_save(GSettings *settings,
                       WorddicConfig *conf,
                       worddic_save fields);

#endif
