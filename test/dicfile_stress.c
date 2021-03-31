//gcc -g  dicfile_stress.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -lz -I../src/worddic/ -o dicfile_stress
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"

int main(int argc, char **argv)
{
  gchar *path = argv[1];
  gint max = atoi(argv[2]);
  gint i;
  for (i = 0; i < max; i++)
  {
    WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
    dicfile->path = g_strdup(argv[1]);
    worddic_dicfile_open(dicfile);

    g_printf("**%d**\n%s (len %d)\nUTF8: %d\nGZ: %d\n",
             i,
             dicfile->informations,
             strlen(dicfile->informations),
             dicfile->utf8,
             dicfile->is_gz);

    worddic_dicfile_close(dicfile);
    worddic_dicfile_free(dicfile);
  }
  return 1;
}
