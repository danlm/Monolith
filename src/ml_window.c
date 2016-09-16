/* Monolith window class.
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
 * $Id: ml_window.c,v 1.7 2002/12/01 18:28:30 rich Exp $
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <pstring.h>

#include <pthr_http.h>

#include "monolith.h"
#include "ml_window.h"

struct ml_window
{
  pool pool;			/* Pool for allocations. */
  const char *windowid;		/* Window ID. */

  /* For ordinary windows: */
  ml_widget w;			/* Packed widget. */
  int headers_flag;		/* If set, emit start, end of page. */
  const char *title;		/* Title of the window. */
  const char *stylesheet;	/* Stylesheet for the window. */
  const char *charset;		/* Character encoding. */
  int refresh;			/* Refresh period (0 = no refresh). */
  int scroll_to_x, scroll_to_y;	/* Scroll to (x, y). */

  /* For framesets: */
  const char *rows, *cols;	/* Layout. */
  vector frames;		/* Frames (vector of struct
				 * ml_frame_description). */
  vector actions;		/* Action IDs (one per frame). */

  /* For redirects. */
  const char *uri;		/* URI to redirect to. */
};

static int next_window_id = 0;

ml_window
new_ml_window (ml_session session, pool pool)
{
  ml_window w = pmalloc (pool, sizeof *w);

  w->pool = pool;
  w->w = 0;
  w->headers_flag = 1;
  w->title = 0;
  /* XXX Eventually take this from the default theme for the application? */
  w->stylesheet = "/ml-styles/default.css";
  w->charset = "utf-8";
  w->refresh = 0;
  w->scroll_to_x = w->scroll_to_y = 0;

  w->rows = w->cols = 0;
  w->frames = 0;
  w->actions = 0;

  w->uri = 0;

  /* Register and set current window. */
  w->windowid = pitoa (pool, ++next_window_id);
  _ml_session_set_current_window (session, w, w->windowid);

  return w;
}

void
ml_window_pack (ml_window w, ml_widget _w)
{
  w->w = _w;
}

void
ml_window_set_headers_flag (ml_window w, int headers_flag)
{
  w->headers_flag = headers_flag;
}

int
ml_window_get_headers_flag (ml_window w)
{
  return w->headers_flag;
}

void
ml_window_set_title (ml_window w, const char *title)
{
  w->title = title;
}

const char *
ml_window_get_title (ml_window w)
{
  return w->title;
}

void
ml_window_set_stylesheet (ml_window w, const char *stylesheet)
{
  w->stylesheet = stylesheet;
}

const char *
ml_window_get_stylesheet (ml_window w)
{
  return w->stylesheet;
}

void
ml_window_set_charset (ml_window w, const char *_charset)
{
  w->charset = _charset;
}

const char *
ml_window_get_charset (ml_window w)
{
  return w->charset;
}

void
ml_window_set_refresh (ml_window w, int refresh)
{
  w->refresh = refresh;
}

int
ml_window_get_refresh (ml_window w)
{
  return w->refresh;
}

void
ml_window_scroll_to (ml_window w, int x, int y)
{
  w->scroll_to_x = x;
  w->scroll_to_y = y;
}

static void update_actions (ml_window w, ml_session session);

ml_window
new_ml_frameset (ml_session session, pool pool,
		 const char *rows, const char *cols, vector frames)
{
  ml_window w = new_ml_window (session, pool);

  w->rows = rows;
  w->cols = cols;
  w->frames = frames;
  update_actions (w, session);

  return w;
}

void
ml_frameset_set_description (ml_window w, ml_session session,
			     const char *rows, const char *cols, vector frames)
{
  w->rows = rows;
  w->cols = cols;
  w->frames = frames;
  update_actions (w, session);
}

void
ml_frameset_set_title (ml_window w, const char *title)
{
  w->title = title;
}

const char *
ml_frameset_get_title (ml_window w)
{
  return w->title;
}

static void
update_actions (ml_window w, ml_session session)
{
  int i;
  const char *actionid;
  struct ml_frame_description frame;

  if (w->actions)
    {
      /* Unregister the old actions. */
      for (i = 0; i < vector_size (w->actions); ++i)
	{
	  vector_get (w->actions, i, actionid);
	  ml_unregister_action (session, actionid);
	}
      vector_clear (w->actions);
    }
  else
    w->actions = new_vector (w->pool, const char *);

  if (w->frames && vector_size (w->frames) > 0)
    {
      /* Register new actions. */
      for (i = 0; i < vector_size (w->frames); ++i)
	{
	  vector_get (w->frames, i, frame);
	  actionid = ml_register_action (session, frame.fn, frame.data);
	  vector_push_back (w->actions, actionid);
	}
    }
}

