/* Monolith text label class.
 * - by Richard W.M. Jones <rich@annexia.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: ml_text_label.c,v 1.6 2002/11/23 17:31:01 rich Exp $
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_text_label.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations text_label_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_text_label
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *text;		/* Text to be displayed. */

  /* Various style information. */
  const char *text_align;
  const char *colour;
  const char *font_weight;
  const char *font_size;
};

static struct ml_widget_property properties[] =
  {
    { name: "text",
      offset: ml_offsetof (struct ml_text_label, text),
      type: ML_PROP_STRING },
    { name: "text.align",
      offset: ml_offsetof (struct ml_text_label, text_align),
      type: ML_PROP_STRING },
    { name: "color",
      offset: ml_offsetof (struct ml_text_label, colour),
      type: ML_PROP_STRING },
    { name: "font.weight",
      offset: ml_offsetof (struct ml_text_label, font_weight),
      type: ML_PROP_STRING },
    { name: "font.size",
      offset: ml_offsetof (struct ml_text_label, font_size),
      type: ML_PROP_STRING },
    { 0 },
  };

ml_text_label
new_ml_text_label (pool pool, const char *text)
{
  ml_text_label w = pmalloc (pool, sizeof *w);

  w->ops = &text_label_ops;
  w->pool = pool;
  w->text = text;

  w->text_align = 0;		/* NULL means default for all these. */
  w->colour = 0;
  w->font_weight = 0;
  w->font_size = 0;

  return w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_text_label w = (ml_text_label) vw;

  if (w->text)
    {
      if (w->text_align || w->colour || w->font_weight || w->font_size)
	{
	  io_fprintf (io, "<span style=\"");
	  if (w->text_align)
	    io_fprintf (io, "text-align: %s;", w->text_align);
	  if (w->colour)
	    io_fprintf (io, "color: %s;", w->colour);
	  if (w->font_weight)
	    io_fprintf (io, "font-weight: %s;", w->font_weight);
	  if (w->font_size)
	    io_fprintf (io, "font-size: %s;", w->font_size);
	  io_fprintf (io, "\">");
	}

      ml_plaintext_print (io, w->text);

      if (w->text_align || w->colour || w->font_weight || w->font_size)
	{
	  io_fprintf (io, "</span>");
	}
    }
}
