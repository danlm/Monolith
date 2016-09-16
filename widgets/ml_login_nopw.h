/* Monolith login widget (email validation, no password).
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
 * $Id: ml_login_nopw.h,v 1.1 2002/10/19 16:09:34 rich Exp $
 */

#ifndef ML_LOGIN_NOPW_H
#define ML_LOGIN_NOPW_H

#include <pool.h>

#include "monolith.h"

struct ml_login_nopw;
typedef struct ml_login_nopw *ml_login_nopw;

/* Function: new_ml_login_nopw - login widget (email validation, no password)
 *
 * Login widgets provide a place for users to login, logout and register
 * for the service. This particular login widget uses only email validation,
 * and doesn't require passwords (or onerous registration steps). A user
 * will see a box inviting them to type their email address. A confirmation
 * email is sent to the user containing a link which they click. This
 * authenticates them to the site, at which point they are logged in.
 * Logged in users are shown a logout button.
 *
 * The @code{ml_login_nopw} widget requires the user tables, from
 * @code{sql/monolith_users_create.sql}.
 *
 * @code{new_ml_login_nopw} creates a new login widget. @code{dbf} should
 * point to the PostgreSQL database configured with the user tables.
 */
extern ml_login_nopw new_ml_login_nopw (pool pool, ml_session session, ml_dbh_factory dbf);

#endif /* ML_LOGIN_NOPW_H */
