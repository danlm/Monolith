/* Monolith form checkbox input class.
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
 * $Id: ml_form_checkbox.c,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form_input.h"
#include "ml_form_checkbox.h"

static void repaint (void *, ml_session, const char *, io_handle);
static void clear_value (void *);
static void set_value (void *, const char *value);
static const char *get_value (void *);

struct ml_widget_operations form_checkbox_ops =
  {
    repaint: repaint
  };

struct ml_form_input_operations form_checkbox_input_ops =
  {
    clear_value: clear_value,
    set_value: set_value,
    get_value: get_value
  };

struct ml_form_checkbox
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  const char *value;		/* Value of the input field. */
};

ml_form_checkbox
new_ml_form_checkbox (pool pool, ml_form form)
{
  ml_form_checkbox w = pmalloc (pool, sizeof *w);

  w->ops = &form_checkbox_ops;
  w->fops = &form_checkbox_input_ops;
  w->pool = pool;
  w->value = 0;

  /* Register ourselves with the form. */
  w->name = _ml_form_register_widget (form, w);

  return w;
}

static void
clear_value (void *vw)
{
  ml_form_checkbox w = (ml_form_checkbox) vw;

  w->value = 0;
}

static void
set_value (void *vw, const char *value)
{
  ml_form_checkbox w = (ml_form_checkbox) vw;

  assert (value);
  w->value = "1";
}

static const char *
get_value (void *vw)
{
  ml_form_checkbox w = (ml_form_checkbox) vw;

  return w->value;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_checkbox w = (ml_form_checkbox) vw;

  io_fprintf (io, "<input class=\"ml_form_checkbox\" type=\"checkbox\" "
	      "name=\"%s\" value=\"1\""
	      "%s />",
	      w->name, w->value ? " checked=\"1\"" : "");
}
