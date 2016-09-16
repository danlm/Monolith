/* Monolith heading class.
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
 * $Id: ml_heading.c,v 1.2 2002/11/23 17:31:01 rich Exp $
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_heading.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations heading_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_heading
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  int level;			/* Heading level (1 - 6). */
  const char *text;		/* Text to be displayed. */
};

static struct ml_widget_property properties[] =
  {
    { name: "text",
      offset: ml_offsetof (struct ml_heading, text),
      type: ML_PROP_STRING },
    { name: "heading.level",
      offset: ml_offsetof (struct ml_heading, level),
      type: ML_PROP_INT },
    { 0 },
  };

ml_heading
new_ml_heading (pool pool, int level, const char *text)
{
  ml_heading w = pmalloc (pool, sizeof *w);

  w->ops = &heading_ops;
  w->pool = pool;
  w->level = level;
  w->text = text;

  return w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_heading w = (ml_heading) vw;

  if (w->text)
    {
      io_fprintf (io, "<h%d class=\"ml_heading\">", w->level);
      ml_plaintext_print (io, w->text);
      io_fprintf (io, "</h%d>", w->level);
    }
}
