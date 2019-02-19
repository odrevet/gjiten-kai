//gcc getline.c ../src/worddic/dicentry.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o getline
//gcc getline.c ../src/worddic/dicentry.c ../src/worddic/gloss.c ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -DMINGW -o getline.exe
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"
#include "../src/worddic/gloss.h"

int main( int argc, char **argv )
{
  FILE *fp;
  fp = fopen(argv[1], "r");

  gchar *line = NULL;
  size_t len = 0;
  ssize_t read;

  do{
    read = getline(&line, &len, fp);
    g_printf("-->%s<--", line);
  }while(read != -1);
  
  g_free(line);
  
  return 1;
}
