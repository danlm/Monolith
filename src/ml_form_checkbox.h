/* Monolith for checkbox input class.
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
 * $Id: ml_form_checkbox.h,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#ifndef ML_FORM_CHECKBOX_H
#define ML_FORM_CHECKBOX_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_checkbox;
typedef struct ml_form_checkbox *ml_form_checkbox;

/* Function: new_ml_form_checkbox - monolith form checkbox input widget
 *
 * This is a checkbox (tickbox) for use in forms.
 *
 * @code{new_ml_form_checkbox} creates a new form checkbox input widget. The
 * form into which this widget is being embedded is passed as the
 * @code{form} parameter.
 *
 * See also: @ref{new_ml_form(3)}, @ref{ml_form_input_get_value(3)}.
 */
extern ml_form_checkbox new_ml_form_checkbox (pool pool, ml_form form);

#endif /* ML_FORM_CHECKBOX_H */
