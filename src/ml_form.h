/* Monolith form class.
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
 * $Id: ml_form.h,v 1.5 2002/11/23 16:46:05 rich Exp $
 */

#ifndef ML_FORM_H
#define ML_FORM_H

#include "monolith.h"
#include "ml_form_input.h"

struct ml_form;
typedef struct ml_form *ml_form;

/* Function: new_ml_form - monolith form widget
 * Function: ml_form_set_callback
 * Function: ml_form_pack
 *
 * A monolith form widget is a low-level widget for handling input of
 * text fields, check buttons, radio buttons and so on, collected together
 * into a single form on a page.
 *
 * A form should contain a mixture of input widgets and ordinary widgets.
 * Since the form widget itself can only be packed with a single widget,
 * normally you would pack a layout widget directly into the form, and
 * then a mixture of labels and input widgets into the layout widget.
 *
 * One or more of the widgets packed into the form can be a submit
 * button (see @ref{new_ml_form_submit(3)}). Pressing on the submit
 * button causes the form's callback function to be called, and in this
 * function the values entered into the other input widgets can be
 * read. It is recommended that all forms contain at least one submit
 * button, because the effect of creating a form with no submit buttons
 * is browser-dependent, and can mean that the form cannot be submitted.
 *
 * Forms cannot be nested (a form widget cannot contain another form
 * widget inside itself). This is a limitation of HTML.
 *
 * Forms are low-level entities. To do seriously interesting things with
 * forms such as self-validation, use one of the higher-level form-type
 * abstractions that monolith provides (XXX will provide, not now - RWMJ).
 *
 * @code{new_ml_form} creates a new form widget.
 *
 * @code{ml_form_set_callback} sets the callback function which is
 * called when the form is submitted. The callback function is invoked
 * as @code{void callback_fn (ml_session session, void *data)}. The
 * values that the user entered in the input fields are available
 * by calling (for example) @ref{ml_form_input_get_value(3)} on each
 * input widget within the form.
 *
 * @code{ml_form_pack} packs a widget into the form. A form can only
 * store a single widget, so if you call pack subsequent times, the
 * earlier widgets are forgotten.
 *
 * The following properties can be changed on forms (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{method}: This is an esoteric property that you will
 * probably never need to use. It changes the behaviour of the
 * HTML form to use either @code{"GET"} or @code{"POST"} - the
 * default being @code{"POST"}.
 *
 * @code{form.name}: A read-only property containing the name of
 * the form. It is unlikely that you will ever need to know this.
 *
 * See also: @ref{new_ml_form_input(3)}, @ref{new_ml_form_textarea(3)},
 * @ref{new_ml_form_submit(3)}.
 */
extern ml_form new_ml_form (pool pool);
extern void ml_form_set_callback (ml_form, void (*callback_fn) (ml_session, void *), ml_session session, void *data);
extern void ml_form_pack (ml_form, ml_widget);

/* Form input widgets must call this function during their 'new' phase
 * to register themselves with the form. The function returns a unique
 * name for the form input field.
 */
extern const char *_ml_form_register_widget (ml_form f, ml_form_input w);

#endif /* ML_FORM_H */
