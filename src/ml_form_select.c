/* Monolith form select box input class.
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
 * $Id: ml_form_select.c,v 1.4 2002/11/13 20:46:37 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form_input.h"
#include "ml_form_select.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];
static void set_value (void *, const char *value);
static void clear_value (void *);

struct ml_widget_operations form_select_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_form_input_operations form_select_input_ops =
  {
    set_value: set_value,
    get_value: 0,
    clear_value: clear_value
  };

struct ml_form_select
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  int size;			/* Size property. */
  int multiple;			/* Multiple property. */
  vector options;		/* Options (vector of strings). */
  vector selections;		/* Selected options (vector of int). */
  int selected;			/* Single selection. */
};

static struct ml_widget_property properties[] =
  {
    { name: "form.select.size",
      offset: ml_offsetof (struct ml_form_select, size),
      type: ML_PROP_INT },
    { name: "form.select.multiple",
      offset: ml_offsetof (struct ml_form_select, multiple),
      type: ML_PROP_BOOL },
    { 0 }
  };

ml_form_select
new_ml_form_select (pool pool, ml_form form)
{
  ml_form_select w = pmalloc (pool, sizeof *w);

  w->ops = &form_select_ops;
  w->fops = &form_select_input_ops;
  w->pool = pool;
  w->size = 0;
  w->multiple = 0;
  w->options = new_vector (pool, const char *);
  w->selections = 0;
  w->selected = -1;

  /* Register ourselves with the form. */
  w->name = _ml_form_register_widget (form, w);

  return w;
}

void
ml_form_select_push_back (ml_form_select w, const char *option)
{
  vector_push_back (w->options, option);
}

const char *
ml_form_select_pop_back (ml_form_select w)
{
  const char *option;

  vector_pop_back (w->options, option);
  return option;
}

void
ml_form_select_push_front (ml_form_select w, const char *option)
{
  vector_push_front (w->options, option);
}

const char *
ml_form_select_pop_front (ml_form_select w)
{
  const char *option;

  vector_pop_front (w->options, option);
  return option;
}

const char *
ml_form_select_get (ml_form_select w, int option_index)
{
  const char *option;

  vector_get (w->options, option_index, option);
  return option;
}

void
ml_form_select_insert (ml_form_select w, int option_index, const char *option)
{
  vector_insert (w->options, option_index, option);
}

void
ml_form_select_replace (ml_form_select w, int option_index, const char *option)
{
  vector_replace (w->options, option_index, option);
}

void
ml_form_select_erase (ml_form_select w, int option_index)
{
  vector_erase (w->options, option_index);
}

void
ml_form_select_clear (ml_form_select w)
{
  vector_clear (w->options);
}

int
ml_form_select_size (ml_form_select w)
{
  return vector_size (w->options);
}

void
ml_form_select_set_selection (ml_form_select w, int option_index)
{
  if (!w->multiple)
    w->selected = option_index;
  else
    abort ();
}

void
ml_form_select_set_selections (ml_form_select w, vector selected)
{
  if (w->multiple)
    w->selections = selected;
  else
    abort ();
}

int
ml_form_select_get_selection (ml_form_select w)
{
  if (!w->multiple)
    return w->selected;
  else
    abort ();
}

const vector
ml_form_select_get_selections (ml_form_select w)
{
  if (w->multiple)
    return w->selections;
  else
    abort ();
}

static void
clear_value (void *vw)
{
  ml_form_select w = (ml_form_select) vw;

  if (!w->multiple)
    {
      w->selected = -1;
    }
  else
    {
      int zero = 0;

      w->selections = new_vector (w->pool, int);
      vector_fill (w->selections, zero, vector_size (w->options));
    }
}

static void
set_value (void *vw, const char *value)
{
  ml_form_select w = (ml_form_select) vw;

  sscanf (value, "%d", &w->selected);

  if (w->multiple && w->selections &&
      w->selected >= 0 && w->selected < vector_size (w->selections))
    {
      int one = 1;

      vector_replace (w->selections, w->selected, one);
    }
}

static inline int
is_selected (ml_form_select w, int index)
{
  if (!w->multiple)
    return index == w->selected;
  else
    {
      if (w->selections && index < vector_size (w->selections))
	{
	  int i;

	  vector_get (w->selections, index, i);
	  return i;
	}
      else
	return 0;
    }
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_select w = (ml_form_select) vw;
  int i;
  const char *option;

  io_fprintf (io, "<select class=\"ml_form_select\" name=\"%s\"", w->name);
  if (w->size) io_fprintf (io, " size=\"%d\"", w->size);
  if (w->multiple) io_fprintf (io, " multiple=\"1\"");
  io_fprintf (io, ">");

  for (i = 0; i < vector_size (w->options); ++i)
    {
      vector_get (w->options, i, option);

      io_fprintf (io, "<option value=\"%d\"", i);
      if (is_selected (w, i))
	io_fprintf (io, " selected=\"1\"");
      io_fprintf (io, ">%s</option>\n", option);
    }

  io_fprintf (io, "</select>");
}
