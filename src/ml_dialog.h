/* Monolith dialog class.
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
 * $Id: ml_dialog.h,v 1.3 2002/11/13 21:41:01 rich Exp $
 */

#ifndef ML_DIALOG_H
#define ML_DIALOG_H

#include "monolith.h"
#include "ml_window.h"

struct ml_dialog;
typedef struct ml_dialog *ml_dialog;

/* Function: new_ml_dialog - monolith dialog widget
 * Function: ml_dialog_set_text
 * Function: ml_dialog_get_text
 * Function: ml_dialog_set_title
 * Function: ml_dialog_get_title
 * Function: ml_dialog_set_icon
 * Function: ml_dialog_get_icon
 * Function: ml_dialog_clear_buttons
 * Function: ml_dialog_add_button
 * Function: ml_dialog_add_close_button
 *
 * A dialog is a widget for asking a user a question, and getting an
 * answer. It can also be used for presenting the user with
 * confirmation that some operation has been carried out, or for
 * presenting the user with an error message.
 *
 * Fundamentally, a dialog consists of some text, and a series of
 * zero or more buttons along the bottom of the widget. A dialog
 * can also have an optional title which appears along the top
 * and an optional icon which appears on the left of the window.
 *
 * Dialogs are almost always used packed directly into a window.
 *
 * Dialogs are actually "super-widgets", built up from fundamental
 * widgets like labels, buttons, and table layouts. However currently
 * none of the fundamental widgets are actually exposed through the
 * @code{ml_dialog_*} interface.
 *
 * @code{new_ml_dialog} creates a new dialog widget with no text, no
 * title, no icon and no buttons (ie. not very useful! - after calling
 * this you should immediately call @code{ml_dialog_set_text} and
 * probably @code{ml_dialog_add_button} too).
 *
 * @code{ml_dialog_(set|get)_text} updates the text in a dialog
 * widget. Although having text is not strictly mandatory, it is
 * highly advisable.
 *
 * @code{ml_dialog_(set|get)_title} changes the title. To have no
 * title, set the title to @code{NULL}.
 *
 * @code{ml_dialog_(set|get)_icon} changes the icon. To have no
 * icon, set the icon to @code{NULL}.
 *
 * @code{ml_dialog_clear_buttons} removes all the buttons from the
 * dialog.
 *
 * @code{ml_dialog_add_button} adds a single button to the dialog. If
 * there are already buttons attached to the dialog, then this adds
 * the new button on the right. The @code{text} which appears on
 * the button must be specified. When the button is pressed,
 * @code{callback_fn} will be called.
 *
 * @code{ml_dialog_add_close_button} adds a single close button to the
 * dialog. The @code{text} which appears on the button must be specified.
 * If @code{close_flags} contains @code{ML_DIALOG_CLOSE_RELOAD_OPENER}
 * then the close button will cause the opener window to reload (see
 * @ref{new_ml_close_button(3)}).
 *
 * See also: @ref{ml_ok_window(3)}, @ref{ml_error_window(3)}.
 */
extern ml_dialog new_ml_dialog (pool pool);
extern void ml_dialog_set_text (ml_dialog, const char *text);
extern const char *ml_dialog_get_text (ml_dialog);
extern void ml_dialog_set_title (ml_dialog, const char *title);
extern const char *ml_dialog_get_title (ml_dialog);
extern void ml_dialog_set_icon (ml_dialog, const char *icon);
extern const char *ml_dialog_get_icon (ml_dialog);
extern void ml_dialog_clear_buttons (ml_dialog);
extern void ml_dialog_add_button (ml_dialog, const char *text, void (*callback_fn) (ml_session, void *), ml_session session, void *data);
extern void ml_dialog_add_close_button (ml_dialog w, const char *text, int close_flags);

/* Function: ml_ok_window - confirmation and error windows
 * Function: ml_error_window
 *
 * @code{ml_ok_window} and @code{ml_error_window} are handy helper
 * functions which display either a confirmation of success, or error
 * window. They are just convenient wrappers around @code{new_ml_window}
 * and @code{new_ml_dialog}.
 *
 * @code{ml_ok_window} is a window which displays a confirmation of
 * success. The @code{session} argument is the current session. The
 * @code{text} is the text to display. @code{flags} is explained below.
 *
 * @code{ml_error_window} is a window which displays an error
 * message. The @code{session} argument is the current session. The
 * @code{text} is the text to display. @code{flags} is explained below.
 *
 * The @code{flags} argument is a list of the following flags:
 *
 * @code{ML_DIALOG_CLOSE_BUTTON}: If set, display a close window button.
 *
 * @code{ML_DIALOG_CLOSE_RELOAD_OPENER}: If set, refresh the opener
 * window on close (see @ref{new_ml_close_button(3)}).
 *
 * Both functions return the new window.
 *
 * See also: @ref{new_ml_window(3)}, @ref{new_ml_dialog(3)}.
 */
extern ml_window ml_ok_window (pool, ml_session, const char *text, int flags);
extern ml_window ml_error_window (pool, ml_session, const char *text, int flags);

#define ML_DIALOG_CLOSE_BUTTON        0x0001
#define ML_DIALOG_CLOSE_RELOAD_OPENER 0x0002

#endif /* ML_DIALOG_H */
