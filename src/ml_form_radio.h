/* Monolith form radio button input class.
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
 * $Id: ml_form_radio.h,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#ifndef ML_FORM_RADIO_H
#define ML_FORM_RADIO_H

#include "monolith.h"
#include "ml_form_radio_group.h"

struct ml_form_radio;
typedef struct ml_form_radio *ml_form_radio;

/* Function: new_ml_form_radio - monolith radio button widget
 * Function: ml_form_radio_set_checked
 * Function: ml_form_radio_get_checked
 *
 * This is a radio button input, for use in forms. Radio buttons
 * represent choices, and thus are grouped together. For this reason,
 * you must place all related radio buttons inside a @code{ml_form_radio_group}
 * widget, which itself goes inside the form.
 *
 * @code{new_ml_form_radio} creates a new radio button widget. The
 * radio button group into which this widget is being embedded is passed as the
 * @code{group} parameter.
 *
 * After the form has been submitted, you can see which button was
 * pressed by checking the value of the @code{ml_form_radio_group}
 * widget, or by looking at the checked status of each individual
 * radio button using @code{ml_form_radio_(set|get)_checked}.
 *
 * See also: @ref{new_ml_form(3)}, @ref{new_ml_form_radio_group(3)}.
 */
extern ml_form_radio new_ml_form_radio (pool pool, ml_form_radio_group group, const char *value);
extern void ml_form_radio_set_checked (ml_form_radio w, int checked);
extern int ml_form_radio_get_checked (ml_form_radio w);

#endif /* ML_FORM_RADIO_H */
