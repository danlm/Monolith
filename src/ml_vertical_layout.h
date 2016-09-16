/* Monolith vertical layout class.
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
 * $Id: ml_vertical_layout.h,v 1.2 2003/01/12 22:12:42 rich Exp $
 */

#ifndef ML_VERTICAL_LAYOUT_H
#define ML_VERTICAL_LAYOUT_H

#include <ml_widget.h>

struct ml_vertical_layout;
typedef struct ml_vertical_layout *ml_vertical_layout;

/* Function: new_ml_vertical_layout - monolith vertical_layout widget
 * Function: ml_vertical_layout_push_back
 * Function: ml_vertical_layout_pop_back
 * Function: ml_vertical_layout_push_front
 * Function: ml_vertical_layout_pop_front
 * Function: ml_vertical_layout_get
 * Function: ml_vertical_layout_insert
 * Function: ml_vertical_layout_replace
 * Function: ml_vertical_layout_erase
 * Function: ml_vertical_layout_clear
 * Function: ml_vertical_layout_size
 * Function: ml_vertical_layout_pack
 *
 * A vertical layout widget is a simple type of layout which can be
 * thought of as a specialised one-column table.
 *
 * @code{new_ml_vertical_layout} creates a new vertical layout widget.
 *
 * Underlying the vertical layout widget is a simple c2lib vector, and the
 * other access functions use the same notation as the equivalent
 * c2lib @code{vector_*} functions. Go to the SEE ALSO section
 * below to see how to manipulate widgets within a vertical layout.
 *
 * @code{ml_vertical_layout_pack} is equivalent to
 * @code{ml_vertical_layout_push_back}: it appends the widget to the
 * end of the current vector of widgets.
 *
 * The following properties can be changed on vertical layouts (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 * See also: @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_vertical_layout new_ml_vertical_layout (pool pool);
extern void ml_vertical_layout_push_back (ml_vertical_layout, ml_widget);
extern ml_widget ml_vertical_layout_pop_back (ml_vertical_layout);
extern void ml_vertical_layout_push_front (ml_vertical_layout, ml_widget);
extern ml_widget ml_vertical_layout_pop_front (ml_vertical_layout);
extern ml_widget ml_vertical_layout_get (ml_vertical_layout, int i);
extern void ml_vertical_layout_insert (ml_vertical_layout, int i, ml_widget);
extern void ml_vertical_layout_replace (ml_vertical_layout, int i, ml_widget);
extern void ml_vertical_layout_erase (ml_vertical_layout, int i);
extern void ml_vertical_layout_clear (ml_vertical_layout);
extern int ml_vertical_layout_size (ml_vertical_layout);
extern void ml_vertical_layout_pack (ml_vertical_layout, ml_widget);

#endif /* ML_VERTICAL_LAYOUT_H */
