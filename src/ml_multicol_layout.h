/* Monolith multi-column layout class.
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
 * $Id: ml_multicol_layout.h,v 1.3 2002/11/13 20:46:37 rich Exp $
 */

#ifndef ML_MULTICOL_LAYOUT_H
#define ML_MULTICOL_LAYOUT_H

#include <ml_widget.h>

struct ml_multicol_layout;
typedef struct ml_multicol_layout *ml_multicol_layout;

/* Function: new_ml_multicol_layout - monolith multi-column layout widget
 * Function: ml_multicol_layout_set_align
 * Function: ml_multicol_layout_set_valign
 * Function: ml_multicol_layout_set_class
 * Function: ml_multicol_layout_set_header
 * Function: ml_multicol_layout_pack
 *
 * The multi-column layout widget is a very simple type of layout widget
 * suitable for forms and simple tabular data. The widget is initialised
 * with the fixed number of columns, then each call to the pack method
 * adds a widget at the next available free place. This layout widget
 * creates more rows as required, unlike the table layout widget where
 * rows must be created explicitly.
 *
 * Note: When creating a form, it is best to use the specialised version
 * of multi-column layout, @code{ml_form_layout} (see
 * @ref{new_ml_form_layout(3)}).
 *
 * @code{new_ml_multicol_layout} creates a new multi-column layout
 * widget. The fixed number of columns is specified in the
 * @code{nr_cols} parameter.
 *
 * The following properties can be changed on multi-column layouts (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 *
 * @code{ml_multicol_layout_set_(valign|align|class|header)} sets the
 * cell alignment, class or header flag for the NEXT cell (ie. you
 * must call this function BEFORE calling the pack function). See
 * @ref{ml_table_layout_set_valign(3)},
 * @ref{ml_table_layout_set_align(3)},
 * @ref{ml_table_layout_set_class(3)} and
 * @ref{ml_table_layout_set_header(3)}.
 *
 * @code{ml_multicol_layout_pack} packs another widget at the next
 * available free space, going left-to-right, top-to-bottom. An additional
 * rows is created if required. To pack an empty cell, call this
 * function with the widget parameter set to @code{NULL}.
 */
extern ml_multicol_layout new_ml_multicol_layout (pool pool, int nr_cols);
extern void ml_multicol_layout_set_align (ml_multicol_layout, const char *align);
extern void ml_multicol_layout_set_valign (ml_multicol_layout, const char *valign);
extern void ml_multicol_layout_set_class (ml_multicol_layout, const char *clazz);
extern void ml_multicol_layout_set_header (ml_multicol_layout, int is_header);
extern void ml_multicol_layout_pack (ml_multicol_layout, ml_widget);

#endif /* ML_MULTICOL_LAYOUT_H */
