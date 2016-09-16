/* Monolith flow layout class.
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
 * $Id: ml_flow_layout.c,v 1.1 2002/08/28 19:50:28 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_flow_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations flow_layout_ops =
  {
    repaint: repaint
  };

struct ml_flow_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  vector v;			/* Vector of widgets. */
};

ml_flow_layout
new_ml_flow_layout (pool pool)
{
  ml_flow_layout w = pmalloc (pool, sizeof *w);

  w->ops = &flow_layout_ops;
  w->pool = pool;
  w->v = new_vector (pool, ml_widget);

  return w;
}

void
ml_flow_layout_push_back (ml_flow_layout w, ml_widget _w)
{
  vector_push_back (w->v, _w);
}

void
ml_flow_layout_pack (ml_flow_layout w, ml_widget _w)
{
  vector_push_back (w->v, _w);
}

ml_widget
ml_flow_layout_pop_back (ml_flow_layout w)
{
  ml_widget _w;

  vector_pop_back (w->v, _w);
  return _w;
}

void
ml_flow_layout_push_front (ml_flow_layout w, ml_widget _w)
{
  vector_push_front (w->v, _w);
}

ml_widget
ml_flow_layout_pop_front (ml_flow_layout w)
{
  ml_widget _w;

  vector_pop_front (w->v, _w);
  return _w;
}

ml_widget
ml_flow_layout_get (ml_flow_layout w, int i)
{
  ml_widget _w;

  vector_get (w->v, i, _w);
  return _w;
}

void
ml_flow_layout_insert (ml_flow_layout w, int i, ml_widget _w)
{
  vector_insert (w->v, i, _w);
}

void
ml_flow_layout_replace (ml_flow_layout w, int i, ml_widget _w)
{
  vector_replace (w->v, i, _w);
}

void
ml_flow_layout_erase (ml_flow_layout w, int i)
{
  vector_erase (w->v, i);
}

void
ml_flow_layout_clear (ml_flow_layout w)
{
  vector_clear (w->v);
}

int
ml_flow_layout_size (ml_flow_layout w)
{
  return vector_size (w->v);
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_flow_layout w = (ml_flow_layout) vw;
  int i;

  for (i = 0; i < vector_size (w->v); ++i)
    {
      ml_widget _w;

      vector_get (w->v, i, _w);
      ml_widget_repaint (_w, session, windowid, io);
    }
}
