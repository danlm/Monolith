-- Drop schema for monolith resources.
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
-- $Id: monolith_resources_drop.sql,v 1.1 2002/10/22 12:31:02 rich Exp $

drop index ml_resources_name_ui;
drop table ml_resources;
drop sequence ml_resources_resid_seq;
