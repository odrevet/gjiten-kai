#ifndef WORDDIC_H
#define WORDDIC_H

#include <gtk/gtk.h>

#include <locale.h>
#include <libintl.h>
#include <libsoup/soup.h>

#include "conf.h"
#include "inflection.h"
#include "unit_style.h"
#include "sense.h"
#include "../gjitenkai/conf.h"
#include "../gjitenkai/dicfile.h"

#define SETTINGS_WORDDIC "gjitenkai.worddic"
#define UI_DEFINITIONS_FILE_WORDDIC "worddic.ui"

GdkCursor *cursor_selection;
GdkCursor *cursor_default;

SoupSession *session;
const gchar *download_location;

typedef struct worddic_t{
  GtkBuilder *definitions;
  GSettings *settings;
  struct _WorddicConfig *conf;

  gint match_criteria_lat;
  gint match_criteria_jp;

  GThread *thread_load_dic;

  GList *results;          //dicresuls list to partially display
  gint entries_per_page;   //number of entries to display per 'page'
  gint current_page;       //current page to be displayed

  GSList *vinfl_list;      //list of inflections

}worddic;


extern void init_prefs_window(worddic *worddic);

void worddic_init (worddic * );
void init_search_menu(worddic *);

/**
   Search for the regex expression in active dictionaries entries.
   Will also do special searches (inflection, katakana/hiragana conversions)
   * Load any active and unloaded dictionaries
   * If a thread currently loading dictionary entries, wait until the thread ends.
   @return TRUE is one or more expression matched or FALSE if no match
*/
gboolean worddic_search(const gchar *search_text, worddic *worddic);
void print_unit(GtkTextBuffer *textbuffer,
                gchar *text, unit_style *style);
void print_entries(GtkTextBuffer *textbuffer, worddic *p_worddic);
void highlight_result(GtkTextBuffer *textbuffer,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight,
                      GtkTextIter *iter_from);

#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
extern void on_worddic_search_results_edge_reached(GtkScrolledWindow* sw,
						   GtkPositionType pos,
						   worddic* p_worddic);
#endif

void worddic_free(worddic *p_worddic);

#endif
