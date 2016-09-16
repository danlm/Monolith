/* Monolith form input class.
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
 * $Id: ml_form_input.h,v 1.3 2002/08/30 14:28:47 rich Exp $
 */

#ifndef ML_FORM_INPUT_H
#define ML_FORM_INPUT_H

#include "monolith.h"

/* As with widgets, form inputs are opaque except within ml_form_input.c
 * itself.
 */
typedef void *ml_form_input;

/* A pointer to this struct must occupy the second slot in every
 * form input structure we define elsewhere (just after the widget
 * pointer in the first slot).
 */
struct ml_form_input_operations
{
  void (*clear_value) (void *form_input);
  void (*set_value) (void *form_input, const char *value);
  const char *(*get_value) (void *form_input);
};

/* Function: ml_form_input_set_value - Operations on generic form inputs.
 * Function: ml_form_input_get_value
 * Function: ml_form_input_clear_value
 *
 * @code{ml_form_input_(set|get)_value} update the value field in any
 * form input.
 *
 * @code{ml_form_input_clear_value} clears (nulls) the value field.
 */
extern void ml_form_input_set_value (ml_form_input form_input, const char *value);
extern const char *ml_form_input_get_value (ml_form_input form_input);
extern void ml_form_input_clear_value (ml_form_input form_input);

#endif /* ML_FORM_INPUT_H */
