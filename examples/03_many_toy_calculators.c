/* Toy calculator from example 02 turned into a reusable widget.
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
 * $Id: 03_many_toy_calculators.c,v 1.3 2002/09/07 13:46:58 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_table_layout.h"

#include "toy_calculator.h"

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  ml_window w;
  ml_table_layout tbl;
  toy_calculator calcs[4];

  /* Create the top-level window. */
  w = new_ml_window (session, pool);

  /* Create a table layout widget to arrange the calculators. */
  tbl = new_ml_table_layout (pool, 2, 2);

  /* Create the calculators and pack them into the table layout. */
  calcs[0] = new_toy_calculator (pool, session);
  ml_table_layout_pack (tbl, calcs[0], 0, 0);
  calcs[1] = new_toy_calculator (pool, session);
  ml_table_layout_pack (tbl, calcs[1], 0, 1);
  calcs[2] = new_toy_calculator (pool, session);
  ml_table_layout_pack (tbl, calcs[2], 1, 0);
  calcs[3] = new_toy_calculator (pool, session);
  ml_table_layout_pack (tbl, calcs[3], 1, 1);

  /* Pack the table into the window. */
  ml_window_pack (w, tbl);
}
