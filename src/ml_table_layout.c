/* Monolith table layout class.
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
 * $Id: ml_table_layout.c,v 1.3 2002/11/13 20:46:38 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_table_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations table_layout_ops =
  {
    repaint: repaint,
    properties: properties
  };

struct cell
{
  ml_widget w;			/* Widget stored in the cell. */
  const char *clazz;		/* Stylesheet class. */
  unsigned char rowspan;	/* Rowspan. */
  unsigned char colspan;	/* Colspan. */
  unsigned char flags;		/* Various flags. */
#define CELL_FLAGS_NO_PAINT  0x01
#define CELL_FLAGS_IS_HEADER 0x02
  char align;			/* l|r|c */
  char valign;			/* t|b|m */
};

struct ml_table_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  int rows, cols;		/* Number of rows, columns. */
  const char *clazz;		/* Stylesheet class for the whole table. */

  /* Cells are stored in this 2D array. */
  struct cell **cells;
};

static struct ml_widget_property properties[] =
  {
    { name: "class",
      offset: ml_offsetof (struct ml_table_layout, clazz),
      type: ML_PROP_STRING },
    { 0 }
  };

static inline struct cell *
get_cell (ml_table_layout w, int row, int col)
{
  assert (0 <= row && row < w->rows);
  assert (0 <= col && col < w->cols);
  return &w->cells[row][col];
}

static inline void
init_cell (ml_table_layout w, int r, int c)
{
  w->cells[r][c].w = 0;
  w->cells[r][c].clazz = 0;
  w->cells[r][c].rowspan = 1;
  w->cells[r][c].colspan = 1;
  w->cells[r][c].flags = 0;
  w->cells[r][c].align = 'l';
  w->cells[r][c].valign = 'm';
}

ml_table_layout
new_ml_table_layout (pool pool, int rows, int cols)
{
  ml_table_layout w = pmalloc (pool, sizeof *w);
  int r, c;

  w->ops = &table_layout_ops;
  w->pool = pool;
  w->rows = rows;
  w->cols = cols;
  w->clazz = 0;

  w->cells = pmalloc (pool, sizeof (struct cell *) * rows);
  for (r = 0; r < rows; ++r)
    {
      w->cells[r] = pmalloc (pool, sizeof (struct cell) * cols);
      for (c = 0; c < cols; ++c)
	init_cell (w, r, c);
    }

  return w;
}

void
ml_table_layout_pack (ml_table_layout w, ml_widget _w, int row, int col)
{
  get_cell (w, row, col)->w = _w;
}

void
ml_table_layout_add_row (ml_table_layout w)
{
  int c;

  w->cells = prealloc (w->pool,
		       w->cells, sizeof (struct cell *) * (w->rows+1));
  w->cells[w->rows] = pmalloc (w->pool, sizeof (struct cell) * w->cols);

  for (c = 0; c < w->cols; ++c)
    init_cell (w, w->rows, c);

  w->rows++;
}

void
ml_table_layout_set_colspan (ml_table_layout w, int row, int col, int colspan)
{
  assert (colspan > 0);
  assert (col + colspan <= w->cols);
  get_cell (w, row, col)->colspan = colspan;
}

void
ml_table_layout_set_rowspan (ml_table_layout w, int row, int col, int rowspan)
{
  assert (rowspan > 0);
  assert (row + rowspan <= w->rows);
  get_cell (w, row, col)->rowspan = rowspan;
}

void
ml_table_layout_set_align (ml_table_layout w, int row, int col,
			   const char *align)
{
  get_cell (w, row, col)->align = align[0];
}

void
ml_table_layout_set_valign (ml_table_layout w, int row, int col,
			    const char *valign)
{
  get_cell (w, row, col)->valign = valign[0];
}

void
ml_table_layout_set_class (ml_table_layout w, int row, int col,
			   const char *clazz)
{
  get_cell (w, row, col)->clazz = clazz;
}

void
ml_table_layout_set_header (ml_table_layout w, int row, int col,
			    int is_header)
{
  if (is_header)
    get_cell (w, row, col)->flags |= CELL_FLAGS_IS_HEADER;
  else
    get_cell (w, row, col)->flags &= ~CELL_FLAGS_IS_HEADER;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_table_layout w = (ml_table_layout) vw;
  int r, c;

  /* Clear all the NO_PAINT flags. */
  for (r = 0; r < w->rows; ++r)
    for (c = 0; c < w->cols; ++c)
      get_cell (w, r, c)->flags &= ~CELL_FLAGS_NO_PAINT;

  /* Start of the table. */
  io_fputs ("<table", io);
  if (w->clazz) io_fprintf (io, " class=\"%s\"", w->clazz);
  io_fputs (">", io);

  /* Paint the cells. */
  for (r = 0; r < w->rows; ++r)
    {
      io_fprintf (io, "<tr>");

      for (c = 0; c < w->cols; ++c)
	{
	  struct cell *cl = get_cell (w, r, c);

	  if (!(cl->flags & CELL_FLAGS_NO_PAINT))
	    {
	      int i, j;

	      /* If there is a row or column span > 1, then we need to mark
	       * the cells in the "shadow" as not painted.
	       */
	      for (i = 0; i < cl->rowspan; ++i)
		for (j = 0; j < cl->colspan; ++j)
		  get_cell (w, r+i, c+j)->flags |= CELL_FLAGS_NO_PAINT;

	      if (!(cl->flags & CELL_FLAGS_IS_HEADER))
		io_fprintf (io, "<td");
	      else
		io_fprintf (io, "<th");
	      if (cl->clazz)
		io_fprintf (io, " class=\"%s\"", cl->clazz);
	      if (cl->rowspan > 1)
		io_fprintf (io, " rowspan=\"%d\"", cl->rowspan);
	      if (cl->colspan > 1)
		io_fprintf (io, " colspan=\"%d\"", cl->colspan);
	      if (cl->align == 'r')
		io_fprintf (io, " align=\"right\"");
	      else if (cl->align == 'c')
		io_fprintf (io, " align=\"center\"");
	      if (cl->valign == 't')
		io_fprintf (io, " valign=\"top\"");
	      else if (cl->valign == 'b')
		io_fprintf (io, " valign=\"bottom\"");
	      io_fprintf (io, ">");
	      if (cl->w)
		ml_widget_repaint (cl->w, session, windowid, io);
	      else
		io_fprintf (io, "&nbsp;");
	      if (!(cl->flags & CELL_FLAGS_IS_HEADER))
		io_fprintf (io, "</td>\n");
	      else
		io_fprintf (io, "</th>\n");
	    }
	}

      io_fprintf (io, "</tr>\n");
    }

  io_fprintf (io, "</table>");
}
