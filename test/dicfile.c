//gcc -g  dicfile.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/worddic/parser/jmdict.c  ../src/worddic/parser/edict.c ../src/gjitenkai/dicutil.c ../src/worddic/sense.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile

#include <gtk/gtk.h>

#include "print_entry.h"
#include "../src/worddic/worddic_dicfile.h"
#include "../src/worddic/dicentry.h"

int main(int argc, char **argv)
{
  g_printf("load and free a dictionary.\nParameters is 'Dicionary path'\n");

  if (argc < 1)
  {
    printf("Missing first argument: dictionary path\n");
    return 0;
  }

  char *path = argv[1];
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(path);
  worddic_dicfile_open(dicfile);
  worddic_dicfile_free(dicfile);
  return 1;
}
