/* Monolith select layout class.
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
 * $Id: ml_select_layout.h,v 1.2 2002/11/29 10:43:03 rich Exp $
 */

#ifndef ML_SELECT_LAYOUT_H
#define ML_SELECT_LAYOUT_H

#include <ml_widget.h>

struct ml_select_layout;
typedef struct ml_select_layout *ml_select_layout;

/* Function: new_ml_select_layout - monolith select layout widget
 * Function: ml_select_layout_push_back
 * Function: ml_select_layout_pop_back
 * Function: ml_select_layout_push_front
 * Function: ml_select_layout_pop_front
 * Function: ml_select_layout_get
 * Function: ml_select_layout_insert
 * Function: ml_select_layout_replace
 * Function: ml_select_layout_erase
 * Function: ml_select_layout_clear
 * Function: ml_select_layout_size
 * Function: ml_select_layout_pack
 * Function: ml_select_layout_set_selection
 * Function: ml_select_layout_get_selection
 *
 * This is the select layout widget. The appearance of select layouts
 * is similar to the "preferences" dialog in versions of the Netscape
 * browser. On the left is a list of items to choose from. On the right,
 * the chosen item is displayed. The user may click an item from the
 * list on the left in order to change the widget which is shown on the
 * right.
 *
 * @code{new_ml_select_layout} creates a new, empty select layout widget.
 *
 * The following properties can be changed on select layout widgets (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 *
 * @code{select_layout.top} (a widget): A widget displayed above
 * the list of selections (the default is @code{NULL} meaning nothing
 * is displayed there).
 *
 * @code{select_layout.bottom} (a widget): A widget displayed below
 * the list of selections (the default is @code{NULL} meaning nothing
 * is displayed there).
 *
 * Underlying the select layout widget is a simple c2lib vector, and the
 * other access functions use the same notation as the equivalent
 * c2lib @code{vector_*} functions. Go to the SEE ALSO section
 * below to see how to manipulate widgets within a flow layout.
 *
 * @code{ml_select_layout_pack} is equivalent to
 * @code{ml_select_layout_push_back}: it appends the item to the
 * end of the current vector of widgets.
 *
 * @code{ml_select_layout_set_selection} sets the currently selected
 * widget. This highlights the selection name on the left and displays
 * the corresponding widget on the right. The default is that the first
 * (index 0) widget is displayed.
 *
 * @code{ml_select_layout_get_selection} returns the currently selected
 * widget.
 *
 * See also: @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_select_layout new_ml_select_layout (pool pool, ml_session session);
extern void ml_select_layout_push_back (ml_select_layout, const char *name, ml_widget);
extern ml_widget ml_select_layout_pop_back (ml_select_layout);
extern void ml_select_layout_push_front (ml_select_layout, const char *name, ml_widget);
extern ml_widget ml_select_layout_pop_front (ml_select_layout);
extern ml_widget ml_select_layout_get (ml_select_layout, int i);
extern void ml_select_layout_insert (ml_select_layout, int i, const char *name, ml_widget);
extern void ml_select_layout_replace (ml_select_layout, int i, const char *name, ml_widget);
extern void ml_select_layout_erase (ml_select_layout, int i);
extern void ml_select_layout_clear (ml_select_layout);
extern int ml_select_layout_size (ml_select_layout);
extern void ml_select_layout_pack (ml_select_layout, const char *name, ml_widget);
extern void ml_select_layout_set_selection (ml_select_layout, int i);
extern int ml_select_layout_get_selection (ml_select_layout);

#endif /* ML_SELECT_LAYOUT_H */
