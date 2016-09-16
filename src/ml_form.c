/* Monolith form class.
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
 * $Id: ml_form.c,v 1.6 2002/11/23 16:46:05 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <vector.h>
#include <hash.h>
#include <pstring.h>

#include <pthr_iolib.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form_input.h"
#include "ml_form.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations form_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_form
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_widget w;			/* Packed widget. */
  const char *action_id;	/* Form's callback action. */
  const char *method;		/* Either "GET" or "POST". */
  const char *name;		/* Unique name of the form. */

  /* This is the user's real callback function. */
  void (*submit_fn) (ml_session, void *);
  void *submit_data;

  vector inputs;		/* List of input widgets. */
};

static struct ml_widget_property properties[] =
  {
    { name: "method",
      offset: ml_offsetof (struct ml_form, method),
      type: ML_PROP_STRING },
    { name: "form.name",
      offset: ml_offsetof (struct ml_form, name),
      type: ML_PROP_STRING,
      flags: ML_PROP_READ_ONLY },
    { 0 },
  };

struct form_input
{
  ml_form_input w;		/* Widget pointer. */
  const char *name;		/* Name of input widget. */
};

static void form_callback (ml_session, void *);

ml_form
new_ml_form (pool pool)
{
  static int unique = 0;
  ml_form w = pmalloc (pool, sizeof *w);

  w->ops = &form_ops;
  w->pool = pool;
  w->w = 0;
  w->action_id = 0;
  w->method = "POST";
  w->name = psprintf (pool, "ml_form%d", ++unique);
  w->submit_fn = 0;
  w->submit_data = 0;
  w->inputs = new_vector (pool, struct form_input);

  return w;
}

void
ml_form_set_callback (ml_form w, void (*callback_fn) (ml_session, void *),
		      ml_session session, void *data)
{
  if (w->action_id)
    ml_unregister_action (session, w->action_id);
  w->action_id = 0;
  w->submit_fn = 0;
  w->submit_data = 0;

  if (callback_fn)
    {
      w->action_id = ml_register_action (session, form_callback, w);
      w->submit_fn = callback_fn;
      w->submit_data = data;
    }
}

void
ml_form_pack (ml_form w, ml_widget _w)
{
  w->w = _w;
}

/* We capture the callback function when the submit button is pressed
 * and allow ourselves to update the values fields of all our input
 * widgets first.
 */
static void
form_callback (ml_session session, void *vw)
{
  pool session_pool = ml_session_pool (session);
  ml_form w = (ml_form) vw;
  cgi args;
  int i;

  /* Get the arguments passed to this HTTP request. */
  args = _ml_session_submitted_args (session);

  /* Update the values of submitted form elements. */
  for (i = 0; i < vector_size (w->inputs); ++i)
    {
      struct form_input input;
      vector values;
      const char *value;
      int j;

      vector_get (w->inputs, i, input);
      values = cgi_param_list (args, input.name);

      /* Clear value first. */
      ml_form_input_clear_value (input.w);

      /* Set value(s). */
      if (values)
	for (j = 0; j < vector_size (values); ++j)
	  {
	    vector_get (values, j, value);
	    ml_form_input_set_value (input.w, pstrdup (session_pool, value));
	  }
    }

  /* Call the user's real callback function. */
  assert (w->submit_fn != 0);
  w->submit_fn (session, w->submit_data);
}

const char *
_ml_form_register_widget (ml_form f, ml_form_input w)
{
  struct form_input input;
  static int name_id = 0;

  input.name = psprintf (f->pool, "ml_f%d", ++name_id);
  input.w = w;

  vector_push_back (f->inputs, input);

  return input.name;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form w = (ml_form) vw;

  if (w->w)
    {
      if (w->action_id)
	io_fprintf (io,
		    "<form method=\"%s\" action=\"%s\" name=\"%s\">"
		    "<input type=\"hidden\" name=\"ml_window\" value=\"%s\" />"
		    "<input type=\"hidden\" name=\"ml_action\" value=\"%s\" />"
		    ,
		    w->method,
		    ml_session_script_name (session),
		    w->name,
		    windowid,
		    w->action_id);
      else
	io_fputs ("<form>", io);
      ml_widget_repaint (w->w, session, windowid, io);
      io_fputs ("</form>", io);
    }
}
