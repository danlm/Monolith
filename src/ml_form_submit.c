/* Monolith form submit button class.
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
 * $Id: ml_form_submit.c,v 1.4 2003/01/11 16:39:10 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_form_submit.h"

static void repaint (void *, ml_session, const char *, io_handle);
static void clear_value (void *);
static void set_value (void *, const char *value);
static const char *get_value (void *);

struct ml_widget_operations form_submit_ops =
  {
    repaint: repaint
  };

struct ml_form_input_operations form_submit_input_ops =
  {
    clear_value: clear_value,
    set_value: set_value,
    get_value: get_value
  };

struct ml_form_submit
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  const char *value;		/* Value of the input field. */
};

ml_form_submit
new_ml_form_submit (pool pool, ml_form form, const char *value)
{
  ml_form_submit w = pmalloc (pool, sizeof *w);

  w->ops = &form_submit_ops;
  w->fops = &form_submit_input_ops;
  w->pool = pool;
  w->value = value;

  /* Register ourselves with the form. */
  w->name = _ml_form_register_widget (form, w);

  return w;
}

static void
clear_value (void *vw)
{
  ml_form_submit w = (ml_form_submit) vw;

  w->value = 0;
}

static void
set_value (void *vw, const char *value)
{
  ml_form_submit w = (ml_form_submit) vw;

  w->value = value;
}

static const char *
get_value (void *vw)
{
  ml_form_submit w = (ml_form_submit) vw;

  return w->value;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_submit w = (ml_form_submit) vw;

  io_fprintf (io, "<input class=\"ml_form_submit\" "
	      "type=\"submit\" name=\"%s\" value=\"",
	      w->name);
  if (w->value) ml_plaintext_print (io, w->value);
  io_fputs ("\" />", io);
}
