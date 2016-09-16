/* Monolith statistics/debugging application.
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
 * $Id: stats.c,v 1.9 2003/02/08 15:50:26 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <pool.h>
#include <pstring.h>

#include <pthr_pseudothread.h>
#include <pthr_cgi.h>
#include <pthr_dbi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_widget.h"
#include "ml_form_layout.h"
#include "ml_table_layout.h"
#include "ml_multicol_layout.h"
#include "ml_flow_layout.h"
#include "ml_form.h"
#include "ml_form_select.h"
#include "ml_form_submit.h"
#include "ml_text_label.h"
#include "ml_button.h"
#include "ml_dialog.h"

/*----- The following standard boilerplate code must appear -----*/

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/*----- End of standard boilerplate code -----*/

/* Per-session data structure. */
struct data
{
  pool pool;			/* Session pool for allocations. */

  /* This is the top-level window. */
  ml_window std_win;
  ml_table_layout std_tbl;
  ml_form std_form;		/* Area selection form. */
  ml_flow_layout std_flow;
  ml_form_select std_choices;	/* Top-level choices. */
  ml_form_submit std_submit;
};

/* Arguments to the show_session function. */
struct show_session_args
{
  const char *sessionid;
  struct data *data;
};

static void list_dbfs (ml_session session, struct data *data);
static void show_reactor (ml_session session, struct data *data);
static void list_sessions (ml_session session, struct data *data);
static void show_session (ml_session session, void *vargs);
static void list_threads (ml_session session, struct data *data);
static void jump_to (ml_session session, void *vdata);
static void pack (struct data *data, ml_widget widget);
static const char *resolve_addr (pool pool, unsigned long addr);
static const char *pr_time (pool pool, reactor_time_t time);

static struct {
  const char *choice;
  void (*fn) (ml_session, struct data *data);
  int is_default;
} choices[] = {
  { "Database handle factories", list_dbfs,     0 },
  { "Reactor",                   show_reactor,  0 },
  { "Sessions",                  list_sessions, 1 },
  { "Threads",                   list_threads,  0 },
};

#define nr_choices (sizeof choices / sizeof choices[0])

/* If this variable is set, then sessionids are not revealed in full. Set
 * this using the 'monolith stats restricted: 1' flag in the configuration
 * file.
 */
static int restricted = 1;

/* If this variable is set, then the application is available to everyone.
 * If not set, then it is only available to localhost. Set this using the
 * 'monolith stats world readable: 1' flag in the configuration file.
 */
static int world_readable = 0;

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;
  int i;

  /* Read configuration settings. */
  restricted = ml_cfg_get_bool (session, "monolith stats restricted", 1);
  world_readable =
    ml_cfg_get_bool (session, "monolith stats world readable", 0);

  /* Allocate per-session data structure. */
  data = pmalloc (pool, sizeof *data);
  data->pool = pool;

  /* Lay out the window. */
  data->std_win = new_ml_window (session, pool);
  data->std_tbl = new_ml_table_layout (pool, 2, 1);
  data->std_form = new_ml_form (pool);
  data->std_flow = new_ml_flow_layout (pool);
  data->std_choices = new_ml_form_select (pool, data->std_form);
  ml_flow_layout_pack (data->std_flow, data->std_choices);
  data->std_submit = new_ml_form_submit (pool, data->std_form, "Go");
  ml_flow_layout_pack (data->std_flow, data->std_submit);
  ml_form_pack (data->std_form, data->std_flow);
  ml_table_layout_pack (data->std_tbl, data->std_form, 0, 0);
  ml_window_pack (data->std_win, data->std_tbl);

  /* Populate the list of choices. */
  for (i = 0; i < nr_choices; ++i)
    {
      ml_form_select_push_back (data->std_choices, choices[i].choice);
      if (choices[i].is_default)
	ml_form_select_set_selection (data->std_choices, i);
    }

  /* Set the form callback so that we jump to the right page. */
  ml_form_set_callback (data->std_form, jump_to, session, data);

  /* Set the window title. */
  ml_window_set_title (data->std_win,
		       "Monolith statistics and debugging application");

  /* Jump to the initial page (default choice). */
  jump_to (session, data);
}

