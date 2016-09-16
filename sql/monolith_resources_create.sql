-- Create schema for monolith resources.
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
-- $Id: monolith_resources_create.sql,v 1.2 2002/11/15 20:46:05 rich Exp $
--
-- Depends: monolith_core

begin work;

create table ml_resources
(
	resid serial,
	name text		-- Unique name for each resource
		constraint ml_resources_name_nn
		not null
);

create unique index ml_resources_name_ui on ml_resources (name);

-- Grant access to the webserver.

grant select, insert, update, delete on ml_resources to nobody;
grant select, update on ml_resources_resid_seq to nobody;

commit work;
