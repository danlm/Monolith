/* Monolith button class.
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
 * $Id: ml_button.c,v 1.8 2003/01/11 16:39:09 rich Exp $
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_smarttext.h"
#include "ml_button.h"

static void repaint (void *, ml_session, const char *, io_handle);
static struct ml_widget_property properties[];

struct ml_widget_operations button_ops =
  {
    repaint: repaint,
    properties: properties,
  };

struct ml_button
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  const char *text;		/* HTML printed for the button. */
  const char *title;		/* Tool tip (NULL for none). */
  const char *clazz;		/* Class (NULL for default). */
  const char *style;		/* Style: default, key, compact, link */
  const char *colour;		/* Button colour (NULL for default). */
  const char *action_id;	/* Action. */
  const char *target;		/* Button target (NULL for no popup). */
  int popup_w, popup_h;		/* Popup window size. */
};

static struct ml_widget_property properties[] =
  {
    { name: "text",
      offset: ml_offsetof (struct ml_button, text),
      type: ML_PROP_STRING },
    { name: "title",
      offset: ml_offsetof (struct ml_button, title),
      type: ML_PROP_STRING },
    { name: "button.style",
      offset: ml_offsetof (struct ml_button, style),
      type: ML_PROP_STRING },
    { name: "color",
      offset: ml_offsetof (struct ml_button, colour),
      type: ML_PROP_STRING },
    { name: "class",
      offset: ml_offsetof (struct ml_button, clazz),
      type: ML_PROP_STRING },
    { 0 }
  };

ml_button
new_ml_button (pool pool, const char *text)
{
  ml_button w = pmalloc (pool, sizeof *w);

  w->ops = &button_ops;
  w->pool = pool;
  w->text = text;
  w->title = 0;
  w->clazz = 0;
  w->style = 0;
  w->colour = 0;
  w->action_id = 0;
  w->target = 0;
  w->popup_w = w->popup_h = 0;

  return w;
}

void
ml_button_set_callback (ml_button w,
			void (*fn)(ml_session, void *),
			ml_session session, void *data)
{
  if (w->action_id)
    ml_unregister_action (session, w->action_id);
  w->action_id = 0;

  if (fn)
    w->action_id = ml_register_action (session, fn, data);
}

void
ml_button_set_popup (ml_button w, const char *name)
{
  w->target = name;
}

void
ml_button_set_popup_size (ml_button w, int width, int height)
{
  w->popup_w = width;
  w->popup_h = height;
}

static inline const char *
get_class (ml_button w)
{
  if (!w->clazz)
    {
      if (w->style == 0 || strcmp (w->style, "default") == 0)
	return "ml_button";
      else if (strcmp (w->style, "key") == 0)
	return "ml_button_key";
      else if (strcmp (w->style, "compact") == 0)
	return "ml_button_compact";
      else if (strcmp (w->style, "link") == 0)
	return "ml_button_link";
      else
	abort ();		/* Unknown button.style for button. */
    }

  return w->clazz;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_button w = (ml_button) vw;

  if (w->text)
    {
      const char *clazz = get_class (w);

      if (w->action_id)
	{
	  pool tmp = new_subpool (w->pool);

	  /* XXX Link should not contain ml_window parameter if w->target
	   * is set.
	   */
	  const char *link =
	    psprintf (tmp, "%s?ml_action=%s&ml_window=%s",
		      ml_session_script_name (session),
		      w->action_id,
		      windowid);

	  io_fprintf (io, "<a class=\"%s\" href=\"%s\"", clazz, link);

	  if (w->title)
	    {
	      io_fputs (" title=\"", io);
	      ml_plaintext_print (io, w->title);
	      io_fputc ('"', io);
	    }

	  if (w->colour)
	    io_fprintf (io, " style=\"color: %s\"", w->colour);

	  if (w->target)
	    {
	      io_fprintf (io, " target=\"%s\"", w->target);
	      if (w->popup_w != 0 && w->popup_h != 0)
		{
		  io_fprintf (io, " onclick=\"open ('%s', '%s', "
			      "'width=%d,height=%d,scrollbars=1'); "
			      "return false;\"",
			      link, w->target, w->popup_w, w->popup_h);
		}
	    }

	  if (!w->style || strcmp (w->style, "compact") != 0)
	    io_fprintf (io, ">%s</a>", w->text);
	  else
	    io_fprintf (io, ">[%s]</a>", w->text);

	  delete_pool (tmp);
	}
      else
	io_fprintf (io, "<span class=\"%s\">%s</span>", clazz, w->text);
    }
}