static void
jump_to (ml_session session, void *vdata)
{
  struct data *data = (struct data *) vdata;
  int i;

  /* Find out which choice is selected. */
  i = ml_form_select_get_selection (data->std_choices);
  if (i >= 0 && i < nr_choices)
    /* Run that function to update the page. */
    choices[i].fn (session, data);
}

static void
pack (struct data *data, ml_widget widget)
{
  ml_table_layout_pack (data->std_tbl, widget, 1, 0);
}

static const char *
disguise_sessionid (pool pool, const char *sessionid)
{
  if (!restricted) return sessionid;
  else
    {
      char *s = pstrndup (pool, sessionid, 6);
      s = pstrcat (pool, s, "[...]");
      return s;
    }
}

static void
list_dbfs (ml_session session, struct data *data)
{
  pool pool = data->pool;
  vector dbfs;
  ml_multicol_layout tbl;
  ml_text_label lbl;
  int i;

  /* Pull out the list of conninfos. */
  dbfs = _ml_get_dbh_factories (pool);

  tbl = new_ml_multicol_layout (pool, 3);
  ml_widget_set_property (tbl, "class", "ml_stats_table");

  /* Table headers. */
  lbl = new_ml_text_label (pool, "conninfo");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "total");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "in use/free");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);

  /* The database handles themselves. */
  for (i = 0; i < vector_size (dbfs); ++i)
    {
      const char *conninfo;
      ml_dbh_factory dbf;
      int total, in_use, free_hs;
      ml_button b;

      vector_get (dbfs, i, conninfo);
      dbf = _ml_get_dbh_factory (conninfo);

      if (strlen (conninfo) > 0)
	b = new_ml_button (pool, conninfo);
      else
	b = new_ml_button (pool, "(empty string)");
      ml_widget_set_property (b, "button.style", "link");
      ml_multicol_layout_pack (tbl, b);

      total = _ml_dbh_factory_get_nr_allocated_handles (dbf);
      lbl = new_ml_text_label (pool, pitoa (pool, total));
      ml_multicol_layout_pack (tbl, lbl);

      free_hs = _ml_dbh_factory_get_nr_free_handles (dbf);
      in_use = total - free_hs;
      lbl = new_ml_text_label (pool,
			       psprintf (pool, "%d/%d", in_use, free_hs));
      ml_multicol_layout_pack (tbl, lbl);
    }

  pack (data, tbl);
}

static void
show_reactor (ml_session session, struct data *data)
{
  /* XXX not impl. */
}

static void
list_sessions (ml_session session, struct data *data)
{
  pool pool = data->pool;
  vector sessionids;
  ml_multicol_layout tbl;
  ml_text_label lbl;
  int i;

  /* Pull out the list of session IDs and turn them into session objects. */
  sessionids = _ml_get_sessions (pool);

  tbl = new_ml_multicol_layout (pool, 5);
  ml_widget_set_property (tbl, "class", "ml_stats_table");

  /* Table headers. */
  lbl = new_ml_text_label (pool, "sessionid");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "last access");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "address");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "size");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "path");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);

  /* The sessions themselves. */
  for (i = 0; i < vector_size (sessionids); ++i)
    {
      const char *sessionid;
      ml_session s;
      ml_button b;
      struct sockaddr_in addr;
      const char *host_header, *canonical_path;
      struct pool *sp;
      struct pool_stats pool_stats;
      struct show_session_args *args;

      vector_get (sessionids, i, sessionid);
      s = _ml_get_session (sessionid);

      b = new_ml_button (pool, disguise_sessionid (pool, sessionid));
      ml_widget_set_property (b, "button.style", "link");
      args = pmalloc (pool, sizeof *args);
      args->sessionid = sessionid;
      args->data = data;
      ml_button_set_callback (b, show_session, session, args);
      ml_button_set_popup (b, psprintf (pool, "session_%s", sessionid));
      ml_button_set_popup_size (b, 640, 480);
      ml_multicol_layout_pack (tbl, b);

      lbl = new_ml_text_label (pool,
			       pr_time (pool,
					_ml_session_get_last_access (s)));
      ml_multicol_layout_pack (tbl, lbl);

      addr = _ml_session_get_original_ip (s);
      lbl = new_ml_text_label (pool, pstrdup (pool,
					      inet_ntoa (addr.sin_addr)));
      ml_multicol_layout_pack (tbl, lbl);

      sp = ml_session_pool (s);
      pool_get_stats (sp, &pool_stats, sizeof (pool_stats));
      lbl = new_ml_text_label (pool, pitoa (pool, pool_stats.struct_size));
      ml_multicol_layout_pack (tbl, lbl);

      host_header = ml_session_host_header (s);
      canonical_path = ml_session_canonical_path (s);
      lbl = new_ml_text_label (pool,
			       psprintf (pool, "http://%s%s",
					 host_header, canonical_path));
      ml_multicol_layout_pack (tbl, lbl);
    }

  pack (data, tbl);
}

