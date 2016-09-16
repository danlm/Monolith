/* Monolith user directory widget.
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
 * $Id: ml_user_directory.h,v 1.1 2002/11/13 20:46:38 rich Exp $
 */

#ifndef ML_USER_DIRECTORY_H
#define ML_USER_DIRECTORY_H

#include <pool.h>

#include "monolith.h"
#include "ml_form.h"

struct ml_user_directory;
typedef struct ml_user_directory *ml_user_directory;

/* Function: new_ml_user_directory - monolith user directory widget
 * Function: ml_user_directory_get_selection
 * Function: ml_user_directory_set_selection
 * Function: ml_user_directory_set_callback
 *
 * The user directory is a widget for picking a user on the system.
 * It is very primitive at the moment, but in the future will become
 * more advanced (able to cope with 100s-millions of people in the
 * directory) with search features and so on.
 *
 * The user directory supports picking single users only at the moment,
 * but we intend to extend it in future to support picking multiple
 * users.
 *
 * The user directory can be used either a form input or as a
 * standalone widget which works rather like a button.
 *
 * It requires the schema in @code{sql/ml_userdir_create.sql}.
 *
 * @code{new_ml_user_directory} creates a new user directory widget.
 * The @code{pool}, @code{session} and @code{dbf} arguments point
 * to a pool for allocations, the current session and a database
 * handle factory. The @code{form} argument may be either a form
 * object (see @ref{new_ml_form(3)}), or @code{NULL} if this widget
 * is not going to be used in a form. @code{userid} is the initially
 * selected user ID, or it may be @code{0} meaning that no user is
 * selected initially.
 *
 * @code{ml_user_directory_(get|set)_selection} reads or changes the
 * currently selected user ID.
 *
 * @code{ml_user_directory_set_callback} sets a function which is
 * called when the currently selected user is changed. Callbacks
 * should only be set when the widget is NOT in a form.
 *
 * When the widget is used in a form, then the currently selected user
 * ID is only available when the form is submitted (ie. in the form's
 * callback function). When the widget is not in a form, then you can
 * set a callback for the widget, and call
 * @code{ml_user_directory_get_selection} during this function.
 */
extern ml_user_directory new_ml_user_directory (pool pool, ml_session session, ml_dbh_factory dbf, ml_form form, int userid);
extern int ml_user_directory_get_selection (ml_user_directory w);
extern void ml_user_directory_set_selection (ml_user_directory w, int userid);
extern void ml_user_directory_set_callback (ml_user_directory w, void (*fn) (ml_session, void *), ml_session session, void *data);

#endif /* ML_USER_DIRECTORY_H */
