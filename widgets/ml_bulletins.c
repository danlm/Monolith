/* Monolith bulletins (recent news spool).
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
 * $Id: ml_bulletins.c,v 1.11 2002/12/01 14:58:30 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <pstring.h>
#include <pre.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_button.h"
#include "ml_window.h"
#include "ml_table_layout.h"
#include "ml_form_layout.h"
#include "ml_text_label.h"
#include "ml_dialog.h"
#include "ml_form.h"
#include "ml_form_input.h"
#include "ml_form_textarea.h"
#include "ml_form_select.h"
#include "ml_form_text.h"
#include "ml_form_submit.h"
#include "ml_bulletins.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations bulletins_ops =
  {
    repaint: repaint
  };

struct ml_bulletins
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_session session;		/* Current session. */
  ml_dbh_factory dbf;		/* Database factory. */
  int sectionid;		/* Which section? */
  int first_item;		/* First item to display. */
  int nr_items;			/* Number of items to display on each page. */
  ml_button post, home, prev, next; /* Buttons along the bottom. */

  /* These are used during posting. */
  ml_form_textarea post_item;
  ml_form_select post_type;
  ml_form_text post_link;
  ml_form_text post_link_text;
};

static int can_post (db_handle dbh, int sectionid, int userid);
static void post (ml_session, void *vw);
static void update_buttons (ml_bulletins w);
static void home_button (ml_session, void *vw);
static void prev_button (ml_session, void *vw);
static void next_button (ml_session, void *vw);
static void post_button (ml_session, void *vw);

ml_bulletins
new_ml_bulletins (pool pool, ml_session session, ml_dbh_factory dbf,
		  const char *section_name)
{
  ml_bulletins w = pmalloc (pool, sizeof *w);
  db_handle dbh;
  st_handle sth;

  w->ops = &bulletins_ops;
  w->pool = pool;
  w->session = session;
  w->dbf = dbf;
  w->first_item = 0;
  w->nr_items = 10;

  /* Get the sectionid. */
  dbh = ml_get_dbh (session, dbf);

  sth = st_prepare_cached
    (dbh,
     "select resid from ml_resources where name = ?", DBI_STRING);
  st_execute (sth, section_name);

  st_bind (sth, 0, w->sectionid, DBI_INT);

  if (!st_fetch (sth)) return 0;

  ml_put_dbh (session, dbh);

  /* Create the buttons for the bottom of the page. The home/prev/next
   * buttons get enabled (possibly) in update_buttons. The post button
   * is always enabled, but only shown to eligible posters.
   */
  w->post = new_ml_button (pool, "Post");
  ml_button_set_callback (w->post, post_button, session, w);
  ml_button_set_popup (w->post, "bulletins_post_window");
  ml_button_set_popup_size (w->post, 400, 300);

  w->home = new_ml_button (pool, "Most recent");
  w->prev = new_ml_button (pool, "&lt;&lt;");
  w->next = new_ml_button (pool, "&gt;&gt;");
  update_buttons (w);

  return w;
}

/* Callback for the "home" button. */
static void
home_button (ml_session session, void *vw)
{
  ml_bulletins w = (ml_bulletins) vw;

  w->first_item = 0;
  update_buttons (w);
}

/* Callback for the "prev" button. */
static void
prev_button (ml_session session, void *vw)
{
  ml_bulletins w = (ml_bulletins) vw;

  w->first_item -= w->nr_items;
  if (w->first_item < 0) w->first_item = 0;
  update_buttons (w);
}

/* Callback for the "next" button. */
static void
next_button (ml_session session, void *vw)
{
  ml_bulletins w = (ml_bulletins) vw;

  w->first_item += w->nr_items;
  update_buttons (w);
}

static inline void
enable_home (ml_bulletins w)
{
  ml_button_set_callback (w->home, home_button, w->session, w);
}

static inline void
disable_home (ml_bulletins w)
{
  ml_button_set_callback (w->home, 0, w->session, 0);
}

static inline void
enable_prev (ml_bulletins w)
{
  ml_button_set_callback (w->prev, prev_button, w->session, w);
}

static inline void
disable_prev (ml_bulletins w)
{
  ml_button_set_callback (w->prev, 0, w->session, 0);
}

static inline void
enable_next (ml_bulletins w)
{
  ml_button_set_callback (w->next, next_button, w->session, w);
}

static inline void
disable_next (ml_bulletins w)
{
  ml_button_set_callback (w->next, 0, w->session, 0);
}

/* This function updates the state of each button. It consults the
 * database to find out how many articles are present.
 */