static void
show_session (ml_session session, void *vargs)
{
  struct show_session_args *args = (struct show_session_args *) vargs;
  struct data *data = args->data;
  pool pool = data->pool;
  const char *sessionid = args->sessionid;
  ml_window win;
  ml_form_layout tbl;
  ml_text_label lbl;
  ml_session s;

  win = new_ml_window (session, pool);
  tbl = new_ml_form_layout (pool);
  ml_widget_set_property (tbl, "class", "ml_stats_table");

  /* Get the session. */
  s = _ml_get_session (sessionid);
  if (!s)
    {
      ml_error_window (pool, session,
		       "Session not found. It is likely that the session "
		       "has timed out and been deleted.",
		       ML_DIALOG_CLOSE_BUTTON);
      return;
    }

  /* Get the fields from the session and display them. */

  ml_form_layout_pack (tbl, "sessionid",
		       new_ml_text_label (pool,
			 disguise_sessionid (pool, sessionid)));
  ml_form_layout_pack (tbl, "path",
		       new_ml_text_label (pool,
			 psprintf (pool, "http://%s%s",
				   ml_session_host_header (s),
				   ml_session_canonical_path (s))));

  /* Pool and pool stats. */
  {
    struct pool *sp;
    struct pool_stats pool_stats;

    sp = ml_session_pool (s);
    pool_get_stats (sp, &pool_stats, sizeof (pool_stats));

    ml_form_layout_pack (tbl, "session pool",
			 new_ml_text_label (pool, psprintf (pool, "%p", sp)));
    ml_form_layout_pack (tbl, "session pool size",
			 new_ml_text_label (pool,
			   pitoa (pool, pool_stats.struct_size)));
  }

  ml_form_layout_pack (tbl, "access count",
		       new_ml_text_label (pool,
			 pitoa (pool, _ml_session_get_hits (s))));
  ml_form_layout_pack (tbl, "last access",
		       new_ml_text_label (pool,
			 pr_time (pool, _ml_session_get_last_access (s))));
  ml_form_layout_pack (tbl, "creation time",
		       new_ml_text_label (pool,
			 pr_time (pool, _ml_session_get_created (s))));

  /* Original IP address. */
  {
    struct sockaddr_in addr;

    addr = _ml_session_get_original_ip (s);
    ml_form_layout_pack (tbl, "original ip",
			 new_ml_text_label (pool,
			   pstrdup (pool, inet_ntoa (addr.sin_addr))));
  }

  /* User-Agent header. */
  {
    const char *ua = ml_session_user_agent (s);

    ml_form_layout_pack (tbl, "user agent",
			 new_ml_text_label (pool, ua ? pstrdup (pool, ua)
					    : "(not set)"));
  }

  /* Initial args. */
  {
    cgi args;
    ml_multicol_layout args_tbl;
    int i;
    vector keys;

    args = ml_session_args (s);
    /* XXX cgi_params_in_pool function */
    keys = cgi_params (args);

    args_tbl = new_ml_multicol_layout (pool, 2);
    ml_widget_set_property (args_tbl, "class", "ml_stats_table");

    lbl = new_ml_text_label (pool, "name");
    ml_multicol_layout_set_header (args_tbl, 1);
    ml_multicol_layout_pack (args_tbl, lbl);

    lbl = new_ml_text_label (pool, "value");
    ml_multicol_layout_set_header (args_tbl, 1);
    ml_multicol_layout_pack (args_tbl, lbl);

    for (i = 0; i < vector_size (keys); ++i)
      {
	const char *key, *value;

	vector_get (keys, i, key);
	value = cgi_param (args, key);

	lbl = new_ml_text_label (pool, pstrdup (pool, key));
	ml_multicol_layout_pack (args_tbl, lbl);
	lbl = new_ml_text_label (pool, pstrdup (pool, value));
	ml_multicol_layout_pack (args_tbl, lbl);
      }

    ml_form_layout_pack (tbl, "initial parameters", args_tbl);
  }

  ml_form_layout_pack (tbl, "app_main function",
		       new_ml_text_label (pool,
			 resolve_addr (pool,
			   (unsigned long) _ml_session_get_app_main (s))));

  /* Windows. */
  {
    vector windows;
    ml_multicol_layout win_tbl;
    int i;

    windows = _ml_session_get_windows (s, pool);

    win_tbl = new_ml_multicol_layout (pool, 2);
    ml_widget_set_property (win_tbl, "class", "ml_stats_table");

    lbl = new_ml_text_label (pool, "windowid");
    ml_multicol_layout_set_header (win_tbl, 1);
    ml_multicol_layout_pack (win_tbl, lbl);

    lbl = new_ml_text_label (pool, "window");
    ml_multicol_layout_set_header (win_tbl, 1);
    ml_multicol_layout_pack (win_tbl, lbl);

    for (i = 0; i < vector_size (windows); ++i)
      {
	const char *windowid;
	ml_window window;

	vector_get (windows, i, windowid);
	window = _ml_session_get_window (s, windowid);

	lbl = new_ml_text_label (pool, windowid);
	ml_multicol_layout_pack (win_tbl, lbl);
	lbl = new_ml_text_label (pool, psprintf (pool, "%p", window));
	ml_multicol_layout_pack (win_tbl, lbl);
      }

    ml_form_layout_pack (tbl, "windows", win_tbl);
  }

  /* Actions. */
  {
    vector actions;
    ml_multicol_layout act_tbl;
    int i;

    actions = _ml_session_get_actions (s, pool);

    act_tbl = new_ml_multicol_layout (pool, 3);
    ml_widget_set_property (act_tbl, "class", "ml_stats_table");

    lbl = new_ml_text_label (pool, "actionid");
    ml_multicol_layout_set_header (act_tbl, 1);
    ml_multicol_layout_pack (act_tbl, lbl);

    lbl = new_ml_text_label (pool, "function");
    ml_multicol_layout_set_header (act_tbl, 1);
    ml_multicol_layout_pack (act_tbl, lbl);

    lbl = new_ml_text_label (pool, "data");
    ml_multicol_layout_set_header (act_tbl, 1);
    ml_multicol_layout_pack (act_tbl, lbl);

    for (i = 0; i < vector_size (actions); ++i)
      {
	const char *actionid;
	void *fn, *data;

	vector_get (actions, i, actionid);
	_ml_session_get_action (s, actionid, &fn, &data);

	lbl = new_ml_text_label (pool, actionid);
	ml_multicol_layout_pack (act_tbl, lbl);
	lbl = new_ml_text_label (pool, resolve_addr (pool,
						       (unsigned long) fn));
	ml_multicol_layout_pack (act_tbl, lbl);
	lbl = new_ml_text_label (pool, psprintf (pool, "%p", data));
	ml_multicol_layout_pack (act_tbl, lbl);
      }

    ml_form_layout_pack (tbl, "actions", act_tbl);
  }

  /* Database handles. */
  {
    vector dbhs;

    dbhs = _ml_session_get_dbhs (s, pool);

    ml_form_layout_pack (tbl, "active database handles",
			 new_ml_text_label (pool,
			   pitoa (pool, vector_size (dbhs))));
  }

  ml_form_layout_pack (tbl, "userid",
		       new_ml_text_label (pool,
			 pitoa (pool, ml_session_userid (s))));

  ml_window_pack (win, tbl);

  /* Set window title. */
  ml_window_set_title (win, psprintf (pool, "Session %s",
				      disguise_sessionid (pool, sessionid)));
}

