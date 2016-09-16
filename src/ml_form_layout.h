/* Monolith form layout class.
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
 * $Id: ml_form_layout.h,v 1.1 2002/11/13 20:46:37 rich Exp $
 */

#ifndef ML_FORM_LAYOUT_H
#define ML_FORM_LAYOUT_H

#include <ml_widget.h>

struct ml_form_layout;
typedef struct ml_form_layout *ml_form_layout;

/* Function: new_ml_form_layout - monolith form layout widget
 * Function: ml_form_layout_pack
 * Function: ml_form_layout_pack_help
 *
 * This is the monolith form layout widget. It is used to provide a
 * very easy way to lay out forms. It contains two columns: the left
 * hand column contains the field names, and the right hand column
 * contains the form input widgets.
 *
 * It is a specialised version of @code{ml_multicol_layout}, which is
 * itself a specialised version of @code{ml_table_layout}.
 *
 * @code{new_ml_form_layout} creates a new form layout widget.
 *
 * The following properties can be changed on form layouts (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 *
 * @code{ml_form_layout_pack} packs a row in the form layout widget.
 * The @code{label} string is the label for the row (normally
 * something like @code{"field name:"}. This is placed in the left
 * hand column, and rendered in bold, right-aligned. @code{label} may also be
 * @code{NULL} for no label. @code{input} is a widget (normally
 * a form input widget), which is placed in the right hand column.
 *
 * @code{ml_form_layout_pack_help} packs a row which consists of
 * help information. This is placed in the right hand column and
 * rendered as small text.
 */
extern ml_form_layout new_ml_form_layout (pool pool);
extern void ml_form_layout_pack (ml_form_layout, const char *label, ml_widget input);
extern void ml_form_layout_pack_help (ml_form_layout, const char *help_text);

#endif /* ML_FORM_LAYOUT_H */
