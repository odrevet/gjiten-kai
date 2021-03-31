#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib-object.h>

#include "worddic.h"
#include "conf.h"
#include "../gjitenkai/dicfile.h"

/**
   Identify the Headers of the dictionary list
 */
enum
{
  COL_NAME = 0,
  COL_PATH,
  COL_ACTIVE,
  COL_LOADED
};

/**
   treeview to pass to the thread callback function to update the UI
 */
typedef struct dic_state_ui_t
{
  GtkCellRendererToggle *cell;
  GtkTreeView *treeview;
  WorddicDicfile *dicfile;
  GtkLabel *label_dic_info;
  GtkTreePath *path;
  worddic *worddic;
} dic_state_ui;

/**
   preference window related functions:
   initialisation and callbacks
 */

/**
   Thread call back function for parsing all entries from a dictionary file
 */
gpointer proxy_worddic_dicfile_parse_all(WorddicDicfile *dicfile);

/**
   Initialize controles according to the settings
 */
void init_prefs_window(worddic *worddic);

#endif
