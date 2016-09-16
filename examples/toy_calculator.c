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
 * $Id: toy_calculator.c,v 1.3 2002/11/07 10:49:01 rich Exp $
 */

#include <string.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_table_layout.h"
#include "ml_text_label.h"
#include "ml_box.h"
#include "ml_button.h"
#include "ml_widget.h"

#include "toy_calculator.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations toy_calculator_ops =
  {
    repaint: repaint
  };

struct toy_calculator
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_text_label disp;		/* The display. */
  char digits[16];		/* Display digits (only 10+1 used). */
  double reg;			/* Hidden register. */
  int op;			/* Operation: PLUS, MINUS, TIMES, DIVIDE. */
  ml_box box;			/* The top-level box. */
};

/* Lots of callback functions for each button. */
static void press_button_0 (ml_session, void *);
static void press_button_1 (ml_session, void *);
static void press_button_2 (ml_session, void *);
static void press_button_3 (ml_session, void *);
static void press_button_4 (ml_session, void *);

static void press_button_5 (ml_session, void *);
static void press_button_6 (ml_session, void *);
static void press_button_7 (ml_session, void *);
static void press_button_8 (ml_session, void *);
static void press_button_9 (ml_session, void *);

static void press_button_DOT (ml_session, void *);
static void press_button_EQUALS (ml_session, void *);
static void press_button_PLUS (ml_session, void *);
static void press_button_MINUS (ml_session, void *);
static void press_button_TIMES (ml_session, void *);
static void press_button_DIVIDE (ml_session, void *);
static void press_button_CLEAR (ml_session, void *);
static void press_button_AC (ml_session, void *);

/* These are used as indexes in the b[] array. */
#define DOT 10
#define EQUALS 11
#define PLUS 12
#define MINUS 13
#define TIMES 14
#define DIVIDE 15
#define CLEAR 16
#define AC 17

toy_calculator
new_toy_calculator (pool pool, ml_session session)
{
  toy_calculator w;
  ml_box box;
  ml_table_layout tbl;
  ml_button b[18];
  ml_text_label disp;

  w = pmalloc (pool, sizeof *w);
  w->ops = &toy_calculator_ops;
  w->pool = pool;
  strcpy (w->digits, "0");
  w->reg = 0;
  w->op = 0;

  /* Create the box surrounding the calculator. */
  box = new_ml_box (pool);

  /* A table layout widget is used to arrange the buttons and the screen.
   * There are 6 rows, each with 4 columns.
   */
  tbl = new_ml_table_layout (pool, 6, 4);

  /* Create the numeric buttons. */
  b[0] = new_ml_button (pool, "0");
  ml_button_set_callback (b[0], press_button_0, session, w);
  ml_widget_set_property (b[0], "button.style", "key");
  b[1] = new_ml_button (pool, "1");
  ml_button_set_callback (b[1], press_button_1, session, w);
  ml_widget_set_property (b[1], "button.style", "key");
  b[2] = new_ml_button (pool, "2");
  ml_button_set_callback (b[2], press_button_2, session, w);
  ml_widget_set_property (b[2], "button.style", "key");
  b[3] = new_ml_button (pool, "3");
  ml_button_set_callback (b[3], press_button_3, session, w);
  ml_widget_set_property (b[3], "button.style", "key");
  b[4] = new_ml_button (pool, "4");
  ml_button_set_callback (b[4], press_button_4, session, w);
  ml_widget_set_property (b[4], "button.style", "key");

  b[5] = new_ml_button (pool, "5");
  ml_button_set_callback (b[5], press_button_5, session, w);
  ml_widget_set_property (b[5], "button.style", "key");
  b[6] = new_ml_button (pool, "6");
  ml_button_set_callback (b[6], press_button_6, session, w);
  ml_widget_set_property (b[6], "button.style", "key");
  b[7] = new_ml_button (pool, "7");
  ml_button_set_callback (b[7], press_button_7, session, w);
  ml_widget_set_property (b[7], "button.style", "key");
  b[8] = new_ml_button (pool, "8");
  ml_button_set_callback (b[8], press_button_8, session, w);
  ml_widget_set_property (b[8], "button.style", "key");
  b[9] = new_ml_button (pool, "9");
  ml_button_set_callback (b[9], press_button_9, session, w);
  ml_widget_set_property (b[9], "button.style", "key");

  /* Create the other buttons. */
  b[DOT] = new_ml_button (pool, ".");
  ml_button_set_callback (b[DOT], press_button_DOT, session, w);
  ml_widget_set_property (b[DOT], "button.style", "key");
  b[EQUALS] = new_ml_button (pool, "=");
  ml_button_set_callback (b[EQUALS], press_button_EQUALS, session, w);
  ml_widget_set_property (b[EQUALS], "button.style", "key");
  b[PLUS] = new_ml_button (pool, "+");
  ml_button_set_callback (b[PLUS], press_button_PLUS, session, w);
  ml_widget_set_property (b[PLUS], "button.style", "key");
  b[MINUS] = new_ml_button (pool, "-");
  ml_button_set_callback (b[MINUS], press_button_MINUS, session, w);
  ml_widget_set_property (b[MINUS], "button.style", "key");
  b[TIMES] = new_ml_button (pool, "x");
  ml_button_set_callback (b[TIMES], press_button_TIMES, session, w);
  ml_widget_set_property (b[TIMES], "button.style", "key");
  b[DIVIDE] = new_ml_button (pool, "/");
  ml_button_set_callback (b[DIVIDE], press_button_DIVIDE, session, w);
  ml_widget_set_property (b[DIVIDE], "button.style", "key");
  b[CLEAR] = new_ml_button (pool, "C");
  ml_button_set_callback (b[CLEAR], press_button_CLEAR, session, w);
  ml_widget_set_property (b[CLEAR], "button.style", "key");
  b[AC] = new_ml_button (pool, "AC");
  ml_button_set_callback (b[AC], press_button_AC, session, w);

  /* Create the display. */
  disp = new_ml_text_label (pool, "0");
  ml_widget_set_property (disp, "font.weight", "bold");
  ml_widget_set_property (disp, "font.size", "large");

  /* Pack the buttons and display into the table layout widget. */
  ml_table_layout_pack (tbl, disp, 0, 0);
  ml_table_layout_set_colspan (tbl, 0, 0, 4);
  ml_table_layout_set_align (tbl, 0, 0, "right");

  ml_table_layout_pack (tbl, b[CLEAR], 1, 2);
  ml_table_layout_pack (tbl, b[AC], 1, 3);

  ml_table_layout_pack (tbl, b[7], 2, 0);
  ml_table_layout_pack (tbl, b[8], 2, 1);
  ml_table_layout_pack (tbl, b[9], 2, 2);
  ml_table_layout_pack (tbl, b[DIVIDE], 2, 3);

  ml_table_layout_pack (tbl, b[4], 3, 0);
  ml_table_layout_pack (tbl, b[5], 3, 1);
  ml_table_layout_pack (tbl, b[6], 3, 2);
  ml_table_layout_pack (tbl, b[TIMES], 3, 3);

  ml_table_layout_pack (tbl, b[1], 4, 0);
  ml_table_layout_pack (tbl, b[2], 4, 1);
  ml_table_layout_pack (tbl, b[3], 4, 2);
  ml_table_layout_pack (tbl, b[MINUS], 4, 3);

  ml_table_layout_pack (tbl, b[DOT], 5, 0);
  ml_table_layout_pack (tbl, b[0], 5, 1);
  ml_table_layout_pack (tbl, b[EQUALS], 5, 2);
  ml_table_layout_pack (tbl, b[PLUS], 5, 3);

  /* Pack the table into the box. */
  ml_box_pack (box, tbl);

  /* Save the display widget in the widget structure so that the
   * callback functions can update it.
   */
  w->disp = disp;

  /* Save the box, so we can repaint. */
  w->box = box;

  return w;
}

