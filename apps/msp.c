/* Monolith server-parsed pages (.msp's).
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
 * $Id: msp.c,v 1.7 2002/12/01 14:58:29 rich Exp $
 */

#include <pool.h>
#include <pstring.h>

#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_msp.h"

/*----- The following standard boilerplate code must appear -----*/

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/*----- End of standard boilerplate code -----*/

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  ml_window w;
  ml_msp msp;
  ml_dbh_factory dbf;
  const char *conninfo, *rootdir, *filename, *canonical_path;
  cgi args;

  canonical_path = ml_session_canonical_path (session);

  /* Create the top-level window. */
  w = new_ml_window (session, pool);

  /* Disable the window headers. We can assume that the .msp file will
   * supply <html>...</html>.
   */
  ml_window_set_headers_flag (w, 0);

  /* Nominate this window as the main window for this application. */
  ml_session_set_main_window (session, w);

  /* Get the document root. */
  rootdir = ml_cfg_get_string (session, "msp root", 0);
  if (rootdir == 0)
    {
      fprintf (stderr, "%s: no \"msp root\" directive in conf file.\n",
	       canonical_path);
      return;
    }

  /* Get the database connection name. */
  conninfo = ml_cfg_get_string (session, "msp database", 0);
  if (conninfo != 0)
    dbf = new_ml_dbh_factory (session, conninfo);
  else
    dbf = 0;			/* This is OK, but better not use it! */

  /* Get the filename. */
  args = ml_session_args (session);
  if (!args || (filename = cgi_param (args, "page")) == 0)
    {
      fprintf (stderr, "%s: missing page parameter.\n", canonical_path);
      return;
    }

  /* Create the msp widget. */
  msp = new_ml_msp (pool, session, dbf, rootdir, filename);
  if (!msp)
    {
      const char *msg =
	psprintf (pool,
		  "%s: failed to create msp widget (bad filename "
		  "or msp file contains errors?)",
		  canonical_path);

      /* Send some extra debug to the log file. */
      fprintf (stderr,
	       "%s\n"
	       "%s: rootdir = %s, filename = %s\n",
	       msg, canonical_path, rootdir, filename);

      /* Die with an error back to the browser. */
      pth_die (msg);
    }

  /* Pack the msp widget into the window. */
  ml_window_pack (w, msp);
}
