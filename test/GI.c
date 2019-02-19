//gcc -g  GI.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -lz -I../src/worddic/ -o GI
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"

int main( int argc, char **argv )
{
  enum entry_GI itype;

  itype = ADJI;
  g_printf("ADJI: %d vs GIALL %d : ", itype, GIALL);
  if(itype & GIALL)g_printf("OK\n");
  else g_printf("NOTOK\n");

  itype = NOUN;  
  g_printf("NOUN: %d vs GIALL %d : ", itype, GIALL);
  if(itype & GIALL)g_printf("OK\n");
  else g_printf("NOTOK\n");
  
  itype = V1;
  g_printf("V1: %d vs GIALL %d : ", itype, GIALL);
  if(itype & GIALL)g_printf("OK\n");
  else g_printf("NOTOK\n");

  itype = V5;
  g_printf("V5: %d vs GIALL %d : ", itype, GIALL);
  if(itype & GIALL)g_printf("OK\n");
  else g_printf("NOTOK\n");
  
  return 1;
}
