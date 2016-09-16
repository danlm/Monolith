/* Monolith vertical layout class.
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
 * $Id: ml_vertical_layout.c,v 1.2 2003/01/12 22:12:42 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_vertical_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations vertical_layout_ops =
  {
    repaint: repaint,
    properties: properties
  };

struct ml_vertical_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *clazz;		/* Stylesheet class. */
  vector v;			/* Vector of widgets. */
};

static struct ml_widget_property properties[] =
  {
    { name: "class",
      offset: ml_offsetof (struct ml_vertical_layout, clazz),
      type: ML_PROP_STRING },
    { 0 }
  };

ml_vertical_layout
new_ml_vertical_layout (pool pool)
{
  ml_vertical_layout w = pmalloc (pool, sizeof *w);

  w->ops = &vertical_layout_ops;
  w->pool = pool;
  w->clazz = 0;
  w->v = new_vector (pool, ml_widget);

  return w;
}

void
ml_vertical_layout_push_back (ml_vertical_layout w, ml_widget _w)
{
  vector_push_back (w->v, _w);
}

void
ml_vertical_layout_pack (ml_vertical_layout w, ml_widget _w)
{
  vector_push_back (w->v, _w);
}

ml_widget
ml_vertical_layout_pop_back (ml_vertical_layout w)
{
  ml_widget _w;

  vector_pop_back (w->v, _w);
  return _w;
}

void
ml_vertical_layout_push_front (ml_vertical_layout w, ml_widget _w)
{
  vector_push_front (w->v, _w);
}

ml_widget
ml_vertical_layout_pop_front (ml_vertical_layout w)
{
  ml_widget _w;

  vector_pop_front (w->v, _w);
  return _w;
}

ml_widget
ml_vertical_layout_get (ml_vertical_layout w, int i)
{
  ml_widget _w;

  vector_get (w->v, i, _w);
  return _w;
}

void
ml_vertical_layout_insert (ml_vertical_layout w, int i, ml_widget _w)
{
  vector_insert (w->v, i, _w);
}

void
ml_vertical_layout_replace (ml_vertical_layout w, int i, ml_widget _w)
{
  vector_replace (w->v, i, _w);
}

void
ml_vertical_layout_erase (ml_vertical_layout w, int i)
{
  vector_erase (w->v, i);
}

void
ml_vertical_layout_clear (ml_vertical_layout w)
{
  vector_clear (w->v);
}

int
ml_vertical_layout_size (ml_vertical_layout w)
{
  return vector_size (w->v);
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_vertical_layout w = (ml_vertical_layout) vw;
  int i;

  for (i = 0; i < vector_size (w->v); ++i)
    {
      ml_widget _w;

      vector_get (w->v, i, _w);
      io_fputs ("<div", io);
      if (w->clazz) io_fprintf (io, " class=\"%s\"", w->clazz);
      io_fputc ('>', io);
      ml_widget_repaint (_w, session, windowid, io);
      io_fputs ("</div>\n", io);
    }
}
