/* Monolith box class.
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
 * $Id: ml_box.c,v 1.2 2002/09/14 14:35:58 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_box.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations box_ops =
  {
    repaint: repaint
  };

struct ml_box
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_widget w;			/* Widget packed inside the box. */
};

ml_box
new_ml_box (pool pool)
{
  ml_box w = pmalloc (pool, sizeof *w);

  w->ops = &box_ops;
  w->pool = pool;
  w->w = 0;

  return w;
}

void
ml_box_pack (ml_box w, ml_widget _w)
{
  w->w = _w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_box w = (ml_box) vw;

  io_fprintf (io, "<span class=\"ml_box\">");

  if (w->w)
    ml_widget_repaint (w->w, session, windowid, io);

  io_fprintf (io, "</span>");
}
