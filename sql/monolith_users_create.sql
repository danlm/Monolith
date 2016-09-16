-- Create schema for monolith users.
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
-- $Id: monolith_users_create.sql,v 1.2 2002/11/15 20:46:05 rich Exp $
--
-- Depends: monolith_core

-- Monolith does not require that you use this table. You can modify
-- the schema in monolith_auth_create.sql to use your own users table
-- if you wish. However, many of the monolith standard widgets use
-- this table for accessing user information.

begin work;

create table ml_users
(
	userid serial,		-- Unique number for each user
	email text		-- Unique email address for each user
		constraint ml_users_email_nn
		not null,
	username varchar(32)	-- Displayed username (not necessarily unique)
		constraint ml_users_username_nn
		not null,
	password varchar(32),	-- Hashed password (not always required)

	-- Personal data.
	given_name text,	-- Forename (in western locales)
	family_name text,	-- Surname (in western locales)
	date_of_birth date,	-- Date of birth
	gender char(1)		-- Gender
		constraint ml_users_gender_ck
		check (gender in ('m', 'f')),

	-- Locale information.
	langcode char(8),	-- Language and modifiers
	timezone int4		-- POSIX Timezone
		references ml_timezones (id),
	countrycode char(2)	-- ISO country code
		references ml_countries (code),

	-- Accounting information.
	signup_date date	-- When the account was created
		default current_date
		constraint ml_users_signup_date_nn
		not null,
	lastlogin_date date,	-- When they last logged in
	nr_logins int4		-- Number of times they have logged in
		default 0
		constraint ml_users_nr_logins_nn
		not null,
	bad_logins int4		-- Since they last logged in, how many bad
				-- login attempts have been made
		default 0
		constraint ml_users_bad_logins_nn
		not null
);

create unique index ml_users_email_ui on ml_users (email);

-- Grant access to the webserver.

grant select, insert, update, delete on ml_users to nobody;
grant select, update on ml_users_userid_seq to nobody;

commit work;
