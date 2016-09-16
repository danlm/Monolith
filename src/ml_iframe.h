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
 * $Id: ml_iframe.h,v 1.1 2002/11/08 23:19:14 rich Exp $
 */

#ifndef ML_IFRAME_H
#define ML_IFRAME_H

#include "monolith.h"
#include "ml_widget.h"

struct ml_iframe;
typedef struct ml_iframe *ml_iframe;

/* Function: new_ml_iframe - monolith iframe widget
 *
 * An iframe ("inline frame") is a scrollable, independent window
 * embedded within the application. Note that not all browsers support
 * iframes.
 *
 * @code{new_ml_iframe} creates a new iframe widget. The @code{fn}
 * argument is a function which is called back when the window inside
 * the iframe is drawn. This function must create a window (see
 * @ref{new_ml_window(3)}). The function is called with two
 * parameters, @code{session} and an opaque @code{data} pointer.
 * Since not all browsers support frames, you may specify a
 * widget which is displayed to these users by specifying the
 * @code{non_frame_widget} parameter. If @code{non_frame_widget} is
 * set to @code{NULL} then some generic text is displayed instead.
 *
 * The following properties can be changed on buttons (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 *
 * @code{scrolling}: Can be set to @code{"yes"}, @code{"no"} or
 * @code{"auto"} (the default) to control how scrollbars are
 * displayed around the iframe.
 *
 * @code{width}, @code{height}: Control the width and height of the
 * @code{iframe} in pixels.
 */
extern ml_iframe new_ml_iframe (pool pool, void (*fn) (ml_session, void *), ml_session session, void *data, ml_widget non_frame_widget);

#endif /* ML_IFRAME_H */
