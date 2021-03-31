#ifndef UNIT_STYLE_H
#define UNIT_STYLE_H

typedef struct unit_style_t
{
  GtkTextTag *tag;
  GdkRGBA *color;
  const gchar *font;
  const gchar *start;
  const gchar *end;
} unit_style;

#endif
