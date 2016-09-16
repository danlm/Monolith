-- Create schema for monolith user directory.
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
-- $Id: ml_user_directory_create.sql,v 1.2 2002/11/15 20:46:03 rich Exp $
--
-- Depends: monolith_core, monolith_users

-- The user directory is strictly "opt-in". If a user is not listed in
-- this table, then they do not want to appear in the directory. We
-- need to refine this in the future so that the ML_USERS table contains
-- more information about what details people want to be revealed to
-- others.

begin work;

create table ml_userdir_prefs
(
	userid int4		-- User ID
		constraint ml_userdir_prefs_userid_pk
		primary key
		references ml_users (userid)
		on delete cascade
);

-- Grant access to the webserver.

grant select, insert, update, delete on ml_userdir_prefs to nobody;

commit work;