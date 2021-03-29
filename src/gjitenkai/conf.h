#ifndef CONF_H
#define CONF_H

#include <gio/gio.h>
#include <pango/pango-font.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../config.h"
#include "error.h"

#include "constants.h"
#include "dicfile.h"
#include "dicutil.h"

GSettings* conf_init_handler(const gchar* schema_id);
void conf_close_handler(GSettings* settings);

#endif
