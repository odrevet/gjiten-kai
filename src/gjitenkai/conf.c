#include "conf.h"

static void settings_changed(GSettings *settings, gchar *key, gpointer user_data)
{
}

GSettings *conf_init_handler(const gchar *schema_id)
{
  GSettings *settings = NULL;

  if (settings == NULL)
  {
    settings = g_settings_new(schema_id);
  }

  //set a dummy signal because of a bug in MSYS2's glib package
  //see https://sourceforge.net/p/msys2/tickets/187/
  g_signal_connect(settings,
                   "changed",
                   (GCallback)settings_changed,
                   NULL);

  return settings;
}

void conf_close_handler(GSettings *settings)
{
  if (settings != NULL)
  {
    g_object_unref(G_OBJECT(settings));
    settings = NULL;
  }
}
