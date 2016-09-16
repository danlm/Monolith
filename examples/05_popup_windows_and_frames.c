/* Demonstrate pop-up windows and frames.
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
 * $Id: 05_popup_windows_and_frames.c,v 1.6 2002/10/30 21:03:00 rich Exp $
 */

#include <stdio.h>
#include <math.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_button.h"
#include "ml_table_layout.h"
#include "ml_text_label.h"
#include "ml_form.h"
#include "ml_form_text.h"
#include "ml_form_submit.h"

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/* Private per-session data. */
struct data
{
  /* Main window. */
  ml_window main_win;
  ml_table_layout main_tbl;
  ml_text_label main_text;
  ml_button main_b[3];

  /* Area-of-circle window. */
  ml_window circle_win;
  ml_table_layout circle_tbl;
  ml_text_label circle_text1, circle_text2, circle_ans;
  ml_form circle_form;
  ml_form_text circle_input;
  ml_form_submit circle_submit;

  /* Volume-of-cone window. */
  ml_window cone_win;
  ml_table_layout cone_tbl;
  ml_text_label cone_text1, cone_text2, cone_text3, cone_ans;
  ml_form cone_form;
  ml_form_text cone_input1, cone_input2;
  ml_form_submit cone_submit;

  /* Frames window. */
  ml_window frames_win;
  ml_window frames_tl_win;
  ml_window frames_bl_win;
  ml_window frames_tr_win;
  ml_window frames_br_win;
  ml_text_label frames_tl_text;
  ml_text_label frames_bl_text;
  ml_text_label frames_tr_text;
  ml_text_label frames_br_text;
};

static void circle (ml_session session, void *vp);
static void circle_calculate (ml_session session, void *vp);
static void cone (ml_session session, void *vp);
static void cone_calculate (ml_session session, void *vp);
static void frames (ml_session session, void *vp);
static void tl_frame (ml_session session, void *vp);
static void bl_frame (ml_session session, void *vp);
static void tr_frame (ml_session session, void *vp);
static void br_frame (ml_session session, void *vp);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;

  /* Create the per-session data area. */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  data->main_win = new_ml_window (session, pool);
  data->main_tbl = new_ml_table_layout (pool, 2, 3);
  data->main_text = new_ml_text_label (pool, "Select a demo");

  /* Create some buttons to launch the different windows. */
  data->main_b[0] = new_ml_button (pool, "Area of a circle");
  ml_button_set_popup (data->main_b[0], "popupdemo_circle");
  ml_button_set_popup_size (data->main_b[0], 300, 200);
  ml_button_set_callback (data->main_b[0], circle, session, data);
  data->main_b[1] = new_ml_button (pool, "Volume of a cone");
  ml_button_set_popup (data->main_b[1], "popupdemo_cone");
  ml_button_set_popup_size (data->main_b[1], 300, 200);
  ml_button_set_callback (data->main_b[1], cone, session, data);
  data->main_b[2] = new_ml_button (pool, "Frames demo");
  ml_button_set_popup (data->main_b[2], "popupdemo_frames");
  ml_button_set_popup_size (data->main_b[2], 640, 480);
  ml_button_set_callback (data->main_b[2], frames, session, data);

  /* Pack everything up. */
  ml_table_layout_pack (data->main_tbl, data->main_text, 0, 0);
  ml_table_layout_set_colspan (data->main_tbl, 0, 0, 3);
  ml_table_layout_set_align (data->main_tbl, 0, 0, "center");
  ml_table_layout_pack (data->main_tbl, data->main_b[0], 1, 0);
  ml_table_layout_pack (data->main_tbl, data->main_b[1], 1, 1);
  ml_table_layout_pack (data->main_tbl, data->main_b[2], 1, 2);
  ml_window_pack (data->main_win, data->main_tbl);
}

static void
circle (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  /* Top-level window. */
  data->circle_win = new_ml_window (session, pool);

  /* Construct the form. */
  data->circle_form = new_ml_form (pool);
  data->circle_tbl = new_ml_table_layout (pool, 3, 2);
  data->circle_text1 = new_ml_text_label (pool, "Radius:");
  ml_table_layout_pack (data->circle_tbl, data->circle_text1, 0, 0);
  data->circle_input = new_ml_form_text (pool, data->circle_form);
  ml_table_layout_pack (data->circle_tbl, data->circle_input, 0, 1);
  data->circle_submit = new_ml_form_submit (pool, data->circle_form,
					    "Calculate");
  ml_table_layout_pack (data->circle_tbl, data->circle_submit, 1, 1);
  data->circle_text2 = new_ml_text_label (pool, "Answer:");
  ml_table_layout_pack (data->circle_tbl, data->circle_text2, 2, 0);
  data->circle_ans = new_ml_text_label (pool, "");
  ml_table_layout_pack (data->circle_tbl, data->circle_ans, 2, 1);

  ml_form_pack (data->circle_form, data->circle_tbl);
  ml_window_pack (data->circle_win, data->circle_form);

  /* Set the callback for the form. */
  ml_form_set_callback (data->circle_form, circle_calculate, session, data);
}

