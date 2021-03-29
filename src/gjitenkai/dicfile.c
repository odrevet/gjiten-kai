#include "dicfile.h"

gboolean dicfile_load(GjitenDicfile* dicfile, GjitenDicfile *mmaped_dicfile){
  //if the dictionary is not initialized, init: open a file descriptor
  if (dicfile->status == DICFILE_NOT_INITIALIZED) {
    if (dicfile_init(dicfile) == FALSE) return FALSE;
  }
  if (dicfile->status != DICFILE_OK) return FALSE;

  //if the mapped dictionary is not the requested dictionnary then clear the
  //previously mapped dictionary
  if ((dicfile != mmaped_dicfile) && (mmaped_dicfile != NULL)) {
    dicutil_unload_dic(mmaped_dicfile);
  }

  //if no mapped dictionary, load into memory from the dic's file descriptor
  if (mmaped_dicfile == NULL) {
    mmaped_dicfile = dicfile;
    long unsigned int length;
    g_file_get_contents (dicfile->path, &dicfile->mem, &length, NULL);

    if (dicfile->mem == NULL) gjiten_abort_with_msg("mmap() failed\n");
    mmaped_dicfile = dicfile;
  }

  return TRUE;
}


void dicutil_unload_dic(GjitenDicfile *dicfile) {
  //free mem of previously used dicfile
#ifdef USE_MMAP
  munmap(dicfile->mem, dicfile->size);
#else
  free(dicfile->mem);
#endif

  dicfile->mem = NULL;
}

gboolean dicfile_init(GjitenDicfile *dicfile) {

  if (dicfile->status != DICFILE_OK) {
    dicfile->file = open(dicfile->path, O_RDONLY);

    if (dicfile->file == -1) {
      gjiten_print_error(_("Error opening dictfile:  %s\nCheck your preferences. "),
			 dicfile->path);
      dicfile->status = DICFILE_BAD;
      return FALSE;
    }
    else {
      if (stat(dicfile->path, &dicfile->o_stat) != 0) {
        printf("**ERROR** %s: stat() \n", dicfile->path);
        dicfile->status = DICFILE_BAD;
        return FALSE;
      }
      else {
        dicfile->size = dicfile->o_stat.st_size;
      }
    }
    dicfile->status = DICFILE_OK;
  }
  return TRUE;
}

void dicfile_close(GjitenDicfile *dicfile) {
  if (dicfile->file > 0) {
    close(dicfile->file);
  }
  dicfile->status = DICFILE_NOT_INITIALIZED;
}

gint search_string(gint srchtype, GjitenDicfile *dicfile, const gchar *srchstrg,
                   guintptr *res_index, gint *hit_pos, gint *res_len, gchar *res_str){
  gint search_result;
  gchar *linestart, *lineend;
  gint copySize = 1023;
  static gchar *linsrchptr;

  //if first time this expression is searched
  if (srchtype == SRCH_START) {
    //start the search from the begining
    linsrchptr = dicfile->mem;
  }

 bad_hit:
  search_result = SRCH_FAIL; // assume search fails

  //search next occurance of the string
  linsrchptr = strstr(linsrchptr, srchstrg);

  if (linsrchptr != NULL) {  // if we have a match
    linestart = linsrchptr;

    // find beginning of line
    while ((*linestart != '\n') && (linestart != dicfile->mem)) linestart--;
    if (linestart == dicfile->mem) {
      if ((isKanjiChar(g_utf8_get_char(linestart)) == FALSE) &&
          (isKanaChar(g_utf8_get_char(linestart)) == FALSE))
        {
          linsrchptr++;
          goto bad_hit;
        }
    }

    linestart++;
    lineend = linestart;
    *hit_pos = linsrchptr - linestart;
    while (*lineend != '\n') { // find end of line
      lineend++;
      if (lineend >= dicfile->mem + dicfile->size) {
        printf("weird.\n");
        break;
      }
    }
    linsrchptr++;
    if ((lineend - linestart + 1) < 1023) copySize = lineend - linestart + 1;
    else copySize = 1023;
    strncpy(res_str, linestart, copySize);
    res_str[copySize] = 0;
    *res_index  = (guintptr) linestart;
    search_result = SRCH_OK; // search succeeded
  }

  return search_result;
}
