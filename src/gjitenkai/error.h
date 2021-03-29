#ifndef ERROR_H
#define ERROR_H

#include <gtk/gtk.h>

int gjiten_print_error(const char *fmt, ... );
void gjiten_print_error_and_wait(const char *fmt, ... );
gboolean gjiten_print_question(const char *fmt, ... );
void gjiten_abort_with_msg(const char *fmt, ... );

#endif
