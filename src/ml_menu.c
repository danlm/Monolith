/* Monolith menubar, menu, menuitem classes.
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
 * $Id: ml_menu.c,v 1.2 2003/01/11 16:39:10 rich Exp $
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_menu.h"

static void menubar_repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property menubar_properties[];

struct ml_widget_operations menubar_ops =
  {
    repaint: menubar_repaint,
    properties: menubar_properties,
  };

struct ml_menubar
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  vector menus;			/* Menus contained here. */
  ml_widget w;			/* Widget for main application area. */
};

static struct ml_widget_property menubar_properties[] =
  {
    { 0 }
  };

struct menu
{
  const char *name;
  ml_menu menu;
};

static void menu_repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property menu_properties[];

struct ml_widget_operations menu_ops =
  {
    repaint: menu_repaint,
    properties: menu_properties,
  };

struct ml_menu
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  vector items;			/* Menu items. */
};

static struct ml_widget_property menu_properties[] =
  {
    { 0 }
  };

static void menuitem_button_repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property menuitem_button_properties[];

struct ml_widget_operations menuitem_button_ops =
  {
    repaint: menuitem_button_repaint,
    properties: menuitem_button_properties,
  };

struct ml_menuitem_button
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *text;		/* Text on the button. */
  const char *action_id;	/* Action. */
  const char *title;		/* Tooltip. */
};

static struct ml_widget_property menuitem_button_properties[] =
  {
    { 0 }
  };

static void menuitem_separator_repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property menuitem_separator_properties[];

struct ml_widget_operations menuitem_separator_ops =
  {
    repaint: menuitem_separator_repaint,
    properties: menuitem_separator_properties,
  };

struct ml_menuitem_separator
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
};

static struct ml_widget_property menuitem_separator_properties[] =
  {
    { 0 }
  };

ml_menubar
new_ml_menubar (pool pool)
{
  ml_menubar w = pmalloc (pool, sizeof *w);

  w->ops = &menubar_ops;
  w->pool = pool;
  w->menus = new_vector (pool, struct menu);
  w->w = 0;

  return w;
}

void
ml_menubar_push_back (ml_menubar w, const char *name, ml_menu menu)
{
  struct menu m = { name, menu };

  vector_push_back (w->menus, m);
}

ml_menu
ml_menubar_pop_back (ml_menubar w)
{
  struct menu m;

  vector_pop_back (w->menus, m);
  return m.menu;
}

void
ml_menubar_push_front (ml_menubar w, const char *name, ml_menu menu)
{
  struct menu m = { name, menu };

  vector_push_front (w->menus, m);
}

ml_menu
ml_menubar_pop_front (ml_menubar w)
{
  struct menu m;

  vector_pop_front (w->menus, m);
  return m.menu;
}

ml_menu
ml_menubar_get (ml_menubar w, int i)
{
  struct menu m;

  vector_get (w->menus, i, m);
  return m.menu;
}

void
ml_menubar_insert (ml_menubar w, int i, const char *name, ml_menu menu)
{
  struct menu m = { name, menu };

  vector_insert (w->menus, i, m);
}

void
ml_menubar_replace (ml_menubar w, int i, const char *name, ml_menu menu)
{
  struct menu m = { name, menu };

  vector_replace (w->menus, i, m);
}

void
ml_menubar_erase (ml_menubar w, int i)
{
  vector_erase (w->menus, i);
}

void
ml_menubar_clear (ml_menubar w)
{
  vector_clear (w->menus);
}

int
ml_menubar_size (ml_menubar w)
{
  return vector_size (w->menus);
}

void
ml_menubar_pack (ml_menubar w, ml_widget _w)
{
  w->w = _w;
}

static void
menubar_repaint (void *vw, ml_session session,
		 const char *windowid, io_handle io)
{
  ml_menubar w = (ml_menubar) vw;
  struct menu m;
  int i;

  io_fputs ("<table class=\"ml_menubar\"><tr>", io);

  for (i = 0; i < vector_size (w->menus); ++i)
    {
      vector_get (w->menus, i, m);

      io_fprintf (io, "<td class=\"ml_menubar_item\">%s", m.name);
      menu_repaint (m.menu, session, windowid, io);
      io_fputs ("</td>\n", io);
    }

  io_fputs ("<td width=\"100%\"></td></tr></table>", io);

  if (w->w) ml_widget_repaint (w->w, session, windowid, io);
}

ml_menu
new_ml_menu (pool pool)
{
  ml_menu w = pmalloc (pool, sizeof *w);

  w->ops = &menu_ops;
  w->pool = pool;
  w->items = new_vector (pool, ml_widget);

  return w;
}

void
ml_menu_push_back (ml_menu w, ml_widget menu_or_menuitem)
{
  vector_push_back (w->items, menu_or_menuitem);
}

