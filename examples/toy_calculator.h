/* Toy calculator from example 02 turned into a reusable widget.
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
 * $Id: toy_calculator.h,v 1.1 2002/09/07 13:46:58 rich Exp $
 */

#ifndef TOY_CALCULATOR_H
#define TOY_CALCULATOR_H

#include <pool.h>
#include "monolith.h"

struct toy_calculator;
typedef struct toy_calculator *toy_calculator;

/* Function: new_toy_calculator - toy calculator widget
 *
 * @code{new_toy_calculator} creates a new reusable toy calculator
 * widget.
 */
extern toy_calculator new_toy_calculator (pool, ml_session);

#endif /* TOY_CALCULATOR_H */
