/* Monolith menubar, menu, menuitem classes.
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
 * $Id: ml_menu.h,v 1.1 2002/11/30 15:55:47 rich Exp $
 */

/* Note that this file contains all classes related to menus, menubars,
 * menu items, etc.
 */

#ifndef ML_MENU_H
#define ML_MENU_H

#include "monolith.h"
#include "ml_widget.h"

struct ml_menubar;
typedef struct ml_menubar *ml_menubar;

struct ml_menu;
typedef struct ml_menu *ml_menu;

struct ml_menuitem_button;
typedef struct ml_menuitem_button *ml_menuitem_button;

struct ml_menuitem_separator;
typedef struct ml_menuitem_separator *ml_menuitem_separator;

/* Function: new_ml_menubar - monolith menu bar
 * Function: ml_menubar_push_back
 * Function: ml_menubar_pop_back
 * Function: ml_menubar_push_front
 * Function: ml_menubar_pop_front
 * Function: ml_menubar_get
 * Function: ml_menubar_insert
 * Function: ml_menubar_replace
 * Function: ml_menubar_erase
 * Function: ml_menubar_clear
 * Function: ml_menubar_size
 * Function: ml_menubar_pack
 *
 * The monolith menubar widget is part of monolith's support for
 * pull-down menus. The menubar appears along the top of the screen.
 * Normally you would pack it directly into a window
 * (see @ref{ml_window_pack(3)}), and then pack the main screen
 * of the application into the menubar, using @code{ml_menubar_pack}.
 *
 * To populate the menubar with menus, you need to create @code{ml_menu}
 * objects and insert them into the menubar using the functions described
 * in this manpage. See @ref{new_ml_menu(3)} for details on how to create
 * new menu objects.
 *
 * @code{new_ml_menubar} creates a new, empty menubar widget.
 *
 * Underlying the menubar is a simple c2lib vector, and the other
 * access functions use the same notation as the equivalent
 * c2lib @code{vector_*} functions. Go the SEE ALSO section below
 * to see how to manipulate menu items within a menubar.
 *
 * @code{ml_menubar_pack} packs a widget into the body area of the
 * menubar. Normally you would pack the main screen of the application
 * here.
 *
 * See also: @ref{new_ml_menu(3)},
 * @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_menubar new_ml_menubar (pool pool);
extern void ml_menubar_push_back (ml_menubar, const char *name, ml_menu);
extern ml_menu ml_menubar_pop_back (ml_menubar);
extern void ml_menubar_push_front (ml_menubar, const char *name, ml_menu);
extern ml_menu ml_menubar_pop_front (ml_menubar);
extern ml_menu ml_menubar_get (ml_menubar, int i);
extern void ml_menubar_insert (ml_menubar, int i, const char *name, ml_menu);
extern void ml_menubar_replace (ml_menubar, int i, const char *name, ml_menu);
extern void ml_menubar_erase (ml_menubar, int i);
extern void ml_menubar_clear (ml_menubar);
extern int ml_menubar_size (ml_menubar);
extern void ml_menubar_pack (ml_menubar, ml_widget);

/* Function: new_ml_menu - monolith pull down menu
 * Function: ml_menu_push_back
 * Function: ml_menu_push_back_button
 * Function: ml_menu_push_back_inactive_button
 * Function: ml_menu_push_back_separator
 * Function: ml_menu_pop_back
 * Function: ml_menu_push_front
 * Function: ml_menu_pop_front
 * Function: ml_menu_get
 * Function: ml_menu_insert
 * Function: ml_menu_replace
 * Function: ml_menu_erase
 * Function: ml_menu_clear
 * Function: ml_menu_size
 * Function: ml_menu_pack
 *
 * The monolith menu widget is part of monolith's support for
 * pull-down menus. Menu widgets are normally created and packed
 * into a menubar widget (see @ref{new_ml_menubar(3)}).
 *
 * Menus can contain different menu items:
 *
 * * Simple buttons. When picked by the user, these cause a callback
 * function to be invoked.
 *
 * * Inactive buttons.
 *
 * * Separators. Used to separate groups of buttons.
 *
 * * Submenus. Nested menus to any depth.
 *
 * (In future we will add other types of menu item, in particular check
 * boxes and radio buttons).
 *
 * To create a menu, call @code{new_ml_menu}. This makes a new, empty
 * menu.
 *
 * To populate a menu, you can create menuitem objects of the required
 * type and call @code{ml_menu_push_back}, @code{ml_menu_push_front},
 * @code{ml_menu_insert} or @code{ml_menu_replace} as required. However,
 * this class offers short-cut functions which do the job of creating
 * the menu item and inserting it in the menu. These are described
 * below:
 *
 * @code{ml_menu_push_back_button} creates a button menuitem and
 * appends it to the menu. It is exactly equivalent to calling
 * @code{new_ml_menuitem_button}, then @code{ml_menuitem_button_set_callback},
 * then @code{ml_menu_push_back}.
 *
 * @code{ml_menu_push_back_inactive_button} creates an inactive button
 * menuitem and appends it to the menu. It is exactly equivalent to calling
 * @code{new_ml_menuitem_button} followed by @code{ml_menu_push_back}.
 *
 * @code{ml_menu_push_back_separator} creates a separator menuitem
 * and appends it to the menu. It is exactly equivalent to calling
 * @code{new_ml_menuitem_separator} followed by @code{ml_menu_push_back}.
 *
 * @code{ml_menu_pack} is equivalent to @code{ml_menu_push_back}.
 *
 * See also: @ref{new_ml_menubar(3)}, @ref{new_ml_menuitem_button(3)},
 * @ref{new_ml_menuitem_separator(3)},
 * @ref{vector_push_back(3)}, @ref{vector_pop_back(3)},
 * @ref{vector_push_front(3)}, @ref{vector_pop_front(3)},
 * @ref{vector_get(3)}, @ref{vector_insert(3)}, @ref{vector_replace(3)},
 * @ref{vector_erase(3)}, @ref{vector_clear(3)}, @ref{vector_size(3)}.
 */
