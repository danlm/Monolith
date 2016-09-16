/* Monolith form text input class.
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
 * $Id: ml_form_text.c,v 1.5 2003/01/11 16:39:10 rich Exp $
 */

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_form_input.h"
#include "ml_form_text.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];
static void clear_value (void *);
static void set_value (void *, const char *value);
static const char *get_value (void *);

struct ml_widget_operations form_text_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_form_input_operations form_text_input_ops =
  {
    clear_value: clear_value,
    set_value: set_value,
    get_value: get_value
  };

struct ml_form_text
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
  pool pool;			/* Pool for allocations. */
  const char *name;		/* Name of the input field. */
  const char *value;		/* Value of the input field. */
  int size;			/* Displayed width. */
  int maxlength;		/* Maximum number of characters. */
  ml_form form;			/* The form. */
  int focus;			/* If set, grab focus. */
};

static struct ml_widget_property properties[] =
  {
    { name: "form.text.size",
      offset: ml_offsetof (struct ml_form_text, size),
      type: ML_PROP_INT },
    { name: "form.text.maxlength",
      offset: ml_offsetof (struct ml_form_text, maxlength),
      type: ML_PROP_INT },
    { 0 }
  };

ml_form_text
new_ml_form_text (pool pool, ml_form form)
{
  ml_form_text w = pmalloc (pool, sizeof *w);

  w->ops = &form_text_ops;
  w->fops = &form_text_input_ops;
  w->pool = pool;
  w->value = 0;
  w->size = w->maxlength = -1;
  w->form = form;
  w->focus = 0;

  /* Register ourselves with the form. */
  w->name = _ml_form_register_widget (form, w);

  return w;
}

void
ml_form_text_focus (ml_form_text w)
{
  w->focus = 1;
}

static void
clear_value (void *vw)
{
  ml_form_text w = (ml_form_text) vw;

  w->value = 0;
}

static void
set_value (void *vw, const char *value)
{
  ml_form_text w = (ml_form_text) vw;

  assert (value);
  w->value = value;
}

static const char *
get_value (void *vw)
{
  ml_form_text w = (ml_form_text) vw;

  return w->value;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_text w = (ml_form_text) vw;

  io_fprintf (io, "<input class=\"ml_form_text\" name=\"%s\" value=\"",
	      w->name);
  if (w->value) ml_plaintext_print (io, w->value);
  io_fputc ('"', io);
  if (w->size >= 0)
    io_fprintf (io, " size=\"%d\"", w->size);
  if (w->maxlength >= 0)
    io_fprintf (io, " maxlength=\"%d\"", w->maxlength);
  io_fputs (" />", io);

  /* XXX It is quite likely this won't work. It looks like we need to
   * provide an onLoad function in the window to do this reliably.
   */
  if (w->focus)
    {
      const char *form_name;

      ml_widget_get_property (w->form, "form.name", form_name);

      io_fprintf (io, "<script language=\"javascript\"><!--\n"
		  "  document.%s.%s.focus ();\n"
		  "//--></script>\n",
		  form_name, w->name);
    }
}