static void
update_buttons (ml_bulletins w)
{
  db_handle dbh;
  st_handle sth;
  int count;

  /* Get a database handle. */
  dbh = ml_get_dbh (w->session, w->dbf);

  /* Find out how many articles are present. */
  sth = st_prepare_cached
    (dbh, "select count (*) from ml_bulletins");
  st_execute (sth);

  st_bind (sth, 0, count, DBI_INT);
  if (!st_fetch (sth))
    pth_die ("select count(*) returned no rows!");

  /* Make sure first_item is sensible. */
  if (w->first_item >= count)
    w->first_item = count - w->nr_items;
  if (w->first_item < 0)
    w->first_item = 0;

  /* Decide which buttons to enable. */
  if (w->first_item > w->nr_items)
    enable_home (w);
  else
    disable_home (w);

  if (w->first_item >= w->nr_items)
    enable_prev (w);
  else
    disable_prev (w);

  if (w->first_item + w->nr_items < count)
    enable_next (w);
  else
    disable_next (w);
}

static void
post_button (ml_session session, void *vw)
{
  ml_bulletins w = (ml_bulletins) vw;
  db_handle dbh;
  ml_window win;
  ml_form_layout tbl;
  ml_form form;
  ml_form_submit sub;

  /* Get a database handle. */
  dbh = ml_get_dbh (session, w->dbf);

  /* Is the current user allowed to post? It can happen that this
   * function is called even if the user is not a legitimate poster.
   * For example:
   * (1) User displays the front page, with "Post" button, then logs
   *     out in another window, then presses the "Post" button.
   * (2) User is attempting to predict action IDs (note that the "Post"
   *     button is registered, even if it not displayed).
   * It's always a good idea to check permissions inside callback
   * functions.
   */
  if (!can_post (dbh, w->sectionid, ml_session_userid (session)))
    {
      ml_error_window
	(w->pool, session,
	 "You do not have permission to post in this section.",
	 ML_DIALOG_CLOSE_BUTTON);
      return;
    }

  /* Display the posting form. */
  win = new_ml_window (session, w->pool);
  form = new_ml_form (w->pool);
  ml_form_set_callback (form, post, session, w);
  ml_widget_set_property (form, "method", "GET");
  tbl = new_ml_form_layout (w->pool);

  w->post_item = new_ml_form_textarea (w->pool, form, 3, 40);
  ml_form_layout_pack (tbl, "Message:", w->post_item);

  w->post_type = new_ml_form_select (w->pool, form);
  ml_form_select_push_back (w->post_type, "Plain text");
  ml_form_select_push_back (w->post_type, "*Smart* text");
  ml_form_select_push_back (w->post_type, "HTML");
  ml_form_select_set_selection (w->post_type, 1); /* XXX From preferences. */
  ml_form_layout_pack (tbl, 0, w->post_type);

  w->post_link = new_ml_form_text (w->pool, form);
  ml_form_layout_pack (tbl, "URL:", w->post_link);

  w->post_link_text = new_ml_form_text (w->pool, form);
  ml_form_layout_pack (tbl, "Link text:", w->post_link_text);

  sub = new_ml_form_submit (w->pool, form, "Post");
  ml_form_layout_pack (tbl, 0, sub);

  /* Pack everything up. */
  ml_form_pack (form, tbl);
  ml_window_pack (win, form);
}

static void
post (ml_session session, void *vw)
{
  ml_bulletins w = (ml_bulletins) vw;
  db_handle dbh;
  st_handle sth;
  const char *item, *item_type, *link, *link_text;
  int type, userid;

  /* Verify the details of the posting, otherwise do nothing, which just
   * represents the form back to the user.
   */
  item = ml_form_input_get_value (w->post_item);
  link = ml_form_input_get_value (w->post_link);
  link_text = ml_form_input_get_value (w->post_link_text);

  if (!item || strlen (item) == 0) return;

  type = ml_form_select_get_selection (w->post_type);

  /* Turn empty strings into nulls for the database. */
  if (strlen (link) == 0) link = 0;
  if (strlen (link_text) == 0) link_text = 0;

  /* Set the item type. */
  switch (type) {
  case 0: item_type = "p"; break;
  case 1: item_type = "s"; break;
  case 2: item_type = "h"; break;
  default: item_type = "s";
  }

  /* Get a database handle. */
  dbh = ml_get_dbh (session, w->dbf);

  /* Verify the user can post. See notes above. */
  userid = ml_session_userid (session);
  if (!can_post (dbh, w->sectionid, userid))
    {
      ml_error_window
	(w->pool, session,
	 "You do not have permission to post in this section.",
	 ML_DIALOG_CLOSE_BUTTON);
      return;
    }

  /* Insert the posting. */
  sth = st_prepare_cached
    (dbh,
     "insert into ml_bulletins "
     "(sectionid, authorid, item, item_type, link, link_text) "
     "values (?, ?, ?, ?, ?, ?)",
     DBI_INT, DBI_INT, DBI_STRING, DBI_STRING, DBI_STRING, DBI_STRING);
  st_execute (sth, w->sectionid, userid, item, item_type, link, link_text);

  /* Commit to the database. */
  db_commit (dbh);
  ml_put_dbh (session, dbh);

  /* Present a confirmation page. */
  ml_ok_window (w->pool, session,
		"Item was successfully posted.",
		ML_DIALOG_CLOSE_BUTTON | ML_DIALOG_CLOSE_RELOAD_OPENER);
}