ml_window
new_ml_redirect (ml_session session, pool pool,
		 const char *uri)
{
  ml_window w = new_ml_window (session, pool);

  w->uri = uri;

  return w;
}

void
ml_redirect_set_uri (ml_window w, const char *uri)
{
  w->uri = uri;
}

const char *
ml_redirect_get_uri (ml_window w)
{
  return w->uri;
}

int
_ml_window_get_response_code (ml_window w)
{
  if (! w->uri) return 200;
  else return 301;
}

const char *
_ml_window_get_response_name (ml_window w)
{
  if (! w->uri) return "OK";
  else return "Moved Permanently";
}

void
_ml_window_send_headers (ml_window w, pool thread_pool,
			 http_response http_response)
{
  if (! w->uri)
    {
      /* Send the Content-Type: header. */
      http_response_send_header
	(http_response,
	 "Content-Type", psprintf (thread_pool,
				   "text/html; charset=%s",
				   w->charset));

      /* Refresh period? */
      if (w->refresh != 0)
	http_response_send_header
	  (http_response,
	   "Refresh", pitoa (thread_pool, w->refresh));
    }
  else
    {
      /* Send the Location: header. */
      http_response_send_header (http_response, "Location", w->uri);

      /* Send the Content-Length: header. */
      http_response_send_header (http_response, "Content-Length", "0");
    }
}

const char *
_ml_window_get_windowid (ml_window w)
{
  return w->windowid;
}

void
_ml_window_repaint (ml_window w, ml_session session, io_handle io)
{
  if (!w->frames && !w->uri)	/* Ordinary window. */
    {
      if (w->headers_flag)
	{
	  io_fprintf
	    (io,
	     "<!DOCTYPE html "
	     "PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
	     "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
	     "<html xmlns=\"http://www.w3.org/1999/xhtml\" "
	     "xml:lang=\"en\" lang=\"en\">\n"
	     "<head>\n");

	  if (w->title)
	    io_fprintf (io, "<title>%s</title>\n", w->title);

	  if (w->stylesheet)
	    io_fprintf (io,
			"<link rel=\"stylesheet\" "
			"href=\"%s\" type=\"text/css\">\n",
			w->stylesheet);

	  io_fprintf (io, "</head><body>\n");
	}

      if (w->w)
	ml_widget_repaint (w->w, session, w->windowid, io);

      if (w->scroll_to_x > 0 || w->scroll_to_y > 0)
	{
	  io_fprintf
	    (io,
	     "<script language=\"javascript\"><!--\n"
	     "window.scrollTo (%d, %d);\n"
	     "//--></script>\n",
	     w->scroll_to_x, w->scroll_to_y);
	}

      if (w->headers_flag)
	io_fprintf (io, "</body></html>\n");
    }
  else if (w->frames)		/* Frameset. */
    {
      int i;

      io_fprintf
	(io,
	 "<!DOCTYPE html "
	 "PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" "
	 "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">\n"
	 "<html xmlns=\"http://www.w3.org/1999/xhtml\" "
	 "xml:lang=\"en\" lang=\"en\">\n"
	 "<head>\n");

      if (w->title)
	io_fprintf (io, "<title>%s</title>\n", w->title);

      io_fprintf (io, "</head><frameset");

      if (w->rows)
	io_fprintf (io, " rows=\"%s\"", w->rows);
      if (w->cols)
	io_fprintf (io, " cols=\"%s\"", w->cols);

      io_fprintf (io, ">");

      for (i = 0; i < vector_size (w->frames); ++i)
	{
	  /* struct ml_frame_description frame; */
	  const char *actionid;

	  /* vector_get (w->frames, i, frame); */
	  vector_get (w->actions, i, actionid);

	  io_fprintf (io, "<frame src=\"%s?ml_action=%s\" />",
		      ml_session_script_name (session), actionid);
	}

      io_fprintf (io, "</frameset></html>\n");
    }
  else				/* Redirect. */
    {
      /* Do nothing. */
    }
}
