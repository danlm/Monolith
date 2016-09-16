/* Monolith form submit button class.
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
 * $Id: ml_form_submit.h,v 1.2 2002/08/29 17:34:18 rich Exp $
 */

#ifndef ML_FORM_SUBMIT_H
#define ML_FORM_SUBMIT_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_submit;
typedef struct ml_form_submit *ml_form_submit;

/* Function: new_ml_form_submit - monolith form submit button widget
 *
 * This is a submit button, which can only be used within forms (see
 * @code{new_ml_form(3)}).
 *
 * @code{new_ml_form_submit} creates a new form submit button. The
 * form into which this widget is being embedded is passed as the
 * @code{form} parameter. The @code{value}
 * parameter is what is displayed on the button.
 *
 * See also: @ref{new_ml_form(3)}, @ref{ml_form_input_get_value(3)}.
 */
extern ml_form_submit new_ml_form_submit (pool pool, ml_form form, const char *value);

#endif /* ML_FORM_SUBMIT_H */
