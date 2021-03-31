//gcc -g parse_line.c ../src/worddic/dicentry.c ../src/worddic/sense.c ../src/worddic/parser/edict.c ./print_entry.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o parse_line
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"
#include "../src/worddic/parser/edict.h"
#include "../src/worddic/sense.h"

#include "./print_entry.h"

#define LINE "１コマ;一コマ;１こま;一こま;一齣;一駒(iK) [ひとコマ(一コマ);ひとこま(一こま,一齣,一駒)] /(n) (1) one scene/one frame/one shot/one exposure/(2) one cell/one panel (comic)/EntL1162000X/"

int main(int argc, char **argv)
{
  gchar *line = g_strdup(LINE);
  g_printf("Parsing %s\n\n", line);

  GjitenDicentry *entry = parse_line(line);

  //check is line was altered
  if (strcmp(line, LINE))
  {
    g_printf("WARNING: the line was modified !\nwas\n%s\nis%s\n\n", LINE, line);
  }

  //free the line
  g_free(line);

  //print the entry
  print_entry(entry);

  //free the entry
  dicentry_free(entry);

  return 1;
}
