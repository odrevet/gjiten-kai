#include "dicresult.h"

void dicresult_free(dicresult *p_dicresult)
{
  g_free(p_dicresult->match);
  p_dicresult->match = NULL;
  g_free(p_dicresult->comment);
  p_dicresult->comment = NULL;
  //do not free the entry content
  //because it points to the dictionary entry
  p_dicresult->entry = NULL;
}
