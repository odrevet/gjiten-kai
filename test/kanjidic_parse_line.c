//gcc kanjidic_parse_line.c ../src/kanjidic/kanjifile.c ../src/common/dicutil.c ../src/common/dicfile.c ../src/common/error.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddikanjidic/ -o kanjidic_parse_line && ./kanjidic_parse_line
#include <gtk/gtk.h>

#include "../src/kanjidic/kanjifile.h"

#define LINE "媛 4932 U5a9b B38 G8 S12 F1735 J1 N1238 V1244 DK413 L1950 K1828 O1868 MN6516X MP3.0738 IN2047 DM1969 I3e9.4 Q4244.7 Yyuan4 Yyuan2 Wweon エン ひめ {beautiful woman} {princess}"

int main(int argc, char **argv)
{
  gchar *line = g_strdup(LINE);
  g_printf("Parsing %s\n\n", line);

  kanjifile_entry *entry = kanjidic_dicfile_parse_line(line);

  //free memory
  kanjifile_entry_free(entry);

  g_free(line);
}
