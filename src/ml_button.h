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
 * $Id: ml_button.h,v 1.6 2002/11/08 23:19:14 rich Exp $
 */

#ifndef ML_BUTTON_H
#define ML_BUTTON_H

#include "monolith.h"

struct ml_button;
typedef struct ml_button *ml_button;

/* Function: new_ml_button - monolith button widget
 * Function: ml_button_set_callback
 * Function: ml_button_set_popup
 * Function: ml_button_set_popup_size
 *
 * A button widget is a simple button which calls a function when clicked.
 *
 * @code{new_ml_button} creates a new button widget.
 *
 * The following properties can be changed on buttons (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{text}: The text on the button. The string must have a
 * lifetime as long as or longer than the button's pool.  Setting this
 * to @code{NULL} has the same effect as setting it to the empty
 * string, which is to say not very useful because the button cannot
 * be pressed.
 *
 * @code{title}: If not @code{NULL}, then this string will be
 * displayed as a tool tip (bubble) when the mouse is held over the
 * button. Note that this sort of information hiding rarely scores
 * highly in usability tests, so you should make sure important
 * information is always visible in the interface.
 *
 * @code{button.style}: The style may be: @code{NULL} or @code{"default"}
 * meaning the default look and feel; @code{"key"} meaning that the
 * button has a fixed width of @code{1em}, resulting in a more
 * pleasing effect for calculator key buttons; @code{"compact"}
 * meaning a compact @code{[text]} look; @code{"link"} meaning that
 * the button will look like a hyperlink.
 *
 * @code{color}: The button's color property.
 *
 * @code{class}: The stylesheet class.
 *
 * @code{ml_button_set_callback} updates the callback
 * function which is invoked when the button is pressed.
 *
 * @code{ml_button_set_popup} sets the popup property of the
 * button. When the user clicks on a popup button, a new popup
 * window opens on the screen. To make a button into a popup
 * button, the popup property should be a non-@code{NULL} string
 * which is the name of the new browser window (note that because
 * of limitations in HTML, browser window names are global, so
 * it is recommended that names be "@code{appname}_@code{name}").
 * To change a popup button back to an ordinary button, set the
 * popup property to @code{NULL}.
 *
 * The callback used by a popup button must create a top-level
 * window, otherwise you will get an internal server error.
 *
 * @code{ml_button_set_popup_size} changes the size of the popup
 * window. The default is width 0, height 0, which usually creates a
 * popup window which is the same size and shape as the current
 * browser window.
 */
extern ml_button new_ml_button (pool pool, const char *text);
extern void ml_button_set_callback (ml_button, void (*fn)(ml_session, void *), ml_session, void *);
extern void ml_button_set_popup (ml_button, const char *name);
extern void ml_button_set_popup_size (ml_button, int width, int height);

#endif /* ML_BUTTON_H */