static void
list_threads (ml_session session, struct data *data)
{
  pool pool = data->pool;
  vector threads;
  int i;
  ml_multicol_layout tbl;
  ml_text_label lbl;

  /* Get the threads. */
  threads = pseudothread_get_threads (pool);

  tbl = new_ml_multicol_layout (pool, 4);
  ml_widget_set_property (tbl, "class", "ml_stats_table");

  /* Table headers. */
  lbl = new_ml_text_label (pool, "id");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "program counter");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "size");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);
  lbl = new_ml_text_label (pool, "name");
  ml_multicol_layout_set_header (tbl, 1);
  ml_multicol_layout_pack (tbl, lbl);

  for (i = 0; i < vector_size (threads); ++i)
    {
      pseudothread pth;
      ml_button b;
      struct pool *tp;
      struct pool_stats pool_stats;
      unsigned long pc;
      const char *pc_symbol;
      const char *name;

      vector_get_ptr (threads, i, pth);

      lbl = new_ml_text_label (pool, pitoa (pool, pth_get_thread_num (pth)));
      ml_multicol_layout_pack (tbl, lbl);

      pc = pth_get_PC (pth);
      pc_symbol = resolve_addr (pool, pc);
      lbl = new_ml_text_label (pool, pc_symbol);
      ml_multicol_layout_pack (tbl, lbl);

      tp = pth_get_pool (pth);
      pool_get_stats (tp, &pool_stats, sizeof (pool_stats));
      lbl = new_ml_text_label (pool, pitoa (pool, pool_stats.struct_size));
      ml_multicol_layout_pack (tbl, lbl);

      name = pstrdup (pool, pth_get_name (pth));
      b = new_ml_button (pool, name);
      ml_widget_set_property (b, "button.style", "link");
      ml_multicol_layout_pack (tbl, b);
    }

  pack (data, tbl);
}

