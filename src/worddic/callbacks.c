#include <glib.h>
#include <glib/gprintf.h>
#include <libsoup/soup.h>

#include "worddic.h"
#include "worddic_dicfile.h"
#include "preferences.h"
#include "../gjitenkai/dicfile.h"

/*
 * Update the cursor image if the pointer is above a kanji.
 */
G_MODULE_EXPORT gboolean on_search_results_motion_notify_event(GtkWidget *text_view,
                                                               GdkEventMotion *event)
{
  gint x, y;
  GtkTextIter mouse_iter;
  gunichar kanji;
  gint trailing;

  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(text_view),
                                        GTK_TEXT_WINDOW_WIDGET,
                                        event->x, event->y, &x, &y);

  gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(text_view),
                                     &mouse_iter, &trailing,
                                     x, y);

  kanji = gtk_text_iter_get_char(&mouse_iter);

  // Change the cursor if necessary
  if ((isKanjiChar(kanji) == TRUE))
  {
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(text_view),
                                                   GTK_TEXT_WINDOW_TEXT),
                          cursor_selection);
  }
  else
  {
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(text_view),
                                                   GTK_TEXT_WINDOW_TEXT),
                          cursor_default);
  }

  return FALSE;
}

/**
   search entry activate signal callback:
   Search in the dictionaries the entered text in the search entry
   and put the results in the search result textview buffer
*/
G_MODULE_EXPORT void on_search_expression_activate(GtkEntry *entry, worddic *worddic)
{
  //get the expression to search from the search entry
  const gchar *search_entry_text = gtk_entry_get_text(entry);
  if (!strcmp(search_entry_text, ""))
    return;

  //search for the regex string of the text entry in the dictionaries
  worddic_search(search_entry_text, worddic);
}

G_MODULE_EXPORT void on_checkbutton_dark_theme_toggled(GtkCheckButton *check_button,
                                                       worddic *worddic)
{
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton *)check_button);

  GtkSettings *settings = gtk_settings_get_default();

  g_object_set(G_OBJECT(settings),
               "gtk-application-prefer-dark-theme",
               toggled,
               NULL);

  worddic->conf->dark_theme = toggled;
  worddic_conf_save(worddic->settings, worddic->conf, WSE_DARK_THEME);
}

//if available for this version of GTK, display the next page of result when
//the scroll window is scrolled at the bottom
#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
void G_MODULE_EXPORT on_worddic_search_results_edge_reached(GtkScrolledWindow *sw,
                                                            GtkPositionType pos,
                                                            worddic *worddic)
{

  //get the search result text entry to display matches
  GtkTextBuffer *textbuffer_search_results =
      (GtkTextBuffer *)gtk_builder_get_object(worddic->definitions,
                                              "textbuffer_search_results");
  if (pos == GTK_POS_BOTTOM)
  {
    print_entries(textbuffer_search_results, worddic);
  }
}

#endif

/**
   Callback function when the dictionary has been downloaded.
   write to local drive the content to a file.
*/
static void on_dictionary_download_finished_callback(SoupSession *session,
                                                     SoupMessage *msg,
                                                     void *param)
{
  GtkButton *button = GTK_BUTTON(param);
  if (SOUP_STATUS_IS_SUCCESSFUL(msg->status_code))
  {
    //file path where to save the dictionary
    gchar *url = (gchar *)g_object_get_data(G_OBJECT(button), "url");
    gchar *basename = g_path_get_basename(url);
    gchar *destination = g_strdup_printf("%s/%s", download_location, basename);

    //write to a file
    g_file_set_contents(destination,
                        msg->response_body->data,
                        msg->response_body->length,
                        NULL);

    g_free(basename);
    g_free(destination);
  }
}

static void got_chunk(SoupMessage *msg, SoupBuffer *chunk, GtkProgressBar *pbar)
{
  SoupMessageHeaders *response_headers = msg->response_headers;
  gdouble resp_len = soup_message_headers_get_content_length(response_headers);
  gtk_progress_bar_set_fraction(pbar, ((gdouble)msg->response_body->length / (gdouble)resp_len));
}

/**
   The box is the container that hold the clicked button, a progress bar will be created and added
   to the box, so user can monitor downloading progress
 */
G_MODULE_EXPORT void on_button_download_clicked(GtkButton *button, GtkProgressBar *pbar)
{
  if (!session)
    session = soup_session_new();

  if (!strcmp(gtk_button_get_label(button), "Cancel"))
  {
    gtk_button_set_label(button, "Download");
    SoupMessage *msg = (SoupMessage *)g_object_get_data(G_OBJECT(button), "msg");
    soup_session_cancel_message(session, msg, SOUP_STATUS_CANCELLED);
    g_object_set_data(G_OBJECT(button), "msg", NULL);
    gtk_progress_bar_set_fraction(pbar, 0);
    gtk_progress_bar_set_show_text(pbar, FALSE);
  }
  else
  {
    gtk_button_set_label(button, "Cancel");
    gtk_progress_bar_set_show_text(pbar, TRUE);
    gchar *url = (gchar *)g_object_get_data(G_OBJECT(button), "url");
    SoupMessage *msg = soup_message_new("GET", url);
    //assign the msg to the button data so user can eventually cancel
    g_object_set_data(G_OBJECT(button), "msg", msg);
    soup_session_queue_message(session, msg, on_dictionary_download_finished_callback, button);
    g_object_connect(msg, "signal::got-chunk", got_chunk, pbar, NULL);
  }
}

G_MODULE_EXPORT void on_button_welcome_clicked(GtkButton *button, worddic *p_worddic)
{
  GtkDialog *dialog = (GtkDialog *)gtk_builder_get_object(p_worddic->definitions,
                                                          "dialog_welcome");
  gtk_widget_hide(GTK_WIDGET(dialog));
}