extern ml_menu new_ml_menu (pool pool);
extern void ml_menu_push_back (ml_menu, ml_widget menu_or_menuitem);
extern void ml_menu_push_back_button (ml_menu, const char *text, void (*fn) (ml_session, void *), ml_session session, void *data);
extern void ml_menu_push_back_inactive_button (ml_menu, const char *text);
extern void ml_menu_push_back_separator (ml_menu);
extern ml_widget ml_menu_pop_back (ml_menu);
extern void ml_menu_push_front (ml_menu, ml_widget menu_or_menuitem);
extern ml_widget ml_menu_pop_front (ml_menu);
extern ml_widget ml_menu_get (ml_menu, int i);
extern void ml_menu_insert (ml_menu, int i, ml_widget menu_or_menuitem);
extern void ml_menu_replace (ml_menu, int i, ml_widget menu_or_menuitem);
extern void ml_menu_erase (ml_menu, int i);
extern void ml_menu_clear (ml_menu);
extern int ml_menu_size (ml_menu);
extern void ml_menu_pack (ml_menu, ml_widget menu_or_menuitem);

/* Function: new_ml_menuitem_button
 * Function: ml_menuitem_button_set_callback
 * Function: new_ml_menuitem_separator
 *
 * Monolith menuitem widgets are part of monolith's support for
 * pull-down menus. Menuitem widgets are individual menu entries which
 * are normally created and packed into menu widgets
 * (see @ref{new_ml_menu(3)}).
 *
 * Menuitem buttons are active or inactive buttons which when clicked
 * cause a callback function to be invoked. @code{new_ml_menuitem_button}
 * creates a new button with the given text.
 * @code{ml_menuitem_button_set_callback} sets the callback function
 * on the button to @code{fn}. If the function is set to @code{NULL}, then
 * the button becomes inactive (see also @ref{new_ml_button(3)}).
 *
 * Menuitem separators can be used to group logically similar sets of
 * buttons. @code{new_ml_menuitem_separator} creates a new separator.
 *
 * See also: @ref{new_ml_menu(3)}, and the following convenience functions:
 * @ref{ml_menu_push_back_button(3)},
 * @ref{ml_menu_push_back_inactive_button(3)},
 * @ref{ml_menu_push_back_separator(3)},
 */
extern ml_menuitem_button new_ml_menuitem_button (pool pool, const char *text);
extern void ml_menuitem_button_set_callback (ml_menuitem_button, void (*fn) (ml_session, void *), ml_session session, void *data);
extern ml_menuitem_separator new_ml_menuitem_separator (pool pool);

#endif /* ML_MENU_H */
