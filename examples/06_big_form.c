/* Big, complicated form.
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
 * $Id: 06_big_form.c,v 1.4 2002/11/02 18:53:46 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_text_label.h"
#include "ml_button.h"
#include "ml_table_layout.h"
#include "ml_flow_layout.h"
#include "ml_form.h"
#include "ml_form_submit.h"
#include "ml_form_text.h"
#include "ml_form_textarea.h"
#include "ml_form_password.h"
#include "ml_form_select.h"
#include "ml_form_radio_group.h"
#include "ml_form_radio.h"
#include "ml_form_checkbox.h"
#include "ml_form_textarea.h"

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
  ml_window win;

  /* The form input fields themselves. */
  ml_form_text familyname, givenname;
  ml_form_password password;
  ml_form_select dd, mm, yyyy;	/* Date of birth. */
  ml_form_radio_group gender;
  ml_form_radio m, f;		/* Gender. */
  ml_form_checkbox eating, drinking, sleeping; /* Interests */
  /*ml_form_file photo;            File upload, not yet implemented. */
  ml_form_select dept;
  ml_form_textarea comments;
};

static void create_form (ml_session session, void *vp);
static void submit_form (ml_session session, void *vp);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;

  /* Create the private, per-session data area and save it in the
   * session object.
   */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  data->win = new_ml_window (session, pool);

  create_form (session, data);
}

static void
create_form (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  ml_form form;
  ml_table_layout tbl;
  ml_text_label text;
  ml_form_submit submit;
  ml_flow_layout flow;
  int i;

  /* Create the form. */
  form = new_ml_form (pool);
  ml_form_set_callback (form, submit_form, session, data);

  /* Create the table. */
  tbl = new_ml_table_layout (pool, 10, 2);

  /* Create the contents of the form. */
  text = new_ml_text_label (pool, "Family name / surname");
  ml_table_layout_pack (tbl, text, 0, 0);
  data->familyname = new_ml_form_text (pool, form);
  ml_table_layout_pack (tbl, data->familyname, 0, 1);

  text = new_ml_text_label (pool, "Given name(s) / forename(s)");
  ml_table_layout_pack (tbl, text, 1, 0);
  data->givenname = new_ml_form_text (pool, form);
  ml_table_layout_pack (tbl, data->givenname, 1, 1);

  text = new_ml_text_label (pool, "Password");
  ml_table_layout_pack (tbl, text, 2, 0);
  data->password = new_ml_form_password (pool, form);
  ml_table_layout_pack (tbl, data->password, 2, 1);

  text = new_ml_text_label (pool, "Date of birth");
  ml_table_layout_pack (tbl, text, 3, 0);
  flow = new_ml_flow_layout (pool);
  data->dd = new_ml_form_select (pool, form);
  for (i = 1; i <= 31; ++i)
    ml_form_select_push_back (data->dd, psprintf (pool, "%02d", i));
  data->mm = new_ml_form_select (pool, form);
  ml_form_select_push_back (data->mm, "Jan");
  ml_form_select_push_back (data->mm, "Feb");
  ml_form_select_push_back (data->mm, "Mar");
  ml_form_select_push_back (data->mm, "Apr");
  ml_form_select_push_back (data->mm, "May");
  ml_form_select_push_back (data->mm, "Jun");
  ml_form_select_push_back (data->mm, "Jul");
  ml_form_select_push_back (data->mm, "Aug");
  ml_form_select_push_back (data->mm, "Sep");
  ml_form_select_push_back (data->mm, "Oct");
  ml_form_select_push_back (data->mm, "Nov");
  ml_form_select_push_back (data->mm, "Dec");
  data->yyyy = new_ml_form_select (pool, form);
  for (i = 1900; i <= 2002; ++i)
    ml_form_select_push_back (data->yyyy, psprintf (pool, "%04d", i));

  ml_flow_layout_pack (flow, data->dd);
  ml_flow_layout_pack (flow, data->mm);
  ml_flow_layout_pack (flow, data->yyyy);

  ml_table_layout_pack (tbl, flow, 3, 1);

  text = new_ml_text_label (pool, "Gender");
  ml_table_layout_pack (tbl, text, 4, 0);
  data->gender = new_ml_form_radio_group (pool, form);

  flow = new_ml_flow_layout (pool);
  data->m = new_ml_form_radio (pool, data->gender, "m");
  ml_form_radio_set_checked (data->m, 1);
  ml_flow_layout_pack (flow, data->m);
  text = new_ml_text_label (pool, "Male");
  ml_flow_layout_pack (flow, text);
  data->f = new_ml_form_radio (pool, data->gender, "f");
  ml_flow_layout_pack (flow, data->f);
  text = new_ml_text_label (pool, "Female");
  ml_flow_layout_pack (flow, text);

  ml_form_radio_group_pack (data->gender, flow);
  ml_table_layout_pack (tbl, data->gender, 4, 1);

  text = new_ml_text_label (pool, "Interests");
  ml_table_layout_pack (tbl, text, 5, 0);

  flow = new_ml_flow_layout (pool);
  data->eating = new_ml_form_checkbox (pool, form);
  ml_flow_layout_pack (flow, data->eating);
  text = new_ml_text_label (pool, "Eating");
  ml_flow_layout_pack (flow, text);
  data->drinking = new_ml_form_checkbox (pool, form);
  ml_flow_layout_pack (flow, data->drinking);
  text = new_ml_text_label (pool, "Drinking");
  ml_flow_layout_pack (flow, text);
  data->sleeping = new_ml_form_checkbox (pool, form);
  ml_flow_layout_pack (flow, data->sleeping);
  text = new_ml_text_label (pool, "Sleeping");
  ml_flow_layout_pack (flow, text);

  ml_table_layout_pack (tbl, flow, 5, 1);

  text = new_ml_text_label (pool, "Department");
  ml_table_layout_pack (tbl, text, 6, 0);
  data->dept = new_ml_form_select (pool, form);
  ml_widget_set_property (data->dept, "form.select.size", 4);
  ml_widget_set_property (data->dept, "form.select.multiple", 1);
  ml_form_select_push_back (data->dept, "Accounting");
  ml_form_select_push_back (data->dept, "Development");
  ml_form_select_push_back (data->dept, "Marketing");
  ml_form_select_push_back (data->dept, "Personnel");
  ml_form_select_push_back (data->dept, "Support");
  ml_form_select_push_back (data->dept, "Systems");
  ml_table_layout_pack (tbl, data->dept, 6, 1);

  text = new_ml_text_label (pool, "Comments");
  ml_table_layout_pack (tbl, text, 7, 0);
  data->comments = new_ml_form_textarea (pool, form, 5, 50);
  ml_table_layout_pack (tbl, data->comments, 7, 1);

  text = new_ml_text_label (pool, "Photo");
  ml_table_layout_pack (tbl, text, 8, 0);
  text = new_ml_text_label (pool, "[not implemented]");
  ml_table_layout_pack (tbl, text, 8, 1);

  /* Submit button. */
  submit = new_ml_form_submit (pool, form, "Submit");
  ml_table_layout_pack (tbl, submit, 9, 1);

  /* Pack it all up. */
  ml_form_pack (form, tbl);
  ml_window_pack (data->win, form);
}

