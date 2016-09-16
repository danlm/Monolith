/* Monolith text label class.
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
 * $Id: ml_text_label.h,v 1.4 2003/01/11 16:39:10 rich Exp $
 */

#ifndef ML_TEXT_LABEL_H
#define ML_TEXT_LABEL_H

#include <ml_widget.h>

struct ml_text_label;
typedef struct ml_text_label *ml_text_label;

/* Function: new_ml_text_label - monolith text label widget
 *
 * Text labels are simple strings or paragraphs of text. Unlike
 * the @code{ml_label} widget, HTML sequences are escaped before
 * being sent to the browser, so @code{ml_text_label}s can contain
 * @code{<}, @code{>}, @code{&} and so on and these will be displayed
 * correctly.
 *
 * Text labels allow control over the style, size and other aspects
 * of how the text appears.
 *
 * @code{new_ml_text_label} creates a new text label widget.
 *
 * The following properties can be changed on text labels (see
 * @code{ml_widget_set_property(3)}):
 *
 * @code{text}: The text displayed on the label.
 *
 * @code{text.align}: The text-align style
 * of the text label. Common alignments are @code{"left"}, @code{"right"}
 * and @code{"justify"}.
 *
 * @code{color}: The color style (text color).
 *
 * @code{font.weight}: The font-weight style
 * of the text label. Common weights are @code{"normal"} and @code{"bold"}.
 *
 * @code{font.size}: The font-size style
 * of the text label. Common sizes are @code{"small"}, @code{"medium"}
 * and @code{"large"}.
 *
 * See also: @ref{new_ml_widget(3)}, W3C CSS2 recommendation.
 */
extern ml_text_label new_ml_text_label (pool, const char *text);

#endif /* ML_TEXT_LABEL_H */
