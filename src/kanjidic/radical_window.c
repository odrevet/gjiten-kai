#include "radical_window.h"

void radical_list_init(kanjidic *kanjidic)
{
  button_list = NULL;

  //populate the radical list window
  GtkGrid *grid_radical_list = (GtkGrid *)gtk_builder_get_object(kanjidic->definitions,
                                                                 "grid_radical");

  GSList *radical_list = kanjidic->rad_info_list;
  radical_list = g_slist_reverse(radical_list);

  gint i = 0;
  gint j = 0;

  gint last_strockes_count = 0;

  for (;
       radical_list != NULL;
       radical_list = g_slist_next(radical_list))
  {

    const gchar *radical = (const gchar *)((RadInfo *)radical_list->data)->radical;
    gint strokes_count = ((RadInfo *)radical_list->data)->strokes;

    //the stroke count has change: display a label with the new count and
    //update the last stroke count
    if (last_strockes_count != strokes_count)
    {
      gchar *str_stroke;
      str_stroke = g_strdup_printf("<span font_weight='bold' fgcolor='#EE0101'>%d</span>",
                                   strokes_count);

      GtkWidget *label_stroke_count = gtk_label_new("");
      gtk_label_set_markup(GTK_LABEL(label_stroke_count), str_stroke);
      gtk_grid_attach(GTK_GRID(grid_radical_list), GTK_WIDGET(label_stroke_count), i, j, 1, 1);

      g_free(str_stroke);

      last_strockes_count = strokes_count;

      i++;
      if (i % RADICAL_PER_ROW == 0)
      {
        j++;
        i = 0;
      }
    }

    //add the button
    GtkButton *button_radical = (GtkButton *)gtk_button_new_with_label(radical);

    g_signal_connect(button_radical,
                     "clicked",
                     G_CALLBACK(on_radical_button_clicked),
                     kanjidic);
    gtk_grid_attach(GTK_GRID(grid_radical_list), GTK_WIDGET(button_radical), i, j, 1, 1);

    //add this button in the button list
    button_list = g_slist_append(button_list, button_radical);

    i++;
    if (i % RADICAL_PER_ROW == 0)
    {
      j++;
      i = 0;
    }
  }
}

void radical_buttons_update(kanjidic *kanjidic)
{
  //get the radicals in the radical filter entry
  GtkEntry *entry_filter_radical = (GtkEntry *)gtk_builder_get_object(kanjidic->definitions,
                                                                      "entry_filter_radical");
  //text in the radical entry
  const gchar *radicals = gtk_entry_get_text(entry_filter_radical);

  //point to the head of the button list
  GSList *l = button_list;

  //if no radicals, set all buttons sensitivity to true
  if (!strcmp(radicals, ""))
  {
    for (;
         l != NULL;
         l = g_slist_next(l))
    {
      gtk_widget_set_sensitive(GTK_WIDGET(l->data), TRUE);
      gtk_widget_set_name(l->data, "radical");
    }
  }
  else
  {
    //for every radicals, ckeck if there at least a match with the current
    //entered radicals and the kanji button
    do
    {
      //get the current button and it's kanji
      GtkButton *button = (GtkButton *)l->data;
      const gchar *cur_radical = gtk_button_get_label(button);
      gtk_widget_set_name(GTK_WIDGET(button), "radical");

      //append the current radical to the filter entry radicals text
      gchar *srch = g_new0(gchar, strlen(radicals) + strlen(cur_radical) + 1);
      g_strlcpy(srch, radicals, strlen(radicals) + strlen(cur_radical));
      strcat(srch, cur_radical);

      //if no match, set the sensitivity to false on this button
      gboolean sensitivity;

      //tooltip text to display
      GString *kanji_match = g_string_new(NULL);

      //get the kanji list for the entered radical and the radical of the button
      GSList *kanji_match_list = get_kanji_by_radical(srch,
                                                      kanjidic->rad_info_hash);

      //set radical button class to active if in the radical search entry
      const gchar *kptr = radicals;
      gunichar radical_in_searchentry;
      gunichar radical_clicked = g_utf8_get_char(cur_radical);
      while ((radical_in_searchentry = g_utf8_get_char(kptr)))
      {
        if (radical_clicked == radical_in_searchentry)
        {
          gtk_widget_set_name(GTK_WIDGET(button), "radical_active");
          break;
        }
        kptr = g_utf8_next_char(kptr);
      }

      if (kanji_match_list == NULL)
      {
        sensitivity = FALSE;
      }
      else
      {
        GSList *kanji_list_browser = NULL;

        sensitivity = TRUE;

        //list of matching kanji in a  tooltip
        for (kanji_list_browser = kanji_match_list;
             kanji_list_browser != NULL;
             kanji_list_browser = kanji_list_browser->next)
        {
          kanji_match = g_string_append(kanji_match,
                                        (gchar *)kanji_list_browser->data);
        }
      }

      if (strlen(kanji_match->str) > 0)
      {
        //set the tootlip with the matching radical list
        char *str_tooltip_markup = g_strdup_printf("<span size='xx-large'>%s</span>",
                                                   kanji_match->str);
        gtk_widget_set_tooltip_markup(GTK_WIDGET(button),
                                      str_tooltip_markup);
        g_free(str_tooltip_markup);
      }
      else
      {
        gtk_widget_set_has_tooltip(GTK_WIDGET(button), FALSE);
      }

      //set the sensitivity
      gtk_widget_set_sensitive(GTK_WIDGET(button), sensitivity);

      //free memory
      g_string_free(kanji_match, TRUE);
      g_free(srch);

    } while ((l = g_slist_next(l)));
  }
}
