#ifndef GJITENKAI_H
#define GJITENKAI_H

#include <gtk/gtk.h>

#include "../config.h"
#include "../worddic/worddic.h"
#include "../kanjidic/kanjidic.h"


#define UI_DEFINITIONS_FILE_GJITENKAI "gjitenkai.ui"

typedef struct gjitenkai_t
{
  GtkBuilder *definitions;
  worddic   *worddic;
  kanjidic  *kanjidic;

  //display worddic and kanjidic in a pane or notebook
  GtkNotebook *notebook;
  GtkPaned *paned;

} gjitenkai;


void gjitenkai_init (gjitenkai * );
void gjitenkai_menu_history_append(gjitenkai *p_gjitenkai, const gchar* text);

#endif
