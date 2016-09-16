/* Monolith tabbed layout class.
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
 * $Id: ml_tabbed_layout.h,v 1.1 2002/11/13 20:46:38 rich Exp $
 */

#ifndef ML_TABBED_LAYOUT_H
#define ML_TABBED_LAYOUT_H

#include <ml_widget.h>

struct ml_tabbed_layout;
typedef struct ml_tabbed_layout *ml_tabbed_layout;

/* Function: new_ml_tabbed_layout - monolith tabbed layout ("dialog") widget
 * Function: ml_tabbed_layout_push_back
 * Function: ml_tabbed_layout_pop_back
 * Function: ml_tabbed_layout_push_front
 * Function: ml_tabbed_layout_pop_front
 * Function: ml_tabbed_layout_get
 * Function: ml_tabbed_layout_insert
 * Function: ml_tabbed_layout_replace
 * Function: ml_tabbed_layout_erase
 * Function: ml_tabbed_layout_clear
 * Function: ml_tabbed_layout_size
 * Function: ml_tabbed_layout_pack
 * Function: ml_tabbed_layout_bring_to_front
 * Function: ml_tabbed_layout_send_to_back
 * Function: ml_tabbed_layout_get_front_tab
 *
 * This is the tabbed layout widget (often called a "tabbed dialog"). It
 * displays a row of tabs along the top, allowing the user to select one
 * of several widgets in the main area. The tabs have a three-dimensional
 * appearance, giving the effect of multiple layers of widgets, similar
 * to a card index.
 *
 * The monolith tabbed layout widget has some intentional limitations.
 * First, and foremost, it only supports a small number of tabs. This
 * is for good usability reasons (see, for instance, this page:
 * @code{http://www.iarchitect.com/tabs.htm} ). If you want to have
 * a widget with a large number of tabs, then a better approach is
 * to use a tree which selects a widget (refer to the classic
 * Netscape Navigator preferences window for an example).
 * Secondly, the tabs may only appear at the top of the page.
 *
 * @code{new_ml_tabbed_layout} creates a new, empty tabbed layout widget.
 *
 * Underlying the tabbed layout is a simple c2lib vector, and the
 * main access functions uses a similar notation to the equivalent
 * c2lib @code{vector_*} functions. The exception is that each tab
 * also has a @code{name} field, which is what is displayed on the
 * tab itself. Go to the SEE ALSO section below
 * to see how to manipulate widgets within the tabbed layout.
 *
 * @code{ml_tabbed_layout_pack} is equivalent to
 * @code{ml_tabbed_layout_push_back}: it appends the widget to the end
 * of the current vector of widgets.
 *
 * Any tab added is always added at the bottom of the pile.
 *
 * In left-to-right locales, the tabs appear in the same order that
 * they are pushed into the vector.
 *
 * There are two further operations which can be carried out on
 * tabs. The @code{ml_tabbed_layout_bring_to_front} function
 * takes a tab index (ie. leftmost tab is 0, etc.) and brings that
 * tab to the front or top of the pile. The
 * @code{ml_tabbed_layout_send_to_back} function takes a tab
 * index and sends that tab to the back or bottom of the pile.
 * The user may also bring tabs to the front by clicking on them.
 * @code{ml_tabbed_layout_get_front_tab} returns the index of
 * the tab which is currently on top (hence of the widget which
 * is currently displayed, since all other widgets will be hidden).
 *
 * See also: @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_tabbed_layout new_ml_tabbed_layout (pool pool);
extern void ml_tabbed_layout_push_back (ml_tabbed_layout, const char *name, ml_widget);
extern ml_widget ml_tabbed_layout_pop_back (ml_tabbed_layout);
extern void ml_tabbed_layout_push_front (ml_tabbed_layout, const char *name, ml_widget);
extern ml_widget ml_tabbed_layout_pop_front (ml_tabbed_layout);
extern ml_widget ml_tabbed_layout_get (ml_tabbed_layout, int i);
extern void ml_tabbed_layout_insert (ml_tabbed_layout, int i, const char *name, ml_widget);
extern void ml_tabbed_layout_replace (ml_tabbed_layout, int i, const char *name, ml_widget);
extern void ml_tabbed_layout_erase (ml_tabbed_layout, int i);
extern void ml_tabbed_layout_clear (ml_tabbed_layout);
extern int ml_tabbed_layout_size (ml_tabbed_layout);
extern void ml_tabbed_layout_pack (ml_tabbed_layout, const char *name, ml_widget);
extern void ml_tabbed_layout_bring_to_front (ml_tabbed_layout, int i);
extern void ml_tabbed_layout_send_to_back (ml_tabbed_layout, int i);
extern int ml_tabbed_layout_get_front_tab (ml_tabbed_layout);

#endif /* ML_TABBED_LAYOUT_H */
