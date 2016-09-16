/* Monolith form text input class.
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
 * $Id: ml_form_text.h,v 1.3 2002/11/23 16:46:06 rich Exp $
 */

#ifndef ML_FORM_TEXT_H
#define ML_FORM_TEXT_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_text;
typedef struct ml_form_text *ml_form_text;

/* Function: new_ml_form_text - monolith form text input widget
 * Function: ml_form_text_focus
 *
 * This is a single line text input field, which can only be used
 * within forms (see @code{new_ml_form(3)}).
 *
 * @code{new_ml_form_text} creates a new form text input widget. The
 * form into which this widget is being embedded is passed as the
 * @code{form} parameter.
 *
 * The following properties can be changed on form text input widgets
 * (see @ref{ml_widget_set_property(3)}):
 *
 * @code{form.text.size}: Size (ie. width) of the input box. The default is
 * @code{-1} which is a browser-specific width.
 *
 * @code{form.text.maxlength}: Maximum number of characters which
 * may be entered into the input box (do not rely on this: always
 * check the length). The default is @code{-1} which means unlimited.
 *
 * If @code{ml_form_text_focus} is called on the input widget, then
 * it will attempt to grab keyboard focus when displayed. (NB. This
 * is a temporary function, very likely to change in future when I
 * come up with a better way to handle focus).
 *
 * See also: @ref{new_ml_form(3)}, @ref{ml_form_input_get_value(3)}.
 */
extern ml_form_text new_ml_form_text (pool pool, ml_form form);
extern void ml_form_text_focus (ml_form_text w);

#endif /* ML_FORM_TEXT_H */
