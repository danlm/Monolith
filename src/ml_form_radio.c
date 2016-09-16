/* Monolith form radio button class.
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
 * $Id: ml_form_radio.c,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form_input.h"
#include "ml_form_radio.h"

static void repaint (void *, ml_session, const char *, io_handle);
static const char *get_value (void *);

struct ml_widget_operations form_radio_ops =
  {
    repaint: repaint
  };

struct ml_form_input_operations form_radio_input_ops =
  {
    clear_value: 0,
    set_value: 0,
    get_value: get_value
  };

struct ml_form_radio
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  const char *value;		/* Value of the input field. */
  int is_checked;		/* Radio button checked? */
};

ml_form_radio
new_ml_form_radio (pool pool, ml_form_radio_group group, const char *value)
{
  ml_form_radio w = pmalloc (pool, sizeof *w);

  assert (value);

  w->ops = &form_radio_ops;
  w->fops = &form_radio_input_ops;
  w->pool = pool;
  w->value = value;
  w->is_checked = 0;

  /* Register ourselves with the radio button group widget. */
  w->name = _ml_form_radio_group_register (group, w, value);

  return w;
}

void
ml_form_radio_set_checked (ml_form_radio w, int checked)
{
  w->is_checked = checked;
}

int
ml_form_radio_get_checked (ml_form_radio w)
{
  return w->is_checked;
}

static const char *
get_value (void *vw)
{
  ml_form_radio w = (ml_form_radio) vw;

  return w->value;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_radio w = (ml_form_radio) vw;

  io_fprintf (io, "<input class=\"ml_form_radio\" type=\"radio\" "
	      "name=\"%s\" value=\"%s\""
	      "%s />",
	      w->name, w->value,
	      w->is_checked ? " checked=\"1\"" : "");
}
