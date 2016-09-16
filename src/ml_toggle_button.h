/* Monolith toggle button class.
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
 * $Id: ml_toggle_button.h,v 1.2 2002/10/30 21:03:04 rich Exp $
 */

#ifndef ML_TOGGLE_BUTTON_H
#define ML_TOGGLE_BUTTON_H

#include "monolith.h"

struct ml_toggle_button;
typedef struct ml_toggle_button *ml_toggle_button;

/* Function: new_ml_toggle_button - monolith toggle button widget
 * Function: ml_toggle_button_set_state
 * Function: ml_toggle_button_get_state
 * Function: ml_toggle_button_set_callback
 *
 * A toggle button widget is similar to an ordinary button
 * (see @ref{new_ml_button(3)}) except that it has two states,
 * 0 and non-zero (out, or pushed in respectively). You can
 * query or set the current state of the toggle button, and
 * you can arrange for a callback function to be called when
 * the state changes.
 *
 * Note that toggle buttons cannot be used as part of forms.
 * Use an @code{ml_form_checkbox} instead.
 *
 * @code{new_ml_toggle_button} creates a new toggle button widget.
 *
 * The following properties can be changed on toggle buttons (see
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
 * @code{button.key} (boolean): The "key" property of
 * the button. This should be set on buttons where the text is
 * a single letter, suitable for, say, calculator keys. All key
 * buttons are rendered at a fixed width of "1em", resulting in
 * a more pleasing overall effect.
 *
 * @code{ml_toggle_button_(set|get)_state} updates or queries the
 * current state of the button. The state is either 0 (the default)
 * meaning the button is out, or non-zero meaning the button is
 * pushed in.
 *
 * @code{ml_toggle_button_set_callback} updates the callback
 * function which is invoked when the button changes state
 * (either from "out" to "in", or from "in" to "out"). Use
 * @code{ml_toggle_button_get_state} to find the new state.
 */
extern ml_toggle_button new_ml_toggle_button (pool pool, ml_session, const char *text);
extern void ml_toggle_button_set_state (ml_toggle_button, int state);
extern int ml_toggle_button_get_state (ml_toggle_button);
extern void ml_toggle_button_set_callback (ml_toggle_button, void (*fn)(ml_session, void *), ml_session, void *);

#endif /* ML_TOGGLE_BUTTON_H */
