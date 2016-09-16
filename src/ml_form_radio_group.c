/* Monolith group of radio buttons.
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
 * $Id: ml_form_radio_group.c,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pool.h>
#include <hash.h>

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form.h"
#include "ml_form_input.h"
#include "ml_form_radio.h"
#include "ml_form_radio_group.h"

static void repaint (void *, ml_session, const char *, io_handle);
static void clear_value (void *);
static void set_value (void *, const char *value);
static const char *get_value (void *);

struct ml_widget_operations form_radio_group_ops =
  {
    repaint: repaint
  };

struct ml_form_input_operations form_radio_group_input_ops =
  {
    clear_value: clear_value,
    set_value: set_value,
    get_value: get_value
  };

struct ml_form_radio_group
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  const char *value;		/* Value of the input field. */
  ml_widget w;			/* Packed widget. */
  shash buttons;		/* Related buttons (value -> radio button). */
};

ml_form_radio_group
new_ml_form_radio_group (pool pool, ml_form form)
{
  ml_form_radio_group w = pmalloc (pool, sizeof *w);

  w->ops = &form_radio_group_ops;
  w->fops = &form_radio_group_input_ops;
  w->pool = pool;
  w->value = 0;
  w->w = 0;
  w->buttons = new_shash (pool, ml_form_radio);

  /* Register ourselves with the form. */
  w->name = _ml_form_register_widget (form, w);

  return w;
}

const char *
_ml_form_radio_group_register (ml_form_radio_group w, ml_form_radio r,
			       const char *value)
{
  shash_insert (w->buttons, value, r);
  return w->name;
}

void
ml_form_radio_group_pack (ml_form_radio_group w, ml_widget _w)
{
  w->w = _w;
}

static void
clear_value (void *vw)
{
  ml_form_radio_group w = (ml_form_radio_group) vw;
  int i;
  vector values;
  ml_form_radio r;

  /* Uncheck all of the buttons. */
  values = shash_values (w->buttons);
  for (i = 0; i < vector_size (values); ++i)
    {
      vector_get (values, i, r);
      ml_form_radio_set_checked (r, 0);
    }
  w->value = 0;
}

static void
set_value (void *vw, const char *value)
{
  ml_form_radio_group w = (ml_form_radio_group) vw;
  ml_form_radio r;

  assert (value);
  if (shash_get (w->buttons, value, r))
    ml_form_radio_set_checked (r, 1);
  w->value = value;
}

static const char *
get_value (void *vw)
{
  ml_form_radio_group w = (ml_form_radio_group) vw;

  return w->value;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_radio_group w = (ml_form_radio_group) vw;

  if (w->w)
    ml_widget_repaint (w->w, session, windowid, io);
}
