/* Monolith widget class.
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
 * $Id: ml_widget.h,v 1.8 2002/11/29 10:43:03 rich Exp $
 */

#ifndef ML_WIDGET_H
#define ML_WIDGET_H

#include <stdarg.h>

#include <pthr_iolib.h>

struct ml_session;

/* Best to treat widgets as opaque, except in ml_widget.c itself where
 * we know that they in fact contain a pointer to the list of widget
 * operations.
 */
typedef void *ml_widget;

/* Widget property. */
struct ml_widget_property
{
  const char *name;		/* Name of the property. */
  int offset;			/* Offset into widget structure. */
  int type;			/* Property type. */
#define ML_PROP_STRING    1
#define ML_PROP_INT       2
#define ML_PROP_BOOL      ML_PROP_INT
#define ML_PROP_CHAR      3
#define ML_PROP_WIDGET    4
  void (*on_set) (ml_widget w); /* Called after property is set. */
  void (*on_get) (ml_widget w); /* Called before property is got. */
  int flags;			/* Flags. */
#define ML_PROP_READ_ONLY  1
#define ML_PROP_WRITE_ONLY 2
};

/* A pointer to this struct must occupy the first slot in every
 * widget-type structure we define elsewhere.
 */
struct ml_widget_operations
{
  /* All widgets must have a repaint function. */
  void (*repaint) (void *widget, struct ml_session *session,
		   const char *windowid, io_handle io);

  /* List of properties (NULL = no properties). */
  struct ml_widget_property *properties;
};

/* Function: ml_widget_repaint - Operations on generic monolith widgets.
 * Function: ml_widget_get_properties
 * Function: ml_widget_set_property
 * Function: ml_widget_get_property
 * Function: _ml_widget_get_property
 *
 * @code{ml_widget_repaint} calls the repaint function on a generic
 * widget.
 *
 * The @code{*property} functions are concerned with widget properties.
 * Properties are generic attributes of a widget which can be read and
 * updated using @code{ml_widget_set_property} and
 * @code{ml_widget_get_property}.
 *
 * @code{ml_widget_get_properties} returns a list of the properties
 * available to be changed in the current widget. The list returned
 * can be @code{NULL} meaning that this widget does not support
 * properties. @code{struct ml_widget_property} is defined in
 * @code{<ml_widget.h>}.
 *
 * Properties have consistent names. These are some of the property
 * names defined so far:
 *
 * @code{button.style}: For buttons, sets the style of the button,
 * either @code{"default"}, @code{"key"}, @code{"compact"} or
 * @code{"link"}.
 *
 * @code{class}: Many widgets support this. It sets or overrides the
 * class of the top-level HTML element. You can use this, in conjunction
 * with a stylesheet, to dramatically change the look and feel of a
 * particular widget. In fact, this is the recommended method for changing
 * the style for all widgets except text labels.
 *
 * @code{color}: The color style (text color).
 *
 * @code{font.size}: Font size for text displayed on a label or
 * button. Common sizes are @code{"small"}, @code{"medium"} or
 * @code{"large"}.
 *
 * @code{font.weight}: Font weight for text displayed on a label or
 * button. Common weights are @code{"normal"} and @code{"bold"}.
 *
 * @code{form.select.size}: On form select input boxes, the numbers of
 * rows.
 *
 * @code{form.select.multiple}: On form select input boxes, whether
 * the input is single- or multiple-select.
 *
 * @code{image}: On images, the path to the image.
 *
 * @code{text}: The text displayed on a label or button.
 *
 * @code{text.align}: Alignment of text displayed on a label or
 * button. Possible values are: @code{"left"}, @code{"right"},
 * @code{"justify"}.
 *
 * @code{title}: Many widgets support this to give the widget a
 * title or "tooltip".
 *
 */
extern void ml_widget_repaint (ml_widget widget, struct ml_session *, const char *windowid, io_handle);
extern const struct ml_widget_property *ml_widget_get_properties (ml_widget widget);
extern void ml_widget_set_property (ml_widget widget, const char *property_name, ...);
#define ml_widget_get_property(widget,property_name,var) (_ml_widget_get_property ((widget), (property_name), &(var)))
extern void _ml_widget_get_property (ml_widget widget, const char *property_name, void *varptr);

#endif /* ML_WIDGET_H */
