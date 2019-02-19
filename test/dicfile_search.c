//gcc -g  dicfile_search.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/worddic/dicresult.c ../src/worddic/parser/jmdict.c  ../src/worddic/parser/edict.c ../src/gjitenkai/dicutil.c ../src/worddic/sense.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile_search
#include <gtk/gtk.h>

#include <worddic_dicfile.h>
#include <dicresult.h>

#include "print_entry.h"

int main( int argc, char **argv )
{
  GList *results = NULL;

  g_printf("load, search and free a dictionary.\nParameters are 'Dicionary path' 'Search Expression'\n");
  g_printf("JAPANESE SEARCH EXPRESSION ONLY\n");

  //create and open dictionary
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(argv[1]);
  worddic_dicfile_open(dicfile);

  search_expression search_expr;
  search_expr.search_criteria_jp = ANY_MATCH;
  search_expr.search_criteria_lat = ANY_MATCH;

  //kana
  //search hiragana on katakana
  if(hasKatakanaString(argv[2])) {
    gchar *hiragana = kata_to_hira(argv[2]);
    search_expr.search_text = hiragana;

    results = g_list_concat(results, dicfile_search(dicfile,
						    &search_expr,
						    "from katakana",
						    GIALL,
						    1)
			    );
    g_free(hiragana);  //free memory
  }
  ///////////////////////////////////
  //search katakana on hiragana
  if (hasHiraganaString(argv[2])) {
    gchar *katakana = hira_to_kata(argv[2]);
    search_expr.search_text = katakana;

    results = g_list_concat(results, dicfile_search(dicfile,
						    &search_expr,
						    "from hiragana",
						    GIALL,
						    1));
    g_free(katakana); //free memory
  }

  ////////////////////////////////
  //standard search
  search_expr.search_text = argv[2];
  results = g_list_concat(results, dicfile_search(dicfile,
						  &search_expr,
						  NULL,
						  GIALL,
						  1));
  //print
  if(results){
    GList *l;
    for(l=results;l!=NULL;l = l->next){
      dicresult *result = l->data;
      g_printf("COMMENT %s\n MATCH %s:\n", result->comment, result->match);
      print_entry(result->entry);
    }
  }
  else printf("No match for %s\n", argv[2]);


  //free dicresult
  g_list_free_full(results, (GDestroyNotify)dicresult_free);

  //free dictionary
  worddic_dicfile_free(dicfile);

  return 1;
}
