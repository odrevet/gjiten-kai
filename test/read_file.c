//gcc getline.c ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -lz -I../src/worddic/ -o getline
#include <gtk/gtk.h>

#include "../src/common/dicutil.h"

int main( int argc, char **argv )
{
  gchar *file_name = argv[1];
  gchar *file_content = read_file(file_name);
  g_printf("content of %s is \n%s\n", file_name, file_content);
  g_free(file_content);
  return 1;
}
