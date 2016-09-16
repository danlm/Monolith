/* Monolith label class.
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
 * $Id: ml_label.h,v 1.2 2002/10/30 21:03:03 rich Exp $
 */

#ifndef ML_LABEL_H
#define ML_LABEL_H

struct ml_label;
typedef struct ml_label *ml_label;

/* Function: new_ml_label - monolith label widget
 *
 * A label widget is a generic piece of HTML.
 *
 * @code{new_ml_label} creates a new label widget.
 *
 * The following properties can be changed on labels (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{text}: The HTML associated
 * with the label. Note that the text string must be either
 * static, or already allocated in the label's pool, or allocated
 * in a pool with a longer lifetime than the label. If the text
 * is set to @code{NULL} then this has the same effect as setting
 * the text to the empty string.
 */
extern ml_label new_ml_label (pool pool, const char *text);

#endif /* ML_LABEL_H */
