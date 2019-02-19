//gcc -g is_jp.c print_entry.c ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -lz -o is_jp

#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"

int main( int argc, char **argv )
{
  g_printf("Detect if a string is in japanese\n\
Argument: the string to test\n");

  char *search_text = argv[1];
  
  if(detect_japanese(search_text))
    g_printf("'%s' is in Japanese\n", search_text);
  else
    g_printf("'%s' is not in Japanese\n", search_text);
  
  return 1;
}
