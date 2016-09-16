-- Create schema for ml_bulletins widget.
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
-- $Id: ml_bulletins_create.sql,v 1.7 2002/11/15 20:46:03 rich Exp $
--
-- Depends: monolith_core, monolith_users, monolith_auth, monolith_resources

begin work;

create table ml_bulletins_sections
(
	resid int4
		constraint ml_bulletins_sections_resid_pk
		primary key
		references ml_resources (resid)
		on delete cascade
);

create table ml_bulletins_posters
(
	sectionid int4		-- The section
		references ml_bulletins_sections (resid)
		on delete cascade,
	userid int4		-- The user who can post in this section
		references ml_users (userid)
		on delete cascade
);

create unique index ml_bulletins_posters_ui
	on ml_bulletins_posters (sectionid, userid);

create table ml_bulletins
(
	id serial,
	sectionid int4		-- Which section is this in?
		constraint ml_bulletins_sectionid_nn
		not null
		references ml_bulletins_sections (resid)
		on delete cascade,
	authorid int4
		constraint ml_bulletins_authorid_nn
		not null
		references ml_users (userid)
		on delete cascade,
	item text		-- The text body of the item
		constraint ml_bulletins_item_nn
		not null,
	item_type char(1)	-- Type: plain, smart, HTML
		constraint ml_bulletins_item_type_nn
		not null
		constraint ml_bulletins_item_type_ck
		check (item_type in ('p', 's', 'h')),
	posted_date timestamp	-- Date that this item was posted
		default current_timestamp
		constraint ml_bulletins_timestamp_nn
		not null,
	link text,		-- Optional link
	link_text text		-- Optional text on the link
);

create index ml_bulletins_sectionid_i on ml_bulletins (sectionid);

-- Allow the web server to access this table.
grant select, insert, update, delete on ml_bulletins_sections to nobody;
grant select, insert, delete on ml_bulletins_posters to nobody;
grant select, insert, update, delete on ml_bulletins to nobody;
grant select, update on ml_bulletins_id_seq to nobody;

commit work;