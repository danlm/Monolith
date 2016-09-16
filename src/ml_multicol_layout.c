/* Monolith multi-column layout class.
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
 * $Id: ml_multicol_layout.c,v 1.3 2002/11/13 20:46:37 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_table_layout.h"
#include "ml_multicol_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations multicol_layout_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_multicol_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_table_layout tbl;		/* We're really just a table layout. */
  int cols;			/* Fixed number of columns. */
  int rows;			/* Current number of rows in the table. */
  int r, c;			/* Next row, column. */
  const char *clazz;		/* Stylesheet class. */
};

static void update_table_class (void *vw);

static struct ml_widget_property properties[] =
  {
    { name: "class",
      offset: ml_offsetof (struct ml_multicol_layout, clazz),
      type: ML_PROP_STRING,
      on_set: update_table_class },
    { 0 }
  };

static void make_cell (ml_multicol_layout w);

ml_multicol_layout
new_ml_multicol_layout (pool pool, int nr_cols)
{
  ml_multicol_layout w = pmalloc (pool, sizeof *w);

  w->ops = &multicol_layout_ops;
  w->pool = pool;
  w->tbl = new_ml_table_layout (pool, 0, nr_cols);
  w->cols = nr_cols;
  w->rows = 0;
  w->r = w->c = 0;
  w->clazz = 0;

  return w;
}

static void
update_table_class (void *vw)
{
  ml_multicol_layout w = (ml_multicol_layout) vw;

  ml_widget_set_property (w->tbl, "class", w->clazz);
}

void
ml_multicol_layout_set_align (ml_multicol_layout w, const char *align)
{
  make_cell (w);
  ml_table_layout_set_align (w->tbl, w->r, w->c, align);
}

void
ml_multicol_layout_set_valign (ml_multicol_layout w, const char *valign)
{
  make_cell (w);
  ml_table_layout_set_valign (w->tbl, w->r, w->c, valign);
}

void
ml_multicol_layout_set_class (ml_multicol_layout w, const char *clazz)
{
  make_cell (w);
  ml_table_layout_set_class (w->tbl, w->r, w->c, clazz);
}

void
ml_multicol_layout_set_header (ml_multicol_layout w, int is_header)
{
  make_cell (w);
  ml_table_layout_set_header (w->tbl, w->r, w->c, is_header);
}

void
ml_multicol_layout_pack (ml_multicol_layout w, ml_widget _w)
{
  make_cell (w);

  /* Insert the widget. */
  if (_w) ml_table_layout_pack (w->tbl, _w, w->r, w->c);

  /* Move to the next position. */
  w->c++;
  if (w->c >= w->cols)
    {
      w->c = 0;
      w->r++;
    }
}

static void
make_cell (ml_multicol_layout w)
{
  /* Create a new row in the underlying table? */
  if (w->r >= w->rows)
    {
      ml_table_layout_add_row (w->tbl);
      w->rows++;
    }
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_multicol_layout w = (ml_multicol_layout) vw;

  ml_widget_repaint (w->tbl, session, windowid, io);
}
