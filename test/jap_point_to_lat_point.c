//gcc jap_point_to_lat_point.c -I../src/common/ ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -o jap_point_to_lat_point
#include <gtk/gtk.h>

#include "../src/common/dicutil.h"

int main(int argc, char **argv)
{
  gchar *jp = "。";
  gchar *lat = ideographical_full_stop_2_full_stop(jp);
  g_printf("%s -> %s\n", jp, lat);
  if (!g_strcmp0(jp, lat))
  {
    g_printf("warning: unmodified result\n");
  }
  g_free(lat);
  return 1;
}
