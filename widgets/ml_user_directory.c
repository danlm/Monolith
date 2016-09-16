/* Monolith user directory widget.
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
 * $Id: ml_user_directory.c,v 1.1 2002/11/13 20:46:38 rich Exp $
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
#include "ml_form.h"
#include "ml_form_select.h"
#include "ml_form_submit.h"
#include "ml_table_layout.h"
#include "ml_user_directory.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations user_directory_ops =
  {
    repaint: repaint
  };

struct ml_user_directory
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_session session;		/* Current session. */
  ml_dbh_factory dbf;		/* Database factory. */
  ml_form form;			/* Form (can be NULL). */
  int userid;			/* Currently selected userid (0 = none). */
  ml_widget top;		/* This is our top-level widget. */
  ml_form_select select;	/* Selection widget. */
  vector users;			/* List of all users (vector of int). */
};

static void make_select (pool, ml_session, ml_dbh_factory,
			 ml_form, int userid,
			 ml_form_select *select_rtn,
			 vector *users_rtn);

ml_user_directory
new_ml_user_directory (pool pool, ml_session session, ml_dbh_factory dbf,
		       ml_form form, int userid)
{
  ml_user_directory w = pmalloc (pool, sizeof *w);

  w->ops = &user_directory_ops;
  w->pool = pool;
  w->session = session;
  w->dbf = dbf;
  w->userid = userid;

  /* Create the table widget. */
  if (form)			/* We are in an existing form. */
    {
      w->form = form;
      make_select (pool, session, dbf, form, userid, &w->select, &w->users);
      w->top = w->select;
    }
  else				/* We are a standalone widget. */
    {
      ml_table_layout tbl;
      ml_form_submit submit;

      w->form = new_ml_form (w->pool);
      tbl = new_ml_table_layout (pool, 1, 2);
      make_select (pool, session, dbf, w->form, userid, &w->select, &w->users);
      ml_table_layout_pack (tbl, w->select, 0, 0);
      submit = new_ml_form_submit (pool, w->form, "Go");
      ml_table_layout_pack (tbl, submit, 0, 1);
      ml_form_pack (w->form, tbl);

      w->top = w->form;
    }

  return w;
}

static void
make_select (pool pool, ml_session session, ml_dbh_factory dbf,
	     ml_form form, int selected_userid,
	     ml_form_select *select, vector *users)
{
  db_handle dbh;
  st_handle sth;
  int userid;
  const char *email, *given_name, *family_name;

  *select = new_ml_form_select (pool, form);
  *users = new_vector (pool, int);

  dbh = ml_get_dbh (session, dbf);

  /* Pull out the list of users, and details. */
  sth = st_prepare_cached
    (dbh,
     "select p.userid, u.email, u.given_name, u.family_name "
     "from ml_userdir_prefs p, ml_users u "
     "order by u.given_name, u.family_name, u.email");
  st_execute (sth);

  st_bind (sth, 0, userid, DBI_INT);
  st_bind (sth, 1, email, DBI_STRING);
  st_bind (sth, 2, given_name, DBI_STRING);
  st_bind (sth, 3, family_name, DBI_STRING);

  while (st_fetch (sth))
    {
      vector_push_back (*users, userid);
      ml_form_select_push_back
	(*select,
	 psprintf (pool, "%s %s <%s>", given_name, family_name, email));

      if (userid == selected_userid)
	ml_form_select_set_selection (*select,
				      ml_form_select_size (*select) - 1);
    }

  ml_put_dbh (session, dbh);
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_user_directory w = (ml_user_directory) vw;

  ml_widget_repaint (w->top, session, windowid, io);
}
