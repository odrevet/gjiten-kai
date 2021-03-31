//gcc parse_line_stress.c ../src/worddic/dicentry.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o parse_line_stress
//gcc parse_line_stress.c ../src/worddic/dicentry.c ../src/worddic/gloss.c ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -DMINGW -o parse_line_stress.exe
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"
#include "../src/worddic/gloss.h"

#define LINE "１コマ;一コマ;１こま;一こま;一齣;一駒(iK) [ひとコマ(一コマ);ひとこま(一こま,一齣,一駒)] /(n) (1) one scene/one frame/one shot/one exposure/(2) one cell/one panel (comic)/EntL1162000X/"

#define LIMIT 100000
int main(int argc, char **argv)
{
  gint i = 0;
  gint max = atoi(argv[1]);
  if (max > LIMIT)
  {
    g_printf("Limited to %d times\n", LIMIT);
    max = LIMIT;
  }

  gchar *line = g_strdup(LINE);

  for (; i < max; i++)
  {

    //allocate memory and parse the line
    GjitenDicentry *dicentry = parse_line(line);

    //free memory
    dicentry_free(dicentry);
    dicentry = NULL;
  }

  g_free(line);

  return 1;
}
