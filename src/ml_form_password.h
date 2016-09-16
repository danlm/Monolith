/* Monolith form password input class.
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
 * $Id: ml_form_password.h,v 1.1 2002/08/30 14:28:47 rich Exp $
 */

#ifndef ML_FORM_PASSWORD_H
#define ML_FORM_PASSWORD_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_password;
typedef struct ml_form_password *ml_form_password;

/* Function: new_ml_form_password - monolith form password input widget
 *
 * This is a single line text input field, for use in forms. The
 * field is rendered by the browser obscured (often with "*" for
 * characters). However it is passed over the network in plain text.
 *
 * @code{new_ml_form_password} creates a new form password input widget. The
 * form into which this widget is being embedded is passed as the
 * @code{form} parameter.
 *
 * See also: @ref{new_ml_form(3)}, @ref{ml_form_input_get_value(3)}.
 */
extern ml_form_password new_ml_form_password (pool pool, ml_form form);

#endif /* ML_FORM_PASSWORD_H */
