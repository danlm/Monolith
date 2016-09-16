/* Monolith image class.
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
 * $Id: ml_image.c,v 1.2 2002/10/30 21:03:03 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_image.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations image_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_image
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *src;		/* Source for the image. */
};

static struct ml_widget_property properties[] =
  {
    { name: "image",
      offset: ml_offsetof (struct ml_image, src),
      type: ML_PROP_STRING },
    { 0 }
  };

ml_image
new_ml_image (pool pool, const char *src)
{
  ml_image w = pmalloc (pool, sizeof *w);

  w->ops = &image_ops;
  w->pool = pool;
  w->src = src;

  return w;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_image w = (ml_image) vw;

  if (w->src)
    {
      io_fprintf (io, "<img src=\"%s\" />", w->src);
    }
}
