/* Monolith group of radio buttons.
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
 * $Id: ml_form_radio_group.h,v 1.1 2002/08/30 14:28:48 rich Exp $
 */

#ifndef ML_FORM_RADIO_GROUP_H
#define ML_FORM_RADIO_GROUP_H

#include "monolith.h"
#include "ml_form.h"

struct ml_form_radio_group;
typedef struct ml_form_radio_group *ml_form_radio_group;

struct ml_form_radio;

/* Function: new_ml_form_radio_group - monolith group of radio buttons widget
 * Function: ml_form_radio_group_pack
 *
 * A radio group is a widget which contains a group of related radio
 * buttons. You cannot use radio buttons "naked" in a form, but instead
 * must embed related buttons inside one of these widgets.
 *
 * @code{new_ml_form_radio_group} creates a new group widget.
 *
 * @code{ml_form_radio_group_pack} packs a single widget inside the
 * radio group widget. A radio group can only contain a single widget,
 * so if you call pack again, it will forget the previous widget.
 * It is recommended that you pack either a flow layout or a table
 * layout directly inside the radio group, and then pack the actual
 * radio buttons inside that.
 *
 * See also: @ref{new_ml_form(3)}, @ref{new_ml_form_radio(3)},
 * @ref{ml_form_input_get_value(3)}.
 */
extern ml_form_radio_group new_ml_form_radio_group (pool pool, ml_form form);
extern void ml_form_radio_group_pack (ml_form_radio_group w, ml_widget);

/* Radio buttons use this function to register themselves. It returns
 * the name of the button.
 */
extern const char *_ml_form_radio_group_register (ml_form_radio_group w, struct ml_form_radio *, const char *value);

#endif /* ML_FORM_RADIO_H */
