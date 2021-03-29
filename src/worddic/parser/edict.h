#ifndef EDICT_H
#define EDICT_H

#ifndef MINGW
#include <string.h>
#else
#include "../../gjitenkai/dicutil.h"
#endif

#include "../dicentry.h"

GjitenDicentry* parse_line(const gchar* p_line);

#endif
