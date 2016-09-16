/* Toggle buttons.
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
 * $Id: 07_toggle_buttons.c,v 1.2 2002/10/30 21:03:01 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_toggle_button.h"
#include "ml_table_layout.h"
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
  /* The toggle buttons. */
  ml_toggle_button b[4];

  ml_text_label text;
};

static void update_text (ml_session, void *);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;
  ml_window win;
  ml_table_layout tbl;

  /* Create the private, per-session data area and save it in the
   * session object.
   */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  win = new_ml_window (session, pool);

  /* Create the table layout. */
  tbl = new_ml_table_layout (pool, 2, 4);

  /* Create the toggle buttons. */
  data->b[0] = new_ml_toggle_button (pool, session, "A");
  data->b[1] = new_ml_toggle_button (pool, session, "B");
  data->b[2] = new_ml_toggle_button (pool, session, "C");
  data->b[3] = new_ml_toggle_button (pool, session, "D");

  /* Create the status label. */
  data->text = new_ml_text_label (pool, "(press a button)");

  /* Set the callback on the buttons to a function which will
   * update the label.
   */
  ml_toggle_button_set_callback (data->b[0], update_text, session, data);
  ml_toggle_button_set_callback (data->b[1], update_text, session, data);
  ml_toggle_button_set_callback (data->b[2], update_text, session, data);
  ml_toggle_button_set_callback (data->b[3], update_text, session, data);

  /* Pack everything together. */
  ml_table_layout_pack (tbl, data->b[0], 0, 0);
  ml_table_layout_pack (tbl, data->b[1], 0, 1);
  ml_table_layout_pack (tbl, data->b[2], 0, 2);
  ml_table_layout_pack (tbl, data->b[3], 0, 3);
  ml_table_layout_pack (tbl, data->text, 1, 0);
  ml_table_layout_set_colspan (tbl, 1, 0, 4);
  ml_window_pack (win, tbl);
}

static void
update_text (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  int a, b, c, d;

  /* Get the state of the four buttons. */
  a = ml_toggle_button_get_state (data->b[0]);
  b = ml_toggle_button_get_state (data->b[1]);
  c = ml_toggle_button_get_state (data->b[2]);
  d = ml_toggle_button_get_state (data->b[3]);

  ml_widget_set_property
    (data->text, "text",
     psprintf (pool,
	       "Button A: %s\n"
	       "Button B: %s\n"
	       "Button C: %s\n"
	       "Button D: %s\n",
	       a ? "pressed" : "not pressed",
	       b ? "pressed" : "not pressed",
	       c ? "pressed" : "not pressed",
	       d ? "pressed" : "not pressed"));
}
