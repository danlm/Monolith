/* Monolith iframe class.
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
 * $Id: ml_iframe.c,v 1.1 2002/11/08 23:19:14 rich Exp $
 */

#include "config.h"

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_iframe.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations iframe_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_iframe
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *action_id;	/* Action. */
  ml_widget non_frame_widget;	/* Alternative for non-frames browsers. */
  int width, height;		/* Width, height. */
  const char *clazz;		/* Class. */
  const char *scrolling;	/* "yes", "no" or "auto". */
};

static struct ml_widget_property properties[] =
  {
    { name: "width",
      offset: ml_offsetof (struct ml_iframe, width),
      type: ML_PROP_INT },
    { name: "height",
      offset: ml_offsetof (struct ml_iframe, height),
      type: ML_PROP_INT },
    { name: "class",
      offset: ml_offsetof (struct ml_iframe, clazz),
      type: ML_PROP_STRING },
    { name: "scrolling",
      offset: ml_offsetof (struct ml_iframe, scrolling),
      type: ML_PROP_STRING },
    { 0 }
  };

ml_iframe
new_ml_iframe (pool pool, void (*fn) (ml_session, void *),
	       ml_session session, void *data,
	       ml_widget non_frame_widget)
{
  ml_iframe w = pmalloc (pool, sizeof *w);

  w->ops = &iframe_ops;
  w->pool = pool;
  w->clazz = 0;
  w->scrolling = 0;
  w->width = w->height = 0;
  w->non_frame_widget = non_frame_widget;

  /* Register the callback. */
  w->action_id = ml_register_action (session, fn, data);

  return w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_iframe w = (ml_iframe) vw;

  io_fprintf (io, "<iframe src=\"%s?ml_action=%s&ml_window=%s\"",
	      ml_session_script_name (session), w->action_id, windowid);

  if (w->clazz)
    io_fprintf (io, " class=\"%s\"", w->clazz);
  if (w->width)
    io_fprintf (io, " width=\"%d\"", w->width);
  if (w->height)
    io_fprintf (io, " height=\"%d\"", w->height);
  if (w->scrolling)
    io_fprintf (io, " scrolling=\"%s\"", w->scrolling);
  io_fputs (">", io);

  if (w->non_frame_widget)
    ml_widget_repaint (w->non_frame_widget, session, windowid, io);
  else
    io_fputs ("Your browser does not support frames.", io);

  io_fputs ("</iframe>", io);
}
