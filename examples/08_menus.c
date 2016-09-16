/* Menus.
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
 * $Id: 08_menus.c,v 1.1 2002/11/30 15:55:46 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_menu.h"
#include "ml_text_label.h"

/*----- The following standard boilerplate code must appear -----*/

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/*----- End of standard boilerplate code -----*/

/* Private per-session data. */
struct data
{
  ml_window win;		/* Top-level window. */
  ml_menubar menubar;		/* Menubar along top of window. */
  ml_text_label text;		/* Label displayed in the main window. */
};

static void set_red (ml_session session, void *vdata);
static void set_green (ml_session session, void *vdata);
static void set_blue (ml_session session, void *vdata);
static void set_black (ml_session session, void *vdata);
static void set_small (ml_session session, void *vdata);
static void set_medium (ml_session session, void *vdata);
static void set_large (ml_session session, void *vdata);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;
  ml_menu menu;

  /* Create the private, per-session data area and save it in the
   * session object.
   */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  data->win = new_ml_window (session, pool);

  /* Create the menubar. */
  data->menubar = new_ml_menubar (pool);

  /* Create the "Colours" menu. */
  menu = new_ml_menu (pool);
  ml_menu_push_back_button (menu, "Red", set_red, session, data);
  ml_menu_push_back_button (menu, "Green", set_green, session, data);
  ml_menu_push_back_button (menu, "Blue", set_blue, session, data);
  ml_menu_push_back_button (menu, "Black", set_black, session, data);

  /* Pack the "Colours" menu. */
  ml_menubar_push_back (data->menubar, "Colours", menu);

  /* Create the "Sizes" menu. */
  menu = new_ml_menu (pool);
  ml_menu_push_back_button (menu, "Small", set_small, session, data);
  ml_menu_push_back_button (menu, "Medium", set_medium, session, data);
  ml_menu_push_back_button (menu, "Large", set_large, session, data);

  /* Pack the "Sizes" menu. */
  ml_menubar_push_back (data->menubar, "Sizes", menu);

  /* Create the text which appears on the page. (C) Eric Meyer. */
  data->text = new_ml_text_label
    (pool,
     "\n"
     "In other CSS-aware browsers, like Internet Explorer or Opera, you'll see the toplevel links, and they'll work just fine. The popout menus won't work, that's all. Of course, this means that the browser is downloading the contents of the complete menu, but only able to display a small part of what it downloaded. This is in some respects unfortunate, but at least the basic menu will still function. And the amount of markup involved is probably a lot fewer characters than a similar Javascript-driven menu intended to serve a similar purpose.\n\n"
     "(And if you're grumbling that you don't see why anyone would deploy a menu system that only worked in one browser, then you should see how many Javascript menus I deal with that don't work in more than one or two browsers-- usually because they're doing browser sniffing as if this were still 1998. But I digress.)\n\n"
     "If the browser is too old to understand CSS, or doesn't support CSS because (for example) it's a text-mode browser like Lynx, then the user will get the entire menu as a set of nested lists. No problem.\n\n");

  /* Put the text into the menubar widget, so it appears. */
  ml_menubar_pack (data->menubar, data->text);

  /* Pack the menubar inside the window. */
  ml_window_pack (data->win, data->menubar);
}

static void
set_red (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "color", "red");
}

static void
set_green (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "color", "green");
}

static void
set_blue (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "color", "blue");
}

static void
set_black (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "color", "black");
}

static void
set_small (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "font.size", "small");
}

static void
set_medium (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "font.size", "medium");
}

static void
set_large (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;

  ml_widget_set_property (data->text, "font.size", "large");
}