static void
circle_calculate (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  const char *r;
  double radius, area;

  r = ml_form_input_get_value (data->circle_input);
  if (r && sscanf (r, "%lf", &radius) == 1)
    {
      area = M_PI * radius * radius;
      ml_widget_set_property (data->circle_ans, "text",
			      psprintf (pool, "%g", area));
    }
  else
    {
      ml_widget_set_property (data->circle_ans, "text", "Not a number!");
    }
}

static void
cone (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  /* Top-level window. */
  data->cone_win = new_ml_window (session, pool);

  /* Construct the form. */
  data->cone_form = new_ml_form (pool);
  data->cone_tbl = new_ml_table_layout (pool, 4, 2);
  data->cone_text1 = new_ml_text_label (pool, "Radius:");
  ml_table_layout_pack (data->cone_tbl, data->cone_text1, 0, 0);
  data->cone_input1 = new_ml_form_text (pool, data->cone_form);
  ml_table_layout_pack (data->cone_tbl, data->cone_input1, 0, 1);
  data->cone_text3 = new_ml_text_label (pool, "Height:");
  ml_table_layout_pack (data->cone_tbl, data->cone_text3, 1, 0);
  data->cone_input2 = new_ml_form_text (pool, data->cone_form);
  ml_table_layout_pack (data->cone_tbl, data->cone_input2, 1, 1);
  data->cone_submit = new_ml_form_submit (pool, data->cone_form,
					  "Calculate");
  ml_table_layout_pack (data->cone_tbl, data->cone_submit, 2, 1);
  data->cone_text2 = new_ml_text_label (pool, "Answer:");
  ml_table_layout_pack (data->cone_tbl, data->cone_text2, 3, 0);
  data->cone_ans = new_ml_text_label (pool, "");
  ml_table_layout_pack (data->cone_tbl, data->cone_ans, 3, 1);

  ml_form_pack (data->cone_form, data->cone_tbl);
  ml_window_pack (data->cone_win, data->cone_form);

  /* Set the callback for the form. */
  ml_form_set_callback (data->cone_form, cone_calculate, session, data);
}

static void
cone_calculate (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  const char *r, *h;
  double radius, height, vol;

  r = ml_form_input_get_value (data->cone_input1);
  h = ml_form_input_get_value (data->cone_input2);
  if (r && sscanf (r, "%lf", &radius) == 1 &&
      h && sscanf (h, "%lf", &height) == 1)
    {
      vol = M_PI * radius * radius * height / 3.0;
      ml_widget_set_property (data->cone_ans, "text",
			      psprintf (pool, "%g", vol));
    }
  else
    {
      ml_widget_set_property (data->cone_ans, "text", "Not a number!");
    }
}

static void
frames (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  vector fs;
  struct ml_frame_description f;

  /* Create the top-level frameset. */
  fs = new_vector (pool, struct ml_frame_description);

  f.fn = tl_frame; f.data = data;
  vector_push_back (fs, f);

  f.fn = bl_frame; f.data = data;
  vector_push_back (fs, f);

  f.fn = tr_frame; f.data = data;
  vector_push_back (fs, f);

  f.fn = br_frame; f.data = data;
  vector_push_back (fs, f);

  data->frames_win = new_ml_frameset (session, pool, "60%,40%", "34%,66%", fs);
}

static void
tl_frame (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  data->frames_tl_win = new_ml_window (session, pool);
  data->frames_tl_text = new_ml_text_label (pool, "Top left frame");
  ml_window_pack (data->frames_tl_win, data->frames_tl_text);
}

static void
bl_frame (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  data->frames_bl_win = new_ml_window (session, pool);
  data->frames_bl_text = new_ml_text_label (pool, "Bottom left frame");
  ml_window_pack (data->frames_bl_win, data->frames_bl_text);
}

static void
tr_frame (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  data->frames_tr_win = new_ml_window (session, pool);
  data->frames_tr_text = new_ml_text_label (pool, "Top right frame");
  ml_window_pack (data->frames_tr_win, data->frames_tr_text);
}

static void
br_frame (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;

  data->frames_br_win = new_ml_window (session, pool);
  data->frames_br_text = new_ml_text_label (pool, "Bottom right frame");
  ml_window_pack (data->frames_br_win, data->frames_br_text);
}
