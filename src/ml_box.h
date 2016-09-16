/* Monolith box class.
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
 * $Id: ml_box.h,v 1.1 2002/08/28 19:50:27 rich Exp $
 */

#ifndef ML_BOX_H
#define ML_BOX_H

#include <ml_widget.h>

struct ml_box;
typedef struct ml_box *ml_box;

/* Function: new_ml_box - monolith box widget
 * Function: ml_box_pack
 *
 * The monolith box widget encloses another widget inside a rectangular
 * outline.
 *
 * @code{new_ml_box} creates a new box widget.
 *
 * @code{ml_box_pack} packs another widget inside the box. A maximum of
 * one widget can be packed inside a box, so if you call this function
 * multiple times, then earlier widgets will be forgotten.
 */
extern ml_box new_ml_box (pool pool);
extern void ml_box_pack (ml_box, ml_widget);

#endif /* ML_BOX_H */
