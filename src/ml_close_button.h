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
 * $Id: ml_close_button.h,v 1.3 2002/11/13 21:41:01 rich Exp $
 */

#ifndef ML_CLOSE_BUTTON_H
#define ML_CLOSE_BUTTON_H

#include "monolith.h"

struct ml_close_button;
typedef struct ml_close_button *ml_close_button;

/* Function: new_ml_close_button - monolith button widget
 *
 * A close button widget is a simple widget which displays a button.
 * When pressed, the button closes the current window.
 *
 * @code{new_ml_close_button} creates a new close button widget.
 *
 * The following properties can be changed on buttons (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{text}: The HTML text printed
 * on the button. Note that the text string must be either
 * static, or already allocated in the button's pool, or allocated
 * in a pool with a longer lifetime than the button. If the text
 * is set to @code{NULL} then this has the same effect as setting
 * the text to the empty string, which is not very useful because
 * the button can never be pressed.
 *
 * @code{button.reload-opener} (boolean): If set, then when the
 * close button is pressed, then the window which was responsible
 * for opening this window (the "opener") is reloaded. This is
 * useful when we have used a popup button to open a new window,
 * entered some data into a form in the new window, and then need
 * to refresh the old window to show the new data.
 */
extern ml_close_button new_ml_close_button (pool, const char *text);

#endif /* ML_CLOSE_BUTTON_H */
