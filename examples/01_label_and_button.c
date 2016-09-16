/* A very simple monolith program.
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
 * $Id: 01_label_and_button.c,v 1.8 2002/11/02 18:53:46 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_flow_layout.h"
#include "ml_label.h"
#include "ml_button.h"

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
  ml_label lb;			/* Label. */
  int count;			/* Count of number of button presses. */
};

static void increment (ml_session, void *);
static void update_label (pool pool, ml_label lb, int count);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;
  ml_window w;
  ml_flow_layout lay;
  ml_label lb;
  ml_button b;

  /* Create the private, per-session data area. */
  data = pmalloc (pool, sizeof *data);
  data->count = 0;

  /* Create the top-level window. */
  w = new_ml_window (session, pool);

  /* Create the flow layout widget which will be packed into the window. */
  lay = new_ml_flow_layout (pool);

  /* Create the label and button. */
  data->lb = lb = new_ml_label (pool, 0);
  update_label (pool, data->lb, 0);

  b = new_ml_button (pool, "Push me!");
  ml_button_set_callback (b, increment, session, data);

  /* Pack the label and button into the flow layout widget. */
  ml_flow_layout_pack (lay, lb);
  ml_flow_layout_pack (lay, b);

  /* Pack the flow layout widget into the window. */
  ml_window_pack (w, lay);
}

/* This callback is called whenever the button is pressed. */
static void
increment (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  update_label (ml_session_pool (session), data->lb, ++data->count);
}

/* This function just updates the label, once from the main function, and
 * again each time we press the button.
 */
static void
update_label (pool pool, ml_label lb, int count)
{
  ml_widget_set_property (lb, "text",
			  psprintf (pool,
				    "Button pressed: <b>%d</b><br>",
				    count));
}
