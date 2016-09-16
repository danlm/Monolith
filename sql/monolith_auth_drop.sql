-- Drop schema for monolith authentication table.
-- - by Richard W.M. Jones <rich@annexia.org>
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Library General Public
-- License as published by the Free Software Foundation; either
-- version 2 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Library General Public License for more details.
--
-- You should have received a copy of the GNU Library General Public
-- License along with this library; if not, write to the Free
-- Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
--
-- $Id: monolith_auth_drop.sql,v 1.1 2002/10/19 16:09:29 rich Exp $
--
-- Depends: monolith_core, monolith_users

-- This table is used by monolith itself when you use the authentication
-- functions (ml_session_login, ml_session_logout, ml_session_userid).
-- This table depends on a table (or view?) called ml_users. Normally
-- you would use the ml_users table defined in monolith_users_create.sql,
-- but if you prefer you can modify this file to point to your own users
-- table.

drop index ml_user_cookie_userid_ui;
drop index ml_user_cookie_cookie_ui;
drop table ml_user_cookie;
