/* Monolith bulletins (recent news spool).
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
 * $Id: ml_bulletins.h,v 1.3 2002/11/13 20:46:38 rich Exp $
 */

#ifndef ML_BULLETINS_H
#define ML_BULLETINS_H

#include <pool.h>

#include "monolith.h"

struct ml_bulletins;
typedef struct ml_bulletins *ml_bulletins;

/* Function: new_ml_bulletins - monolith bulletins (recent news spool)
 *
 * The bulletins widget is a database-backed recent news spool.
 * Designated administrators may insert short messages which
 * appear at the top of the widget. Older items appear below.
 * Ordinary users see the bulletins, newest at the top, and may
 * also navigate through older items of news using previous/next
 * buttons.
 *
 * Items are arranged into "sections", each with a unique name.
 * This allows you to have different bulletins in different applications,
 * or if building a website with MSP, on different pages in the site.
 *
 * News items are stored in a PostgreSQL database. You can find the
 * schema in the @code{sql/ml_bulletins_create.sql} file in the
 * source distribution.
 *
 * @code{new_ml_bulletins} creates a new widget. You must pass
 * a @code{pool} for allocation and the current @code{session}
 * object. For database access, a database factory @code{dbf}
 * must be passed in. The @code{section_name} is the name of the
 * section (from the @code{ml_bulletins_sections} table).
 *
 * This function returns the widget, or @code{NULL} if the section
 * could not be found in the database.
 */
extern ml_bulletins new_ml_bulletins (pool pool, ml_session session, ml_dbh_factory dbf, const char *section_name);

#endif /* ML_BULLETINS_H */
