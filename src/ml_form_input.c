/* Monolith form input class.
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
 * $Id: ml_form_input.c,v 1.3 2002/08/30 14:28:47 rich Exp $
 */

#include "config.h"

#include "monolith.h"
#include "ml_widget.h"
#include "ml_form_input.h"

/* This is what generic form input objects *actually* look like inside. */
struct form_input
{
  struct ml_widget_operations *ops;
  struct ml_form_input_operations *fops;
};

void
ml_form_input_set_value (void *vw, const char *value)
{
  struct form_input *w = (struct form_input *) vw;

  w->fops->set_value (vw, value);
}

const char *
ml_form_input_get_value (void *vw)
{
  struct form_input *w = (struct form_input *) vw;

  return w->fops->get_value (vw);
}

void
ml_form_input_clear_value (void *vw)
{
  struct form_input *w = (struct form_input *) vw;

  w->fops->clear_value (vw);
}
