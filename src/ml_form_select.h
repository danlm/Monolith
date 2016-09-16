/* Monolith form select box input class.
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
 * $Id: ml_form_select.h,v 1.3 2002/11/02 18:53:47 rich Exp $
 */

#ifndef ML_FORM_SELECT_H
#define ML_FORM_SELECT_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_select;
typedef struct ml_form_select *ml_form_select;

/* Function: new_ml_form_select - monolith form select box input widget
 * Function: ml_form_select_push_back
 * Function: ml_form_select_pop_back
 * Function: ml_form_select_push_front
 * Function: ml_form_select_pop_front
 * Function: ml_form_select_get
 * Function: ml_form_select_insert
 * Function: ml_form_select_replace
 * Function: ml_form_select_erase
 * Function: ml_form_select_clear
 * Function: ml_form_select_size
 * Function: ml_form_select_set_selection
 * Function: ml_form_select_set_selections
 * Function: ml_form_select_get_selection
 * Function: ml_form_select_get_selections
 *
 * This is a select box for use in forms. It can appear in several
 * ways: either as a drop-down menu, or as a selection box allowing
 * single or multiple options to be selected.
 *
 * @code{new_ml_form_select} creates a new form select box input widget. The
 * form into which this widget is being embedded is passed as the
 * @code{form} parameter. The select box is created with no options,
 * in drop-down mode (size 0), single choice (multiple 0).
 *
 * The following properties can be changed on form select box input widgets
 * (see @ref{ml_widget_set_property(3)}):
 *
 * @code{form.select.size}: The size (number of rows)
 * in the select box. If the size is 0, then the select box will be
 * rendered as a drop-down menu. If the size is > 0, then the select
 * box will be rendered as a scrolling list of choices.
 *
 * @code{form.select.multiple} (boolean): The multiple boolean
 * property of the select box. If this is false (the default), then
 * only a single option can be selected. If this is true, then multiple
 * options can be selected by the user.
 *
 * To add options to the select box, use the @code{ml_form_select_push_back}
 * and other access functions. These are modelled on the c2lib @code{vector_*}
 * functions.
 *
 * To choose which option is selected first in a single selection select
 * box, call @code{ml_form_select_set_selection}. For select boxes which
 * allow multiple selections, prepare a @code{vector} of @code{int} which
 * is at least as long as the number of options. Each element of the
 * vector should be a boolean value saying whether the corresponding
 * option is selected or not. Pass this to
 * @code{ml_form_select_set_selections}.
 *
 * If the select box allows only single selections, then after the form
 * has been submitted by the user, you can read back the index of the
 * option which was selected using @code{ml_form_select_get_selection}.
 * This returns -1 if nothing was selected.
 *
 * If the select box allows multiple selections, then call
 * @code{ml_form_select_get_selections} which returns a vector
 * of boolean values (@code{vector} of @code{int}) of exactly
 * the same length as the number of options. This vector will
 * contain true corresponding to every selected option. This
 * function may also return @code{NULL}, indicating that
 * nothing was selected (or the form wasn't submitted).
 *
 * See also: @ref{new_ml_form(3)}, @ref{ml_form_input_get_value(3)},
 * @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_form_select new_ml_form_select (pool pool, ml_form form);
extern void ml_form_select_push_back (ml_form_select w, const char *option);
extern const char *ml_form_select_pop_back (ml_form_select w);
extern void ml_form_select_push_front (ml_form_select w, const char *option);
extern const char *ml_form_select_pop_front (ml_form_select w);
extern const char *ml_form_select_get (ml_form_select w, int option_index);
extern void ml_form_select_insert (ml_form_select w, int option_index, const char *option);
extern void ml_form_select_replace (ml_form_select w, int option_index, const char *option);
extern void ml_form_select_erase (ml_form_select w, int option_index);
extern void ml_form_select_clear (ml_form_select w);
extern int ml_form_select_size (ml_form_select w);
extern void ml_form_select_set_selection (ml_form_select w, int option_index);
extern void ml_form_select_set_selections (ml_form_select w, vector selected);
extern int ml_form_select_get_selection (ml_form_select w);
extern const vector ml_form_select_get_selections (ml_form_select w);

#endif /* ML_FORM_SELECT_H */
