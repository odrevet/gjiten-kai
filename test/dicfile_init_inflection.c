//gcc -g  dicfile_init_inflection.c ../src/worddic/worddic_dicfile.c ../src/worddic/inflection.c  ../src/worddic/dicentry.c ../src/worddic/parser/jmdict.c  ../src/worddic/parser/edict.c ../src/gjitenkai/dicutil.c ../src/worddic/sense.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile_init_inflection

#include <gtk/gtk.h>

#include "../src/worddic/inflection.h"

int main( int argc, char **argv )
{
  //if any command line argument, do not print the inflection list
  gboolean display = TRUE;
  if(argc >= 2)display = FALSE;

  //init inflection engine
  init_inflection();

  if(display){
    //for all the inflections
    GSList *vinfl_list_browser = NULL;
    for(vinfl_list_browser = vinfl_list;
        vinfl_list_browser != NULL;
        vinfl_list_browser = g_slist_next(vinfl_list_browser)){

      struct vinfl_struct * tmp_vinfl_struct = NULL;
      tmp_vinfl_struct = (struct vinfl_struct *) vinfl_list_browser->data;

      g_printf("%s %s -> %s\n",
               tmp_vinfl_struct->type,
               tmp_vinfl_struct->conj,
               tmp_vinfl_struct->infl);
    }
  }
  return 1;
}
