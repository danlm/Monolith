/* Monolith heading class.
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
 * $Id: ml_heading.h,v 1.2 2003/01/11 16:39:10 rich Exp $
 */

#ifndef ML_HEADING_H
#define ML_HEADING_H

#include <ml_widget.h>

struct ml_heading;
typedef struct ml_heading *ml_heading;

/* Function: new_ml_heading - monolith heading widget
 *
 * Headings are simple text strings which are displayed as headings
 * for sections of a document or page. There are six levels of
 * heading, from 1 to 6, with 1 being the top level.
 *
 * @code{new_ml_heading} creates a new heading widget. The
 * @code{level} parameter is the heading level (a number from
 * 1 to 6). The @code{text} parameter is the text which will
 * be displayed in the heading.
 *
 * The following properties can be changed on heading widgets (see
 * @code{ml_widget_set_property(3)}):
 *
 * @code{text}: The text displayed in the heading.
 *
 * @code{heading.level}: The level.
 */
extern ml_heading new_ml_heading (pool, int level, const char *text);

#endif /* ML_HEADING_H */
