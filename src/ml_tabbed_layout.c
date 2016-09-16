/* Monolith tabbed layout class.
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
 * $Id: ml_tabbed_layout.c,v 1.1 2002/11/13 20:46:37 rich Exp $
 */

#include "config.h"

#include <assert.h>

#include <pool.h>
#include <vector.h>

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_tabbed_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations tabbed_layout_ops =
  {
    repaint: repaint
  };

struct ml_tabbed_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  vector tabs;			/* Vector of tabs. */
  vector order;			/* Display order (first = top). */
};

struct tab
{
  const char *name;		/* Name of the tab. */
  ml_widget w;			/* Widget. */
}

ml_tabbed_layout
new_ml_tabbed_layout (pool pool)
{
  ml_tabbed_layout w = pmalloc (pool, sizeof *w);

  w->ops = &tabbed_layout_ops;
  w->pool = pool;
  w->tabs = new_vector (pool, struct tab);
  w->order = new_vector (pool, int);

  return w;
}

static inline void
add_tab_to_order (ml_tabbed_layout w, int index)
{
  vector_push_back (w->order, index);
}

static inline void
del_tab_from_order (ml_tabbed_layout w, int index)
{
  int n, i;

  for (n = 0; n < vector_size (w->order); ++n)
    {
      vector_get (w->order, n, i);
      if (i == index)
	{
	  vector_erase (w->order, n);
	  break;
	}
    }
}

static inline void
increment_order (ml_tabbed_layout w, int first_index)
{
  int n, i;

  for (n = 0; n < vector_size (w->order); ++n)
    {
      vector_get (w->order, n, i);
      if (i >= first_index)
	{
	  i++;
	  vector_replace (w->order, n, i);
	}
    }
}

static inline void
decrement_order (ml_tabbed_layout w, int first_index)
{
  int n, i;

  for (n = 0; n < vector_size (w->order); ++n)
    {
      vector_get (w->order, n, i);
      if (i >= first_index)
	{
	  i++;
	  vector_replace (w->order, n, i);
	}
    }
}

void
ml_tabbed_layout_push_back (ml_tabbed_layout w, const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  vector_push_back (w->tabs, tab);

  add_tab_to_order (w, vector_size (w->tabs) - 1);
}

ml_widget
ml_tabbed_layout_pop_back (ml_tabbed_layout w)
{
  struct tab tab;

  vector_pop_back (w->tabs, tab);
  del_tab_from_order (w, vector_size (w->tabs));
  return tab.w;
}

void
ml_tabbed_layout_push_front (ml_tabbed_layout w,
			     const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  vector_push_front (w->tabs, tab);

  increment_order (w, 0);
  add_tab_to_order (w, 0);
}

ml_widget
ml_tabbed_layout_pop_front (ml_tabbed_layout w)
{
  struct tab tab;

  vector_pop_front (w->tabs, tab);
  del_tab_from_order (w, vector_size (w->tabs));
  decrement_order (w, 1);
  return tab.w;
}

ml_widget
ml_tabbed_layout_get (ml_tabbed_layout w, int i)
{
  struct tab tab;

  vector_get (w->tabs, i, tab);
  return tab.w;
}

void
ml_tabbed_layout_insert (ml_tabbed_layout w, int i,
			 const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  vector_insert (w->tabs, i, tab);

  increment_order (w, i);
  add_tab_to_order (w, i);
}

void
ml_tabbed_layout_replace (ml_tabbed_layout w, int i,
			  const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  vector_replace (w->tabs, i, tab);

  ml_tabbed_layout_send_to_back (w, i);
}

void
ml_tabbed_layout_erase (ml_tabbed_layout w, int i)
{
  vector_erase (w->tabs, i);
  del_tab_from_order (w, i);
  decrement_order (w, i+1);
}

void
ml_tabbed_layout_clear (ml_tabbed_layout w)
{
  vector_clear (w->tabs);
  vector_clear (w->order);
}

int
ml_tabbed_layout_size (ml_tabbed_layout w)
{
  return vector_size (w->tabs);
}

void
ml_tabbed_layout_pack (ml_tabbed_layout w, const char *name, ml_widget _w)
{
  ml_tabbed_layout_push_back (w, name, _w);
}

void
ml_tabbed_layout_bring_to_front (ml_tabbed_layout w, int i)
{
  int i;

  vector_pop_back (w->order, i);
  vector_push_front (w->order, i);
}

void
ml_tabbed_layout_send_to_back (ml_tabbed_layout w, int i)
{
  int i;

  vector_pop_front (w->order, i);
  vector_push_back (w->order, i);
}

int
ml_tabbed_layout_get_front_tab (ml_tabbed_layout w)
{
  int i;

  vector_get (w->order, 0, i);
  return i;
}

/* This function verifies the integrity of the internal structures. We
 * can remove it once the tabbed layout has become a bit more mature.
 */
static inline void
verify_structures (ml_tabbed_layout w)
{
  vector marks;
  unsigned char c = 0;
  int i, n;

  assert (vector_size (w->tabs) == vector_size (w->order));

  marks = new_vector (w->pool, unsigned char);
  vector_fill (marks, c, vector_size (w->tabs));

  for (n = 0; n < vector_size (w->order); ++n)
    {
      vector_get (w->order, n, i);
      vector_get (marks, i, c);
      assert (c == 0);
      c = 1;
      vector_replace (marks, i, c);
    }
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_tabbed_layout w = (ml_tabbed_layout) vw;
  int i;

  verify_structures (w);

  XXX;

  for (i = 0; i < vector_size (w->v); ++i)
    {
      ml_widget _w;

      vector_get (w->v, i, _w);
      ml_widget_repaint (_w, session, windowid, io);
    }
}
