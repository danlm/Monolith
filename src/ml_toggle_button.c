/* Monolith toggle button class.
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
 * $Id: ml_toggle_button.c,v 1.4 2002/11/07 10:49:02 rich Exp $
 */

#include "config.h"

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_toggle_button.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations toggle_button_ops =
  {
    repaint: repaint,
    properties: properties
  };

struct ml_toggle_button
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *text;		/* HTML printed for the button. */
  int state;			/* 0 = out, non-zero = pushed in */
  const char *action_id;	/* Action. */
  void (*fn) (ml_session, void *); /* Callback. */
  void *data;
  int is_key;			/* Key button property. */
};

static struct ml_widget_property properties[] =
  {
    { name: "text",
      offset: ml_offsetof (struct ml_toggle_button, text),
      type: ML_PROP_STRING },
    { name: "button.key",	/* XXX Replace with button.style. */
      offset: ml_offsetof (struct ml_toggle_button, is_key),
      type: ML_PROP_BOOL },
    { 0 }
  };

static void toggle_me (ml_session session, void *vp);

ml_toggle_button
new_ml_toggle_button (pool pool, ml_session session, const char *text)
{
  ml_toggle_button w = pmalloc (pool, sizeof *w);

  w->ops = &toggle_button_ops;
  w->pool = pool;
  w->text = text;
  w->state = 0;
  w->is_key = 0;
  w->fn = 0;
  w->data = 0;

  /* Register the action for this toggle button. */
  w->action_id = ml_register_action (session, toggle_me, w);

  return w;
}

/* This function is called whenever the button is toggled. */
static void
toggle_me (ml_session session, void *vp)
{
  ml_toggle_button w = (ml_toggle_button) vp;

  w->state = !w->state;

  if (w->fn)			/* Call the user's callback function. */
    w->fn (session, w->data);
}

void
ml_toggle_button_set_callback (ml_toggle_button w,
			       void (*fn)(ml_session, void *),
			       ml_session session, void *data)
{
  w->fn = fn;
  w->data = data;
}

void
ml_toggle_button_set_state (ml_toggle_button w, int state)
{
  w->state = state;
}

int
ml_toggle_button_get_state (ml_toggle_button w)
{
  return w->state;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_toggle_button w = (ml_toggle_button) vw;
  pool pool = ml_session_pool (session);

  if (w->text)
    {
      const char *clazz, *link;

      if (!w->is_key)
	{
	  if (!w->state)
	    clazz = "ml_toggle_button";
	  else
	    clazz = "ml_toggle_button_pressed";
	}
      else
	{
	  if (!w->state)
	    clazz = "ml_toggle_button_key";
	  else
	    clazz = "ml_toggle_button_key_pressed";
	}

      link = psprintf (pool, "%s?ml_action=%s&ml_window=%s",
		       ml_session_script_name (session),
		       w->action_id,
		       windowid);

      io_fprintf (io, "<a class=\"%s\" href=\"%s\">%s</a>",
		  clazz, link, w->text);
    }
}
