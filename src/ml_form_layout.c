/* Monolith form layout class.
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
 * $Id: ml_form_layout.c,v 1.1 2002/11/13 20:46:37 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "ml_widget.h"
#include "monolith.h"
#include "ml_text_label.h"
#include "ml_multicol_layout.h"
#include "ml_form_layout.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations form_layout_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_form_layout
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_multicol_layout tbl;	/* We're really a multi-column layout. */
  const char *clazz;		/* Stylesheet class. */
};

static void update_table_class (void *vw);

static struct ml_widget_property properties[] =
  {
    { name: "class",
      offset: ml_offsetof (struct ml_form_layout, clazz),
      type: ML_PROP_STRING,
      on_set: update_table_class },
    { 0 }
  };

ml_form_layout
new_ml_form_layout (pool pool)
{
  ml_form_layout w = pmalloc (pool, sizeof *w);

  w->ops = &form_layout_ops;
  w->pool = pool;
  w->tbl = new_ml_multicol_layout (pool, 2);
  w->clazz = "ml_form_layout";
  update_table_class (w);

  return w;
}

static void
update_table_class (void *vw)
{
  ml_form_layout w = (ml_form_layout) vw;

  ml_widget_set_property (w->tbl, "class", w->clazz);
}

void
ml_form_layout_pack (ml_form_layout w, const char *label, ml_widget input)
{
  ml_text_label lbl;

  if (label)
    {
      lbl = new_ml_text_label (w->pool, label);

      ml_multicol_layout_set_header (w->tbl, 1);
      ml_multicol_layout_pack (w->tbl, lbl);
    }
  else
    {
      ml_multicol_layout_set_header (w->tbl, 1);
      ml_multicol_layout_pack (w->tbl, 0);
    }

  ml_multicol_layout_pack (w->tbl, input);
}

void
ml_form_layout_pack_help (ml_form_layout w, const char *help_text)
{
  ml_text_label lbl;

  ml_multicol_layout_set_header (w->tbl, 1);
  ml_multicol_layout_pack (w->tbl, 0);

  lbl = new_ml_text_label (w->pool, help_text);
  ml_widget_set_property (lbl, "font.size", "small");
  ml_multicol_layout_pack (w->tbl, lbl);
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_form_layout w = (ml_form_layout) vw;

  ml_widget_repaint (w->tbl, session, windowid, io);
}
