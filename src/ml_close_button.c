/* Monolith close button class.
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
 * $Id: ml_close_button.c,v 1.3 2002/11/13 21:41:01 rich Exp $
 */

#include "config.h"

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_close_button.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations close_button_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_close_button
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *text;		/* HTML printed for the button. */
  int reload_opener;		/* If set, reload the opener window. */
};

static struct ml_widget_property properties[] =
  {
    { name: "text",
      offset: ml_offsetof (struct ml_close_button, text),
      type: ML_PROP_STRING },
    { name: "button.reload-opener",
      offset: ml_offsetof (struct ml_close_button, reload_opener),
      type: ML_PROP_BOOL },
    { 0 }
  };

ml_close_button
new_ml_close_button (pool pool, const char *text)
{
  ml_close_button w = pmalloc (pool, sizeof *w);

  w->ops = &close_button_ops;
  w->pool = pool;
  w->text = text;
  w->reload_opener = 0;

  return w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_close_button w = (ml_close_button) vw;

  if (w->text)
    {
      const char *js;

      if (!w->reload_opener)
	js = "top.close()";
      else
	js = "window.opener.location.reload(); top.close()";

      io_fprintf (io,
		  "<a class=\"ml_button\" href=\"javascript:%s\">%s</a>",
		  js, w->text);
    }
}
