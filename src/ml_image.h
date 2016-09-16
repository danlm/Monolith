/* Monolith image class.
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
 * $Id: ml_image.h,v 1.2 2002/10/30 21:03:03 rich Exp $
 */

#ifndef ML_IMAGE_H
#define ML_IMAGE_H

#include "monolith.h"

struct ml_image;
typedef struct ml_image *ml_image;

/* Function: new_ml_image - monolith image widget
 *
 * An image widget displays a graphical image, taken from a
 * particular source (in fact, it corresponds almost exactly
 * to the HTML @code{<img/>} element).
 *
 * The current implementation of @code{ml_image} is rather
 * immature. In future we will support image sizes, alt, title,
 * longdesc, and so on.
 *
 * @code{new_ml_image} creates a new image. You should supply
 * the @code{src} (source) for the image, which is an absolute or
 * relative link. If @code{src} is set to @code{NULL} then no
 * image is displayed.
 *
 * The following properties can be changed on images (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{image}: The source of the image. You may set this to
 * @code{NULL} to display no image at all.
 */
extern ml_image new_ml_image (pool pool, const char *image);

#endif /* ML_IMAGE_H */