void
ml_menu_push_back_button (ml_menu w, const char *text, void (*fn) (ml_session, void *), ml_session session, void *data)
{
  ml_menuitem_button b = new_ml_menuitem_button (w->pool, text);

  ml_menuitem_button_set_callback (b, fn, session, data);
  vector_push_back (w->items, b);
}

void
ml_menu_push_back_inactive_button (ml_menu w, const char *text)
{
  ml_menuitem_button b = new_ml_menuitem_button (w->pool, text);

  vector_push_back (w->items, b);
}

void
ml_menu_push_back_separator (ml_menu w)
{
  ml_menuitem_separator b = new_ml_menuitem_separator (w->pool);

  vector_push_back (w->items, b);
}

ml_widget
ml_menu_pop_back (ml_menu w)
{
  ml_widget menu_or_menuitem;

  vector_pop_back (w->items, menu_or_menuitem);
  return menu_or_menuitem;
}

void
ml_menu_push_front (ml_menu w, ml_widget menu_or_menuitem)
{
  vector_push_front (w->items, menu_or_menuitem);
}

ml_widget
ml_menu_pop_front (ml_menu w)
{
  ml_widget menu_or_menuitem;

  vector_pop_front (w->items, menu_or_menuitem);
  return menu_or_menuitem;
}

ml_widget
ml_menu_get (ml_menu w, int i)
{
  ml_widget menu_or_menuitem;

  vector_get (w->items, i, menu_or_menuitem);
  return menu_or_menuitem;
}

void
ml_menu_insert (ml_menu w, int i, ml_widget menu_or_menuitem)
{
  vector_insert (w->items, i, menu_or_menuitem);
}

void
ml_menu_replace (ml_menu w, int i, ml_widget menu_or_menuitem)
{
  vector_replace (w->items, i, menu_or_menuitem);
}

void
ml_menu_erase (ml_menu w, int i)
{
  vector_erase (w->items, i);
}

void
ml_menu_clear (ml_menu w)
{
  vector_clear (w->items);
}

int
ml_menu_size (ml_menu w)
{
  return vector_size (w->items);
}

void
ml_menu_pack (ml_menu w, ml_widget menu_or_menuitem)
{
  vector_push_back (w->items, menu_or_menuitem);
}

static void
menu_repaint (void *vw, ml_session session,
	      const char *windowid, io_handle io)
{
  ml_menu w = (ml_menu) vw;
  int i;
  ml_widget m;

  io_fputs ("<ul>", io);

  for (i = 0; i < vector_size (w->items); ++i)
    {
      vector_get (w->items, i, m);

      io_fputs ("<li>", io);
      ml_widget_repaint (m, session, windowid, io);
      io_fputs ("</li>\n", io);
    }

  io_fputs ("</ul>\n", io);
}

ml_menuitem_button
new_ml_menuitem_button (pool pool, const char *text)
{
  ml_menuitem_button w = pmalloc (pool, sizeof *w);

  w->ops = &menuitem_button_ops;
  w->pool = pool;
  w->text = text;
  w->action_id = 0;
  w->title = 0;

  return w;
}

void
ml_menuitem_button_set_callback (ml_menuitem_button w,
				 void (*fn) (ml_session, void *),
				 ml_session session, void *data)
{
  if (w->action_id)
    ml_unregister_action (session, w->action_id);
  w->action_id = 0;

  if (fn)
    w->action_id = ml_register_action (session, fn, data);
}

static void
menuitem_button_repaint (void *vw, ml_session session,
			 const char *windowid, io_handle io)
{
  ml_menuitem_button w = (ml_menuitem_button) vw;

  if (w->text)
    {
      if (w->action_id)
	{
	  pool tmp = new_subpool (w->pool);

	  const char *link =
	    psprintf (tmp, "%s?ml_action=%s&ml_window=%s",
		      ml_session_script_name (session),
		      w->action_id,
		      windowid);

	  io_fprintf (io, "<a href=\"%s\"", link);

	  if (w->title)
	    {
	      io_fputs (" title=\"", io);
	      ml_plaintext_print (io, w->title);
	      io_fputc ('"', io);
	    }

	  io_fprintf (io, ">%s</a>", w->text);

	  delete_pool (tmp);
	}
      else
	io_fprintf (io, "<span>%s</span>", w->text);
    }
}

ml_menuitem_separator
new_ml_menuitem_separator (pool pool)
{
  ml_menuitem_separator w = pmalloc (pool, sizeof *w);

  w->ops = &menuitem_separator_ops;
  w->pool = pool;

  return w;
}

static void
menuitem_separator_repaint (void *vw, ml_session session,
			    const char *windowid, io_handle io)
{
  io_fputs ("<hr />", io);
}
