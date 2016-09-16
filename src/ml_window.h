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
 * $Id: ml_window.h,v 1.7 2002/12/01 18:28:30 rich Exp $
 */

#ifndef ML_WINDOW_H
#define ML_WINDOW_H

#include <pthr_iolib.h>
#include <pthr_http.h>
#include <ml_widget.h>

struct ml_window;
typedef struct ml_window *ml_window;

struct ml_session;

struct ml_frame_description
{
  void (*fn) (struct ml_session *, void *data);
  void *data;
};

/* Function: new_ml_window - monolith windows, framesets, redirects
 * Function: ml_window_pack
 * Function: ml_window_set_headers_flag
 * Function: ml_window_get_headers_flag
 * Function: ml_window_set_title
 * Function: ml_window_get_title
 * Function: ml_window_set_stylesheet
 * Function: ml_window_get_stylesheet
 * Function: ml_window_set_charset
 * Function: ml_window_get_charset
 * Function: ml_window_set_refresh
 * Function: ml_window_get_refresh
 * Function: ml_window_scroll_to
 * Function: new_ml_frameset
 * Function: ml_frameset_set_description
 * Function: ml_frameset_set_title
 * Function: ml_frameset_get_title
 * Function: new_ml_redirect
 * Function: ml_redirect_set_uri
 * Function: ml_redirect_get_uri
 *
 * A "window" is a top-level window. Every application has at
 * least one window, created in the main function. A window is
 * just a wrapper. To actually do anything, you must pack a
 * single widget inside the window. Windows can only take a
 * single widget. If you want more than one widget to appear
 * inside a window, then you must pack them into some sort
 * of layout widget first, and pack the layout widget into
 * the window.
 *
 * A "frameset" is used to create framesets (several ordinary
 * windows packed into one top-level window, with independent
 * scrolling capabilities). Framesets may contain windows or
 * other framesets, or a mixture of both.
 *
 * A "redirect" is a window/frame that shows a static page
 * (in fact anything which can be reached using a URI - pages, images,
 * CGI scripts, even other Monolith applications).
 *
 * Windows, framesets and redirects are actually so
 * similar, that I have included them in the same class (and also so
 * that other code can deal with an opaque @code{ml_window} pointer
 * and not have to worry about whether it is a window,
 * frameset, etc.).
 *
 * Monolith windows are not widgets (unlike many of the
 * other classes in monolith).
 *
 * @code{new_ml_window} creates a new monolith window.
 *
 * @code{ml_window_pack} packs a widget into the window. Since a
 * window can only contain a single widget, subsequent calls to
 * this function overwrite the packed widget. (Note: this call
 * does not apply to framesets).
 *
 * @code{ml_window_(set|get)_headers_flag} is a somewhat esoteric flag
 * that you will almost never need to change. When set (the default),
 * the window widget outputs the opening @code{<html>}, @code{<head/>},
 * @code{<body>} and closing @code{</body>}, @code{</html>}. If
 * cleared, then these are not output. Almost the only place where it
 * is useful to clear this flag is when using the @code{ml_msp}
 * (monolith server-parsed pages) widget directly inside a window
 * (see @ref{new_ml_msp(3)}).
 *
 * @code{ml_window_(set|get)_title} changes the title of
 * the window. The title of the window defaults to no title
 * at all, so it is a good idea to set this.
 *
 * @code{ml_window_(set|get)_stylesheet} changes the stylesheet
 * of the page. Monolith default stylesheets are installed in
 * @code{/ml-styles/}, and the default stylesheet is
 * @code{/ml-styles/default.css} (supplied in the monolith
 * distribution). Stylesheets are used to 'theme' monolith
 * applications.
 *
 * @code{ml_window_(set|get)_charset} changes the character
 * encoding associated with the window. Because of limitations
 * in HTML, only a single charset can be associated with all
 * of the widgets in a window. The default charset is
 * @code{utf-8}.
 *
 * @code{ml_window_(set|get)_refresh} changes the refresh
 * period. This is an integer representing a number of seconds.
 * If this is greater than zero, then the browser will automatically
 * refetch the page after this many seconds. The default is zero
 * which means no automatic refresh. It is not recommended that
 * you set this in ordinary applications.
 *
 * @code{ml_window_scroll_to} scrolls the window to the absolute
 * (@code{x}, @code{y}) pixel position given. This is not supported by
 * all browsers.
 *
 * @code{new_ml_frameset} creates a new frameset. @code{rows} and
 * @code{cols} define the number of frames and their layout.  You can
 * use @code{rows} and @code{cols} to create frameset layouts
 * including grids, as described in the HTML 4 standard
 * (@code{http://www.w3.org/TR/html401/}). To create nested framesets,
 * use a frame which generates a frameset instead of a
 * window. @code{frames} is a vector of @code{struct ml_frame_description}
 * structures (note: the structures themselves, not pointers to the
 * structures). @code{struct ml_frame_description} contains the
 * following fields:
 *
 * @code{fn}: The function which is called generate the frame
 * (or nested frameset). This function must call either
 * @code{new_ml_window} or @code{new_ml_frameset}. The function
 * is prototyped as @code{void fn (ml_session session, void *data);}.
 *
 * @code{data}: Data pointer passed to this function.
 *
 * @code{ml_frameset_set_description} allows the frameset description
 * to be updated.
 *
 * @code{ml_frameset_(set|get)_title} updates the window title.
 *
 * @code{new_ml_redirect} creates a new redirect. @code{uri}
 * is the URI of the static page (etc.) containing the actual
 * content for this window/frame.
 *
 * @code{ml_redirect_(set|get)_uri} updates the URI of the static
 * page being displayed. Note that setting the URI will not necessarily
 * repaint the contents of the window.
 *
 * See also: @ref{ml_ok_window(3)}.
 */
extern ml_window new_ml_window (struct ml_session *, pool pool);
extern void ml_window_pack (ml_window, ml_widget);
extern void ml_window_set_headers_flag (ml_window, int headers_flag);
extern int ml_window_get_headers_flag (ml_window);
extern void ml_window_set_title (ml_window, const char *title);
extern const char *ml_window_get_title (ml_window);
extern void ml_window_set_stylesheet (ml_window, const char *stylesheet);
extern const char *ml_window_get_stylesheet (ml_window);
extern void ml_window_set_charset (ml_window, const char *charset);
extern const char *ml_window_get_charset (ml_window);
extern void ml_window_set_refresh (ml_window, int refresh);
extern int ml_window_get_refresh (ml_window);
extern void ml_window_scroll_to (ml_window, int x, int y);
extern ml_window new_ml_frameset (struct ml_session *, pool pool, const char *rows, const char *cols, vector frames);
extern void ml_frameset_set_description (ml_window, struct ml_session *, const char *rows, const char *cols, vector frames);
extern void ml_frameset_set_title (ml_window, const char *);
extern const char *ml_frameset_get_title (ml_window);
extern ml_window new_ml_redirect (struct ml_session *, pool pool, const char *uri);
extern void ml_redirect_set_uri (ml_window, const char *uri);
extern const char *ml_redirect_get_uri (ml_window);

/* Internal functions to repaint the window. */
extern int _ml_window_get_response_code (ml_window w);
extern const char *_ml_window_get_response_name (ml_window w);
extern void _ml_window_send_headers (ml_window w, pool thread_pool, http_response http_response);
extern void _ml_window_repaint (ml_window, struct ml_session *, io_handle);

/* Internal function to get the current windowid - used in a very few,
 * quite rare places in monolith widgets.
 */
extern const char *_ml_window_get_windowid (ml_window);

#endif /* ML_WINDOW_H */