static void
submit_form (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  ml_text_label text;
  ml_table_layout tbl;
  ml_button button;
  const char *str;

  str = psprintf
    (pool,
     "Form submitted.\n"
     "\n"
     "Family name: %s\n"
     "Given name: %s\n"
     "Password: %s\n"
     "Date of birth: dd = %d, mm = %d, yyyy = %d\n"
     "Gender: %s\n"
     "   M is checked: %d  F is checked: %d\n"
     "Interests: Eating = %d, Drinking = %d, Sleeping = %d\n"
     "Dept fields checked: [ %s ]\n"
     "Comments:\n"
     "--start--\n"
     "%s\n"
     "--end--\n",
     ml_form_input_get_value (data->familyname),
     ml_form_input_get_value (data->givenname),
     ml_form_input_get_value (data->password),
     1 + ml_form_select_get_selection (data->dd),
     1 + ml_form_select_get_selection (data->mm),
     1900 + ml_form_select_get_selection (data->yyyy),
     ml_form_input_get_value (data->gender),
     ml_form_radio_get_checked (data->m),
     ml_form_radio_get_checked (data->f),
     ml_form_input_get_value (data->eating) ? 1 : 0,
     ml_form_input_get_value (data->drinking) ? 1 : 0,
     ml_form_input_get_value (data->sleeping) ? 1 : 0,
     pjoin (pool,
	    pvitostr (pool, ml_form_select_get_selections (data->dept)), ", "),
     ml_form_input_get_value (data->comments));

  tbl = new_ml_table_layout (pool, 2, 1);
  text = new_ml_text_label (pool, str);
  ml_table_layout_pack (tbl, text, 0, 0);
  button = new_ml_button (pool, "Back to form");
  ml_button_set_callback (button, create_form, session, data);
  ml_table_layout_pack (tbl, button, 1, 0);

  ml_window_pack (data->win, tbl);
}
