//gcc full2half.c -I../src/common/ ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -o full2half
#include <gtk/gtk.h>

#include "../src/common/dicutil.h"

int main(int argc, char **argv)
{
  //gchar * full = "？";
  //gchar * full = "！";
  //gchar * full = "）";
  //gchar * full = "｛";
  gchar *full = "＋";

  //test if full is a full char (supposed to be true)
  if (isFullChar(g_utf8_get_char(full)))
  {
    g_printf("OK: %s is a full char\n", full);
  }
  else
  {
    g_printf("NOK: %s is NOT a full char\n", full);
  }

  gchar *half = full_to_half(full);
  g_printf("%s -> %s\n", full, half);
  if (!g_strcmp0(full, half))
  {
    g_printf("warning: unmodified result\n");
  }

  //test if half is a full char (supposed to be false)
  if (isFullChar(g_utf8_get_char(half)))
  {
    g_printf("NOK: %s is a full char\n", half);
  }
  else
  {
    g_printf("OK: %s is NOT a full char\n", half);
  }

  g_free(half);
  return 1;
}
