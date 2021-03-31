#ifndef INFLECTION_H
#define INFLECTION_H

#include <glib.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "worddic_dicfile.h"
#include "dicentry.h"

#include "../gjitenkai/constants.h"
#include "../gjitenkai/dicfile.h"
#include "../gjitenkai/dicutil.h"

#define VINFL_FILENAME "vconj.xml"

//map the vconj types
enum conj_type
{
  PLAIN_NEGATIVE_NONPAST = 0,
  POLITE_NONPAST,
  CONDITIONAL,
  VOLITIONAL,
  TEFORM,
  PLAIN_PAST,
  PLAIN_NEGATIVE_PAST,
  PASSIVE,
  CAUSATIVE,
  POTENTIAL_OR_IMPERATIVE,
  IMPERATIVE,
  POLITE_PAST,
  POLITE_NEGATIVE_NONPAST,
  POLITE_NEGATIVE_PAST,
  POLITE_VOLITIONAL_UNUSED,
  ADJ_TO_ADVERB,
  ADJ_PAST,
  POLITE,
  POLITE_VOLITIONAL,
  PASSIVE_OR_POTENTIAL,
  PASSIVE_OR_POTENTIAL_GRP_2,
  ADJ_NEGATIVE,
  ADJ_NEGATIVE_PAST,
  ADJ_PAST_KATTA,
  PLAIN_VERB,
  POLITE_TEFORM
};

struct vinfl_struct
{
  gchar *conj;
  gchar *infl;
  gchar *type;
  enum conj_type itype;
};

GSList *init_inflection();

GList *search_inflections(GSList *vinfl_list,
                          WorddicDicfile *dicfile,
                          const gchar *srchstrg);
void free_vinfl(struct vinfl_struct *vinfl);

#endif
