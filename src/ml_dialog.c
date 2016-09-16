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
 * $Id: ml_dialog.c,v 1.4 2002/11/13 21:41:01 rich Exp $
 */

#include "config.h"

#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_table_layout.h"
#include "ml_flow_layout.h"
#include "ml_label.h"
#include "ml_text_label.h"
#include "ml_image.h"
#include "ml_button.h"
#include "ml_close_button.h"
#include "ml_dialog.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations dialog_ops =
  {
    repaint: repaint,
  };

struct ml_dialog
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */

  ml_table_layout tbl;		/* Top-level table layout. */
  ml_text_label title;		/* Title-bar. */
  ml_image icon;		/* Icon. */
  ml_label text;		/* Main text. */
  ml_flow_layout buttons;	/* Buttons. */
};

ml_dialog
new_ml_dialog (pool pool)
{
  ml_dialog w = pmalloc (pool, sizeof *w);

  w->ops = &dialog_ops;
  w->pool = pool;

  /* At the top level, a dialog is nothing more than a table layout. */
  w->tbl = new_ml_table_layout (pool, 3, 2);

  /* Title bar. */
  w->title = new_ml_text_label (pool, 0);
  ml_widget_set_property (w->title, "font.weight", "bold");
  ml_table_layout_pack (w->tbl, w->title, 0, 0);
  ml_table_layout_set_colspan (w->tbl, 0, 0, 2);

  /* Icon. */
  w->icon = new_ml_image (pool, 0);
  ml_table_layout_pack (w->tbl, w->icon, 1, 0);

  /* Text. */
  w->text = new_ml_label (pool, 0);
  ml_table_layout_pack (w->tbl, w->text, 1, 1);

  /* Buttons. */
  w->buttons = new_ml_flow_layout (pool);
  ml_table_layout_pack (w->tbl, w->buttons, 2, 0);
  ml_table_layout_set_colspan (w->tbl, 2, 0, 2);

  return w;
}

void
ml_dialog_set_text (ml_dialog w, const char *text)
{
  ml_widget_set_property (w->text, "text", text);
}

const char *
ml_dialog_get_text (ml_dialog w)
{
  const char *text;

  ml_widget_get_property (w->text, "text", text);
  return text;
}

void
ml_dialog_set_title (ml_dialog w, const char *title)
{
  ml_widget_set_property (w->title, "text", title);
}

const char *
ml_dialog_get_title (ml_dialog w)
{
  const char *title;

  ml_widget_get_property (w->title, "text", title);
  return title;
}

void
ml_dialog_set_icon (ml_dialog w, const char *icon)
{
  ml_widget_set_property (w->icon, "image", icon);
}

const char *
ml_dialog_get_icon (ml_dialog w)
{
  const char *icon;

  ml_widget_get_property (w->icon, "image", icon);
  return icon;
}

void
ml_dialog_clear_buttons (ml_dialog w)
{
  ml_flow_layout_clear (w->buttons);
}

void
ml_dialog_add_button (ml_dialog w, const char *text,
		      void (*callback_fn) (ml_session, void *),
		      ml_session session, void *data)
{
  ml_button b;

  /* Create a new button widget. */
  b = new_ml_button (w->pool, text);
  ml_button_set_callback (b, callback_fn, session, data);

  /* Pack it into the buttons flow layout. */
  ml_flow_layout_pack (w->buttons, b);
}

void
ml_dialog_add_close_button (ml_dialog w, const char *text, int flags)
{
  ml_close_button b;

  /* Create new button. */
  b = new_ml_close_button (w->pool, text);

  if ((flags & ML_DIALOG_CLOSE_RELOAD_OPENER))
    ml_widget_set_property (b, "button.reload-opener", 1);

  /* Pack it. */
  ml_flow_layout_pack (w->buttons, b);
}

ml_window
ml_ok_window (pool pool, ml_session session, const char *text, int flags)
{
  ml_window win;
  ml_dialog dlg;
  const char *title = "That operation was carried out successfully";

  win = new_ml_window (session, pool);
  dlg = new_ml_dialog (pool);

  ml_window_set_title (win, title);
  ml_dialog_set_text (dlg, text);
  ml_dialog_set_title (dlg, title);
  // ml_dialog_set_icon (dlg, ...);

  if ((flags & ML_DIALOG_CLOSE_BUTTON))
    ml_dialog_add_close_button (dlg, "Close window", flags);

  ml_window_pack (win, dlg);

  return win;
}

ml_window
ml_error_window (pool pool, ml_session session, const char *text, int flags)
{
  ml_window win;
  ml_dialog dlg;
  const char *title = "There was an error";

  win = new_ml_window (session, pool);
  dlg = new_ml_dialog (pool);

  ml_window_set_title (win, title);
  ml_dialog_set_text (dlg, text);
  ml_dialog_set_title (dlg, title);
  // ml_dialog_set_icon (dlg, ...);

  if ((flags & ML_DIALOG_CLOSE_BUTTON))
    ml_dialog_add_close_button (dlg, "Close window", flags);

  ml_window_pack (win, dlg);

  return win;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_dialog w = (ml_dialog) vw;

  ml_widget_repaint (w->tbl, session, windowid, io);
}