static const char *
pr_time (pool pool, reactor_time_t time)
{
  int secs = (reactor_time - time) / 1000LL;

  if (secs < 60)
    return psprintf (pool, "%ds", secs);
  else if (secs < 3600)
    return psprintf (pool, "%dm %ds", secs / 60, secs % 60);
  else
    return psprintf (pool, "%dh %dm %ds", secs / 3600, (secs / 60) % 60,
		     secs % 60);
}

/*----- Address to symbol resolution code. -----*/

/* This code is highly Linux-dependent.
 *
 * The strategy taken is as follows:
 *
 * (1) Look at /proc/$$/maps (if it exists) to try and work out which
 *     executable or shared library the symbol exists in.
 * (2) Look for a corresponding symbol table file in /usr/share/rws/symtabs/
 * (3) If the symbol table file is found, then we can resolve the symbol
 *     immediately with that file.
 * (4) If no symbol table file is found, use the dladdr(3) function from
 *     glibc.
 * (5) If there is no dladdr(3) function, just print the address.
 *
 * All of this must be done without blocking.
 */

#define RES_DEBUG 0

static const char *resolve_addr_in_maps (pool, unsigned long);
static const char *resolve_addr_in_symtab (pool, unsigned long, const char *,
					   unsigned long, unsigned long);
static const char *resolve_addr_with_dladdr (pool, unsigned long);

static const char *
resolve_addr (pool session_pool, unsigned long addr)
{
  pool pool;
  const char *s;

#if RES_DEBUG
  fprintf (stderr, "trying to resolve address %lx\n", addr);
#endif

  /* Give the address resolution code its own scratch pool, but copy
   * the result into the more permanent storage of the session pool.
   */
  pool = new_subpool (session_pool);
  s = pstrdup (session_pool, resolve_addr_in_maps (pool, addr));
  delete_pool (pool);

#if RES_DEBUG
  fprintf (stderr, "resolved address %lx as %s\n", addr, s);
#endif

  return s;
}

