#ifndef JMDICT_H
#define JMDICT_H

#include <libxml/tree.h>
#include <libxml/parser.h>



#include "../dicentry.h"

/**
   return a GjitenDicEntry from an entry in a JMdict
   cur must point to an entry
*/

GjitenDicentry* parse_entry_jmdict(xmlDocPtr doc, xmlNodePtr cur);

#endif