static inline void
show_item (io_handle io, int n, char *item, char *item_type, char *username,
	   char *posted_date, char *timediff, char *link, char *link_text)
{
  /* XXX Lots of issues in this block:
   * (2) parsing/printing of dates
   * (3) smart text support
   * (4) escaping of link text
   * (5) styling of the whole thing
   */
  io_fprintf (io, "<table width=\"100%%\"><tr>"
	      "<td rowspan=\"3\" valign=\"top\">%d.</td>",
	      n);
  io_fprintf (io, "<td>Posted by <strong>%s</strong> on "
	      "<strong>%s</strong></td></tr>",
	      username, posted_date);
  io_fprintf (io, "<tr><td>%s</td></tr>", item);
  if (link && link_text)
    io_fprintf (io, "<tr><td align=\"right\">"
		"<a href=\"%s\">%s</a></td></tr>",
		link, link_text);
  else if (link)
    io_fprintf (io, "<tr><td align=\"right\">"
		"<a href=\"%s\">%s</a></td></tr>",
		link, link);
  else
    io_fprintf (io, "<tr><td></td></tr>");
  io_fprintf (io, "</table>");
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_bulletins w = (ml_bulletins) vw;
  db_handle dbh;
  st_handle sth;
  char *item, *item_type, *username, *posted_date, *timediff,
    *link, *link_text;
  int n, is_poster;

  /* Get a database handle. */
  dbh = ml_get_dbh (session, w->dbf);

  /* Is the current user allowed to post/remove articles? */
  is_poster = can_post (dbh, w->sectionid, ml_session_userid (session));

  /* Pull out the headlines. */
  sth = st_prepare_cached
    (dbh,
     "select b.item, b.item_type, u.username, b.posted_date, "
     "       current_timestamp - b.posted_date, "
     "       b.link, b.link_text "
     "from ml_bulletins b, ml_users u "
     "where b.sectionid = ? and b.authorid = u.userid "
     "order by 4 desc "
     "limit ? "
     "offset ?",
     DBI_INT, DBI_INT, DBI_INT);
  st_execute (sth, w->sectionid, w->nr_items, w->first_item);

  st_bind (sth, 0, item, DBI_STRING);
  st_bind (sth, 1, item_type, DBI_STRING);
  st_bind (sth, 2, username, DBI_STRING);
  st_bind (sth, 3, posted_date, DBI_STRING);
  st_bind (sth, 4, timediff, DBI_STRING);
  st_bind (sth, 5, link, DBI_STRING);
  st_bind (sth, 6, link_text, DBI_STRING);

  /* Display them. */
  io_fprintf (io, "<table><tr><td><table>");

  n = w->first_item + 1;

  while (st_fetch (sth))
    {
      io_fprintf (io, "<tr><td>");

      show_item (io, n, item, item_type, username, posted_date, timediff,
		 link, link_text);

      io_fprintf (io, "</td></tr>");

      n++;
    }

  /* Finish off the page with the buttons at the bottom. */
  io_fprintf (io, "</table></td></tr><tr><td align=\"right\">");
  if (is_poster)
    ml_widget_repaint (w->post, session, windowid, io);
  ml_widget_repaint (w->home, session, windowid, io);
  ml_widget_repaint (w->prev, session, windowid, io);
  ml_widget_repaint (w->next, session, windowid, io);
  io_fprintf (io, "</td></tr></table>");

  /* Be polite: give back the database handle. */
  ml_put_dbh (session, dbh);
}

static int
can_post (db_handle dbh, int sectionid, int userid)
{
  st_handle sth;

  if (userid)
    {
      sth = st_prepare_cached
	(dbh,
	 "select 1 from ml_bulletins_posters "
	 "where sectionid = ? and userid = ?", DBI_INT, DBI_INT);
      st_execute (sth, sectionid, userid);

      return st_fetch (sth);
    }
  else
    return 0;
}
