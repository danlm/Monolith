/* Monolith flow layout class.
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
 * $Id: ml_flow_layout.h,v 1.2 2002/08/30 14:28:46 rich Exp $
 */

#ifndef ML_FLOW_LAYOUT_H
#define ML_FLOW_LAYOUT_H

#include <ml_widget.h>

struct ml_flow_layout;
typedef struct ml_flow_layout *ml_flow_layout;

/* Function: new_ml_flow_layout - monolith flow_layout widget
 * Function: ml_flow_layout_push_back
 * Function: ml_flow_layout_pop_back
 * Function: ml_flow_layout_push_front
 * Function: ml_flow_layout_pop_front
 * Function: ml_flow_layout_get
 * Function: ml_flow_layout_insert
 * Function: ml_flow_layout_replace
 * Function: ml_flow_layout_erase
 * Function: ml_flow_layout_clear
 * Function: ml_flow_layout_size
 * Function: ml_flow_layout_pack
 *
 * A flow layout widget is the simplest type of layout widget. It
 * contains an ordered list of widgets, and it simply arranges them
 * one after another (in other words with no "layout" at all).
 *
 * @code{new_ml_flow_layout} creates a new flow layout widget.
 *
 * Underlying the flow layout widget is a simple c2lib vector, and the
 * other access functions use the same notation as the equivalent
 * c2lib @code{vector_*} functions. Go to the SEE ALSO section
 * below to see how to manipulate widgets within a flow layout.
 *
 * @code{ml_flow_layout_pack} is equivalent to @code{ml_flow_layout_push_back}:
 * it appends the widget to the end of the current vector of widgets.
 *
 * See also: @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_flow_layout new_ml_flow_layout (pool pool);
extern void ml_flow_layout_push_back (ml_flow_layout, ml_widget);
extern ml_widget ml_flow_layout_pop_back (ml_flow_layout);
extern void ml_flow_layout_push_front (ml_flow_layout, ml_widget);
extern ml_widget ml_flow_layout_pop_front (ml_flow_layout);
extern ml_widget ml_flow_layout_get (ml_flow_layout, int i);
extern void ml_flow_layout_insert (ml_flow_layout, int i, ml_widget);
extern void ml_flow_layout_replace (ml_flow_layout, int i, ml_widget);
extern void ml_flow_layout_erase (ml_flow_layout, int i);
extern void ml_flow_layout_clear (ml_flow_layout);
extern int ml_flow_layout_size (ml_flow_layout);
extern void ml_flow_layout_pack (ml_flow_layout, ml_widget);

#endif /* ML_FLOW_LAYOUT_H */