static void press_button_N (toy_calculator, int);

static void
press_button_0 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 0);
}

static void
press_button_1 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 1);
}

static void
press_button_2 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 2);
}

static void
press_button_3 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 3);
}

static void
press_button_4 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 4);
}

static void
press_button_5 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 5);
}

static void
press_button_6 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 6);
}

static void
press_button_7 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 7);
}

static void
press_button_8 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 8);
}

static void
press_button_9 (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  press_button_N (w, 9);
}

static void
press_button_N (toy_calculator w, int n)
{
  int len;

  if (strcmp (w->digits, "0") == 0)
    w->digits[0] = '\0';

  len = strlen (w->digits);

  if ((strchr (w->digits, '.') && len < 11) || len < 10)
    {
      w->digits[len] = '0' + n;
      w->digits[len+1] = '\0';
      ml_widget_set_property (w->disp, "text", w->digits);
    }
}

static void
press_button_DOT (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;
  int len = strlen (w->digits);

  if (strchr (w->digits, '.') == 0 && len < 10)
    {
      strcat (w->digits, ".");
      ml_widget_set_property (w->disp, "text", w->digits);
    }
}

static void
press_button_EQUALS (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;
  double a;

  if (w->op)
    {
      sscanf (w->digits, "%lf", &a);

      if (w->op == PLUS)
	w->reg += a;
      else if (w->op == MINUS)
	w->reg -= a;
      else if (w->op == TIMES)
	w->reg *= a;
      else if (w->op == DIVIDE && a != 0)
	w->reg /= a;

      snprintf (w->digits, 10, "%g", w->reg);
      ml_widget_set_property (w->disp, "text", w->digits);
      w->op = 0;
    }
}

static void
press_button_PLUS (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, vw);

  sscanf (w->digits, "%lf", &w->reg);
  strcpy (w->digits, "0");	/* But DON'T update the label yet. */
  w->op = PLUS;
}

static void
press_button_MINUS (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, vw);

  sscanf (w->digits, "%lf", &w->reg);
  strcpy (w->digits, "0");	/* But DON'T update the label yet. */
  w->op = MINUS;
}

static void
press_button_TIMES (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, vw);

  sscanf (w->digits, "%lf", &w->reg);
  strcpy (w->digits, "0");	/* But DON'T update the label yet. */
  w->op = TIMES;
}

static void
press_button_DIVIDE (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, vw);

  sscanf (w->digits, "%lf", &w->reg);
  strcpy (w->digits, "0");	/* But DON'T update the label yet. */
  w->op = DIVIDE;
}

static void
press_button_CLEAR (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  strcpy (w->digits, "0");
  ml_widget_set_property (w->disp, "text", "0");
}

static void
press_button_AC (ml_session session, void *vw)
{
  toy_calculator w = (toy_calculator) vw;

  strcpy (w->digits, "0");
  w->reg = 0;
  ml_widget_set_property (w->disp, "text", "0");
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  toy_calculator w = (toy_calculator) vw;

  ml_widget_repaint (w->box, session, windowid, io);
}
