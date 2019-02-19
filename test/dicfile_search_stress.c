//gcc -g dicfile_search_stress.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c ../src/worddic/dicresult.c ./print_entry.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o dicfile_search_stress
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"


#include "print_entry.h"

void results_free(GList *results){
  g_list_free_full(results, (GDestroyNotify)dicresult_free);
  
}

int main( int argc, char **argv )
{
  GList *results = NULL;
  
  g_printf("parameters are dictionary path, times to search and search expression (random search if no search expression)\n\
parameters are NOT checked\n");

  gchar *dicfile_path = argv[1];
  gint times = atoi(argv[2]);

  gchar *search_expression = NULL;

  g_printf("%d\n", argc);
  
  if(argc == 4){
    search_expression = argv[3];
  }
  else{
    search_expression = "e.?t";
  }
  
  g_printf("search %s %d time(s) in %s\n", search_expression, times, dicfile_path);
  
  //create and open dictionary
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(dicfile_path);
  worddic_dicfile_open(dicfile);

  //parse dictionary
  worddic_dicfile_parse_all(dicfile);

  //close dicrionary
  worddic_dicfile_close(dicfile);

  gint i;
  for(i=0;i<=times;i++){
    //kana
    //search hiragana on katakana
    if(hasKatakanaString(search_expression)) {
      gchar *hiragana = kata_to_hira(search_expression);
      results = g_list_concat(results, dicfile_search(dicfile,
                                                      hiragana,
                                                      "from katakana",
                                                      GIALL,
                                                      ANY_MATCH,
                                                      ANY_MATCH,
                                                      1)
                              );
      g_free(hiragana);  //free memory
    }
    ///////////////////////////////////

  
    //search kata
    if(hasHiraganaString(search_expression)) { 
      gchar *katakana = hira_to_kata(search_expression);
      results = g_list_concat(results, dicfile_search(dicfile,
                                                      katakana,
                                                      "from hiragana",
                                                      GIALL,
                                                      ANY_MATCH,
                                                      ANY_MATCH,
                                                      1)
                              );
      g_free(katakana); //free memory
    }
    ////////////////////////////////

    //comment
    gchar *comment = g_strdup_printf ("Search #%d", i);
    
    //search 
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    search_expression,
                                                    comment,
                                                    GIALL,
                                                    ANY_MATCH,
                                                    ANY_MATCH,
                                                    -1)
                            );
    g_free(comment);
    
    //print
    GList *l;
    for(l=results;l!=NULL;l = l->next){
      dicresult *result = l->data;
    
      //print matched part and comment
      g_printf("(%s) %s:\n", result->comment, result->match);
      //print the entry
      print_entry(result->entry);
    }
  
    //free dicresult
    //when freeing a dicreslt, do not free the dicentry as it will be used again
    //at the next search, just free the comment and the match
    results_free(results);
    results = NULL;
  }
  //free dictionary
  worddic_dicfile_free(dicfile);
  
  return 1;
}