static const char *
resolve_addr_in_maps (pool pool, unsigned long addr)
{
  FILE *fp;
  char *maps_filename;
#define RES_BUF_SIZE 2048
  char *buffer;
  char *filename, *t;
  unsigned long low, high, offset;

  if (addr == 0) return "NULL";

  maps_filename = psprintf (pool, "/proc/%d/maps", (int) getpid ());

  fp = fopen (maps_filename, "r");
  if (fp == 0) return resolve_addr_with_dladdr (pool, addr);

  buffer = pmalloc (pool, RES_BUF_SIZE);

  /* Read the maps file until we find the appropriate address range. */
  while (fgets (buffer, RES_BUF_SIZE, fp) != 0)
    {
      if (buffer[strlen(buffer)-1] == '\n')
	buffer[strlen(buffer)-1] = '\0';

      if (sscanf (buffer, "%lx-%lx r-xp %lx", &low, &high, &offset) != 3)
	continue;

#if RES_DEBUG
      fprintf (stderr, "\t%lx-%lx offset %lx\n", low, high, offset);
#endif
      if (!(low <= addr && addr < high))
	continue;

      /* Found the address, so we can close the file. */
      fclose (fp);

      /* Find the filename (just the basename). */
      filename = strrchr (buffer, '/');
      if (!filename) return resolve_addr_with_dladdr (pool, addr);
      filename++;

      t = strchr (filename, '.');
      if (t) *t = '\0';

#if RES_DEBUG
      fprintf (stderr, "\tbase filename = %s\n", filename);
#endif

      return resolve_addr_in_symtab (pool, addr, filename,
				     low, offset);
    }

  /* Address not found. Go to the back-up approach. */
  fclose (fp);

  return resolve_addr_with_dladdr (pool, addr);
#undef RES_BUF_SIZE
}

static const char *
resolve_addr_in_symtab (pool pool, unsigned long addr,
			const char *filename, unsigned long low,
			unsigned long offset)
{
  FILE *fp;
#define RES_BUF_SIZE 2048
  char *symtabfile, *buffer, *symname, *lastsymname = "(none)";
  unsigned long lastsymaddr = 0, symaddr, diff;

  /* Look for a matching symtab file. */
  symtabfile = psprintf (pool, SYMTABSDIR "/%s.syms", filename);

  fp = fopen (symtabfile, "r");
  if (!fp) return resolve_addr_with_dladdr (pool, addr);

  buffer = pmalloc (pool, RES_BUF_SIZE);

  /* Read the symbols from the symtab file until one matches. */
  while (fgets (buffer, RES_BUF_SIZE, fp) != 0)
    {
      if (buffer[strlen(buffer)-1] == '\n')
	buffer[strlen(buffer)-1] = '\0';

      symname = strchr (buffer, ' ');
      if (!symname) continue;
      *symname = '\0';
      symname++;

      if (sscanf (buffer, "%lx", &symaddr) != 1) continue;

#if RES_DEBUG
      fprintf (stderr, "\t%s: %lx %s\n", symtabfile, symaddr, symname);
#endif

      /* Work out the in-memory address of this symbol. */
      symaddr += low - offset;

      /* If we've gone past the address, then the symbol must be the
       * previous symbol.
       */
      if (symaddr > addr)
	goto found_it;

      lastsymaddr = symaddr;
      lastsymname = pstrdup (pool, symname);
    }

 found_it:			/* It's the previous symbol. */
  fclose (fp);

  diff = addr - lastsymaddr;
  if (diff != 0)
    return psprintf (pool, "%s+0x%lx (%s)", lastsymname, diff, filename);
  else
    return psprintf (pool, "%s (%s)", lastsymname, filename);

#undef RES_BUF_SIZE
}

static const char *
resolve_addr_with_dladdr (pool pool, unsigned long addr)
{
#if defined(HAVE_DLFCN_H) && defined(HAVE_DLADDR)
  int r;
  Dl_info info;

  r = dladdr ((void *) addr, &info);
  if (r != 0 && info.dli_saddr != 0)
    {
      unsigned long real_addr = (unsigned long) info.dli_saddr;
      unsigned long diff = addr - real_addr;

      if (diff != 0)
	return psprintf (pool, "%s+0x%lx (%s)",
			 info.dli_sname, diff, info.dli_fname);
      else
	return psprintf (pool, "%s (%s)", info.dli_sname, info.dli_fname);
    }
  else
    return psprintf (pool, "%p", (void *) addr);
#else
  return psprintf (pool, "%p", (void *) addr);
#endif
}

/*----- End of symbol resolution code. -----*/
