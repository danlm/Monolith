/* Monolith smart text library.
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
 * $Id: ml_smarttext.h,v 1.1 2002/12/08 17:29:17 rich Exp $
 */

#ifndef ML_SMARTTEXT_H
#define ML_SMARTTEXT_H

#include <pthr_iolib.h>

#include "monolith.h"

/* Function: ml_plaintext_print - convert text to HTML
 * Function: ml_plaintext_to_html
 * Function: ml_smarttext_print
 * Function: ml_smarttext_to_html
 * Function: ml_filterhtml_print
 * Function: ml_filterhtml_to_html
 * Function: ml_anytext_print
 * Function: ml_anytext_to_html
 *
 * The monolith "smart text" library is concerned with rendering
 * plain text, smart text and filtered HTML safely in the browser.
 *
 * Plain text is just ordinary text, in which we escape the HTML
 * special entities such as @code{<} and @code{>} so that the user
 * cannot put up arbitrary HTML.
 *
 * Smart text is ordinary text with user-friendly markup sequences
 * (see below) allow the user to easily make text bold, etc.
 *
 * Filtered HTML is a limited, safe subset of HTML.
 *
 * The library currently supports the following smart text
 * constructs:
 *
 * - URLs.
 *
 * - @code{mailto:} URLs are marked up with a special mail icon.
 *
 * - @code{*bold*}, @code{/italic/} and @code{=monospace=} text, but
 * only around simple text, and currently you cannot combine these
 * with other smart text markup inside the text.
 *
 * - Various smileys, including @code{:-)}, @code{:-(} and @code{:-P}.
 *
 * - @code{(C)}, @code{(R)}, @code{(TM)} are marked up as the appropriate
 * symbol.
 *
 * - @code{1/4}, @code{1/2}, @code{3/4} are marked up as fractions.
 *
 * @code{ml_plaintext_print} converts a string @code{text} containing
 * just plain text to HTML and writes it directly to @code{io}.
 *
 * @code{ml_plaintext_to_html} converts a string @code{text} containing
 * just plain text to HTML and returns this as a new string allocated
 * in @code{pool}.
 *
 * @code{ml_smarttext_print} converts a string @code{text} containing
 * smart text to HTML and writes it directly to @code{io}.
 *
 * @code{ml_smarttext_to_html} converts a string @code{text} containing
 * smart text to HTML and returns this as a new string allocated
 * in @code{pool}.
 *
 * @code{ml_filterhtml_print} converts a string @code{text} containing
 * filtered HTML to HTML and writes it directly to @code{io}.
 *
 * @code{ml_filterhtml_to_html} converts a string @code{text} containing
 * filtered HTML to HTML and returns this as a new string allocated
 * in @code{pool}.
 *
 * @code{ml_anytext_print} and @code{ml_anytext_to_html} can be used
 * on either plain text, smart text or filtered HTML, depending on the
 * value passed in the @code{type} argument, which must be one of
 * @code{'p'}, @code{'s'} or @code{'h'}.
 */
extern void ml_plaintext_print (io_handle io, const char *text);
extern const char *ml_plaintext_to_html (pool, const char *text);
extern void ml_smarttext_print (io_handle io, const char *text);
extern const char *ml_smarttext_to_html (pool, const char *text);
extern void ml_filterhtml_print (io_handle io, const char *text);
extern const char *ml_filterhtml_to_html (pool, const char *text);
extern void ml_anytext_print (io_handle io, const char *text, char type);
extern const char *ml_anytext_to_html (pool, const char *text, char type);

#endif /* ML_SMARTTEXT_H */
