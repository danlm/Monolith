/* Monolith select layout class.
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
 * $Id: ml_select_layout.c,v 1.3 2002/12/02 10:27:18 rich Exp $
 */

#include "config.h"

#include <assert.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <vector.h>

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_smarttext.h"
#include "ml_select_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations select_layout_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_select_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_session session;		/* Current session. */
  vector tabs;			/* Vector of tabs. */
  int selected;			/* Index of selected tab. */
  const char *clazz;		/* Class of top-level table. */
  ml_widget top;		/* "Top" widget (see manpage). */
  ml_widget bottom;		/* "Bottom" widget (see manpage). */
};

struct tab
{
  const char *name;		/* Name of the tab. */
  ml_widget w;			/* Widget. */

  /* Do not set this directly. Use make_action and del_action functions. */
  const char *action_id;
};

static struct ml_widget_property properties[] =
  {
    { name: "class",
      offset: ml_offsetof (struct ml_select_layout, clazz),
      type: ML_PROP_STRING },
    { name: "select_layout.top",
      offset: ml_offsetof (struct ml_select_layout, top),
      type: ML_PROP_WIDGET },
    { name: "select_layout.bottom",
      offset: ml_offsetof (struct ml_select_layout, bottom),
      type: ML_PROP_WIDGET },
    { 0 }
  };

ml_select_layout
new_ml_select_layout (pool pool, ml_session session)
{
  ml_select_layout w = pmalloc (pool, sizeof *w);

  w->ops = &select_layout_ops;
  w->pool = pool;
  w->session = session;
  w->tabs = new_vector (pool, struct tab);
  w->selected = 0;
  w->clazz = 0;
  w->top = w->bottom = 0;

  return w;
}

static void make_action (ml_select_layout w, struct tab *tab);
static void del_action (ml_select_layout w, struct tab *tab);

void
ml_select_layout_push_back (ml_select_layout w, const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  make_action (w, &tab);
  vector_push_back (w->tabs, tab);
}

ml_widget
ml_select_layout_pop_back (ml_select_layout w)
{
  struct tab tab;

  vector_pop_back (w->tabs, tab);
  del_action (w, &tab);
  return tab.w;
}

void
ml_select_layout_push_front (ml_select_layout w,
			     const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  make_action (w, &tab);
  vector_push_front (w->tabs, tab);
}

ml_widget
ml_select_layout_pop_front (ml_select_layout w)
{
  struct tab tab;

  vector_pop_front (w->tabs, tab);
  del_action (w, &tab);
  return tab.w;
}

ml_widget
ml_select_layout_get (ml_select_layout w, int i)
{
  struct tab tab;

  vector_get (w->tabs, i, tab);
  return tab.w;
}

void
ml_select_layout_insert (ml_select_layout w, int i,
			 const char *name, ml_widget _w)
{
  struct tab tab;

  tab.name = name;
  tab.w = _w;
  make_action (w, &tab);
  vector_insert (w->tabs, i, tab);
}

void
ml_select_layout_replace (ml_select_layout w, int i,
			  const char *name, ml_widget _w)
{
  struct tab tab;

  vector_get (w->tabs, i, tab);
  del_action (w, &tab);

  tab.name = name;
  tab.w = _w;
  make_action (w, &tab);
  vector_replace (w->tabs, i, tab);
}

void
ml_select_layout_erase (ml_select_layout w, int i)
{
  struct tab tab;

  vector_get (w->tabs, i, tab);
  del_action (w, &tab);

  vector_erase (w->tabs, i);
}

void
ml_select_layout_clear (ml_select_layout w)
{
  int i;

  for (i = 0; i < vector_size (w->tabs); ++i)
    {
      struct tab tab;

      vector_get (w->tabs, i, tab);
      del_action (w, &tab);
    }

  vector_clear (w->tabs);
}

int
ml_select_layout_size (ml_select_layout w)
{
  return vector_size (w->tabs);
}

void
ml_select_layout_pack (ml_select_layout w, const char *name, ml_widget _w)
{
  ml_select_layout_push_back (w, name, _w);
}

void
ml_select_layout_set_selection (ml_select_layout w, int i)
{
  w->selected = i;
}

int
ml_select_layout_get_selection (ml_select_layout w)
{
  return w->selected;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_select_layout w = (ml_select_layout) vw;
  int i;
  struct tab *tab;
  const char *clazz = w->clazz ? : "ml_select_layout";

  /* Verify the selection is in range. If not, bring it into range. */
  if (w->selected < 0) w->selected = 0;
  else if (w->selected >= vector_size (w->tabs))
    w->selected = vector_size (w->tabs) - 1;

  /* Begin the table. */
  io_fprintf (io, "<table class=\"%s\"><tr><td valign=\"top\">", clazz);

  /* Left hand column. */
  if (w->top) ml_widget_repaint (w->top, session, windowid, io);

  io_fputs ("<table class=\"ml_select_layout_left\">", io);

  for (i = 0; i < vector_size (w->tabs); ++i)
    {
      vector_get_ptr (w->tabs, i, tab);

      io_fputs ("<tr>", io);
      if (i == w->selected) io_fputs ("<th>", io);
      else io_fputs ("<td>", io);

      io_fprintf (io, "<a href=\"%s?ml_action=%s&ml_window=%s\">",
		  ml_session_script_name (w->session),
		  tab->action_id, windowid);
      ml_plaintext_print (io, tab->name);
      io_fputs ("</a>", io);

      if (i == w->selected) io_fputs ("</th>", io);
      else io_fputs ("</td>", io);
      io_fputs ("</tr>\n", io);
    }

  io_fputs ("</table>", io);

  if (w->bottom) ml_widget_repaint (w->bottom, session, windowid, io);

  io_fputs ("</td>\n<td valign=\"top\">", io);

  /* Right hand column: the widget. */
  vector_get_ptr (w->tabs, w->selected, tab);

  if (tab->w) ml_widget_repaint (tab->w, session, windowid, io);

  io_fputs ("</td></tr></table>", io);
}

static void do_select (ml_session session, void *vargs);

struct select_args
{
  ml_select_layout w;		/* The widget. */
  const char *action_id;	/* The action ID. */
};

static void
make_action (ml_select_layout w, struct tab *tab)
{
  struct select_args *args = pmalloc (w->pool, sizeof *args);

  args->w = w;
  args->action_id = tab->action_id =
    ml_register_action (w->session, do_select, args);
}

static void
del_action (ml_select_layout w, struct tab *tab)
{
  ml_unregister_action (w->session, tab->action_id);
}

/* This function is called when the user clicks on one of the items on the
 * left hand side.
 *
 * Because items can move around, we didn't store the absolute item number
 * in the struct select_args. Instead, we stored the action_id, which we'll
 * use to search through the current tabs to find out which tab the
 * user was trying to click.
 */
static void
do_select (ml_session session, void *vargs)
{
  struct select_args *args = (struct select_args *) vargs;
  const char *action_id = args->action_id;
  ml_select_layout w = args->w;
  int i;
  struct tab *tab;

  for (i = 0; i < vector_size (w->tabs); ++i)
    {
      vector_get_ptr (w->tabs, i, tab);

      if (strcmp (tab->action_id, action_id) == 0)
	{
	  /* Found it. */
	  w->selected = i;
	  return;
	}
    }

  /* Don't worry if we don't find it. Perhaps the underlying code has
   * deleted this item, but the browser window was not updated.
   */
}
