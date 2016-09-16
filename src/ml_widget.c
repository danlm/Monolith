/* Monolith widget class.
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
 * $Id: ml_widget.c,v 1.5 2002/11/29 10:43:03 rich Exp $
 */

#include "config.h"

#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "monolith.h"
#include "ml_widget.h"

/* This is what generic widget objects *actually* look like. */
struct widget
{
  struct ml_widget_operations *ops;
};

void
ml_widget_repaint (void *vw, ml_session session, const char *windowid,
		   io_handle io)
{
  struct widget *w = (struct widget *) vw;

  if (w->ops->repaint) w->ops->repaint (vw, session, windowid, io);
}

const struct ml_widget_property *
ml_widget_get_properties (void *vw)
{
  struct widget *w = (struct widget *) vw;

  return w->ops->properties;
}

void
ml_widget_set_property (void *vw, const char *property_name, ...)
{
  struct widget *w = (struct widget *) vw;
  struct ml_widget_property *properties = w->ops->properties;
  va_list args;

  /* If you hit this assertion, then you've tried to set properties
   * on a widget which isn't property-aware.
   */
  assert (properties != 0);

  /* Search for the appropriate property. */
  while (properties->name)
    {
      if (strcmp (properties->name, property_name) == 0)
	{
	  /* Read-only? */
	  assert (!(properties->flags & ML_PROP_READ_ONLY));

	  va_start (args, property_name);

	  /* Update it. */
	  switch (properties->type) {
	  case ML_PROP_STRING:
	    {
	      char **v = (char **) (vw + properties->offset);
	      *v = va_arg (args, char *);
	      break;
	    }
	  case ML_PROP_INT:
	    {
	      int *v = (int *) (vw + properties->offset);
	      *v = va_arg (args, int);
	      break;
	    }
	  case ML_PROP_CHAR:
	    {
	      char *v = (char *) (vw + properties->offset);
	      *v = va_arg (args, int); /* sic */
	      break;
	    }
	  case ML_PROP_WIDGET:
	    {
	      ml_widget *v = (ml_widget *) (vw + properties->offset);
	      *v = va_arg (args, ml_widget);
	      break;
	    }
	  default:
	    abort ();		/* Unknown type. */
	  }

	  va_end (args);

	  if (properties->on_set) properties->on_set (vw);

	  return;
	}

      properties++;
    }

  /* If you reach here, then you've tried to update a non-existant
   * property in a widget.
   */
  fprintf (stderr,
	   "ml_widget_set_property: unknown property name: %s\n",
	   property_name);
  abort ();
}

void
_ml_widget_get_property (void *vw, const char *property_name, void *varptr)
{
  struct widget *w = (struct widget *) vw;
  struct ml_widget_property *properties = w->ops->properties;
  int size;

  /* If you hit this assertion, then you've tried to get properties
   * on a widget which isn't property-aware.
   */
  assert (properties != 0);

  /* Search for the appropriate property. */
  while (properties->name)
    {
      if (strcmp (properties->name, property_name) == 0)
	{
	  /* Write-only? */
	  assert (!(properties->flags & ML_PROP_WRITE_ONLY));

	  if (properties->on_get) properties->on_get (vw);

	  /* Retrieve it. */
	  switch (properties->type) {
	  case ML_PROP_STRING:
	    size = sizeof (char *); break;
	  case ML_PROP_INT:
	    size = sizeof (int); break;
	  case ML_PROP_CHAR:
	    size = sizeof (char); break;
	  case ML_PROP_WIDGET:
	    size = sizeof (ml_widget); break;
	  default:
	    abort ();		/* Unknown type. */
	  }

	  memcpy (varptr, vw + properties->offset, size);

	  return;
	}

      properties++;
    }

  /* If you reach here, then you've tried to update a non-existant
   * property in a widget.
   */
  fprintf (stderr,
	   "_ml_widget_get_property: unknown property name: %s\n",
	   property_name);
  abort ();
}
