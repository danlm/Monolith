/* A toy calculator
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
 * $Id: 02_toy_calculator.c,v 1.9 2002/11/07 10:49:00 rich Exp $
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

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/* Private per-session data. */
struct data
{
  ml_text_label disp;		/* The display. */
  char digits[16];		/* Display digits (only 10+1 used). */
  double reg;			/* Hidden register. */
  int op;			/* Operation: PLUS, MINUS, TIMES, DIVIDE. */
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

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;
  ml_window w;
  ml_box box;
  ml_table_layout tbl;
  ml_button b[18];
  ml_text_label disp;

  /* Create the private, per-session data area and save it in the
   * session object.
   */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  w = new_ml_window (session, pool);

  /* Create the box surrounding the calculator. */
  box = new_ml_box (pool);

  /* A table layout widget is used to arrange the buttons and the screen.
   * There are 6 rows, each with 4 columns.
   */
  tbl = new_ml_table_layout (pool, 6, 4);

  /* Create the numeric buttons. */
  b[0] = new_ml_button (pool, "0");
  ml_button_set_callback (b[0], press_button_0, session, data);
  ml_widget_set_property (b[0], "button.style", "key");
  b[1] = new_ml_button (pool, "1");
  ml_button_set_callback (b[1], press_button_1, session, data);
  ml_widget_set_property (b[1], "button.style", "key");
  b[2] = new_ml_button (pool, "2");
  ml_button_set_callback (b[2], press_button_2, session, data);
  ml_widget_set_property (b[2], "button.style", "key");
  b[3] = new_ml_button (pool, "3");
  ml_button_set_callback (b[3], press_button_3, session, data);
  ml_widget_set_property (b[3], "button.style", "key");
  b[4] = new_ml_button (pool, "4");
  ml_button_set_callback (b[4], press_button_4, session, data);
  ml_widget_set_property (b[4], "button.style", "key");

  b[5] = new_ml_button (pool, "5");
  ml_button_set_callback (b[5], press_button_5, session, data);
  ml_widget_set_property (b[5], "button.style", "key");
  b[6] = new_ml_button (pool, "6");
  ml_button_set_callback (b[6], press_button_6, session, data);
  ml_widget_set_property (b[6], "button.style", "key");
  b[7] = new_ml_button (pool, "7");
  ml_button_set_callback (b[7], press_button_7, session, data);
  ml_widget_set_property (b[7], "button.style", "key");
  b[8] = new_ml_button (pool, "8");
  ml_button_set_callback (b[8], press_button_8, session, data);
  ml_widget_set_property (b[8], "button.style", "key");
  b[9] = new_ml_button (pool, "9");
  ml_button_set_callback (b[9], press_button_9, session, data);
  ml_widget_set_property (b[9], "button.style", "key");

  /* Create the other buttons. */
  b[DOT] = new_ml_button (pool, ".");
  ml_button_set_callback (b[DOT], press_button_DOT, session, data);
  ml_widget_set_property (b[DOT], "button.style", "key");
  b[EQUALS] = new_ml_button (pool, "=");
  ml_button_set_callback (b[EQUALS], press_button_EQUALS, session, data);
  ml_widget_set_property (b[EQUALS], "button.style", "key");
  b[PLUS] = new_ml_button (pool, "+");
  ml_button_set_callback (b[PLUS], press_button_PLUS, session, data);
  ml_widget_set_property (b[PLUS], "button.style", "key");
  b[MINUS] = new_ml_button (pool, "-");
  ml_button_set_callback (b[MINUS], press_button_MINUS, session, data);
  ml_widget_set_property (b[MINUS], "button.style", "key");
  b[TIMES] = new_ml_button (pool, "x");
  ml_button_set_callback (b[TIMES], press_button_TIMES, session, data);
  ml_widget_set_property (b[TIMES], "button.style", "key");
  b[DIVIDE] = new_ml_button (pool, "/");
  ml_button_set_callback (b[DIVIDE], press_button_DIVIDE, session, data);
  ml_widget_set_property (b[DIVIDE], "button.style", "key");
  b[CLEAR] = new_ml_button (pool, "C");
  ml_button_set_callback (b[CLEAR], press_button_CLEAR, session, data);
  ml_widget_set_property (b[CLEAR], "button.style", "key");
  b[AC] = new_ml_button (pool, "AC");
  ml_button_set_callback (b[AC], press_button_AC, session, data);

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

  /* Pack the table into the box and the box into the window. */
  ml_box_pack (box, tbl);
  ml_window_pack (w, box);

  /* Save the display widget in the per-session data structure so
   * that the callback functions can update it.
   */
  data->disp = disp;
  strcpy (data->digits, "0");
  data->reg = 0;
  data->op = 0;
}

static void press_button_N (ml_session, int, void *);

static void
press_button_0 (ml_session session, void *vp)
{
  press_button_N (session, 0, vp);
}

static void
press_button_1 (ml_session session, void *vp)
{
  press_button_N (session, 1, vp);
}

static void
press_button_2 (ml_session session, void *vp)
{
  press_button_N (session, 2, vp);
}

static void
press_button_3 (ml_session session, void *vp)
{
  press_button_N (session, 3, vp);
}

static void
press_button_4 (ml_session session, void *vp)
{
  press_button_N (session, 4, vp);
}

static void
press_button_5 (ml_session session, void *vp)
{
  press_button_N (session, 5, vp);
}

static void
press_button_6 (ml_session session, void *vp)
{
  press_button_N (session, 6, vp);
}

static void
press_button_7 (ml_session session, void *vp)
{
  press_button_N (session, 7, vp);
}

static void
press_button_8 (ml_session session, void *vp)
{
  press_button_N (session, 8, vp);
}

static void
press_button_9 (ml_session session, void *vp)
{
  press_button_N (session, 9, vp);
}

static void
press_button_N (ml_session session, int n, void *vp)
{
  struct data *data = (struct data *) vp;
  int len;

  if (strcmp (data->digits, "0") == 0)
    data->digits[0] = '\0';

  len = strlen (data->digits);

  if ((strchr (data->digits, '.') && len < 11) || len < 10)
    {
      data->digits[len] = '0' + n;
      data->digits[len+1] = '\0';
      ml_widget_set_property (data->disp, "text", data->digits);
    }
}

static void
press_button_DOT (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;
  int len = strlen (data->digits);

  if (strchr (data->digits, '.') == 0 && len < 10)
    {
      strcat (data->digits, ".");
      ml_widget_set_property (data->disp, "text", data->digits);
    }
}

static void
press_button_EQUALS (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;
  double a;

  if (data->op)
    {
      sscanf (data->digits, "%lf", &a);

      if (data->op == PLUS)
	data->reg += a;
      else if (data->op == MINUS)
	data->reg -= a;
      else if (data->op == TIMES)
	data->reg *= a;
      else if (data->op == DIVIDE && a != 0)
	data->reg /= a;

      snprintf (data->digits, 10, "%g", data->reg);
      ml_widget_set_property (data->disp, "text", data->digits);
      data->op = 0;
    }
}

static void
press_button_PLUS (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, data);

  sscanf (data->digits, "%lf", &data->reg);
  strcpy (data->digits, "0");	/* But DON'T update the label yet. */
  data->op = PLUS;
}

static void
press_button_MINUS (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, data);

  sscanf (data->digits, "%lf", &data->reg);
  strcpy (data->digits, "0");	/* But DON'T update the label yet. */
  data->op = MINUS;
}

static void
press_button_TIMES (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, data);

  sscanf (data->digits, "%lf", &data->reg);
  strcpy (data->digits, "0");	/* But DON'T update the label yet. */
  data->op = TIMES;
}

static void
press_button_DIVIDE (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  /* Act like we just pressed the EQUALS key. */
  press_button_EQUALS (session, data);

  sscanf (data->digits, "%lf", &data->reg);
  strcpy (data->digits, "0");	/* But DON'T update the label yet. */
  data->op = DIVIDE;
}

static void
press_button_CLEAR (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  strcpy (data->digits, "0");
  ml_widget_set_property (data->disp, "text", "0");
}

static void
press_button_AC (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  strcpy (data->digits, "0");
  data->reg = 0;
  ml_widget_set_property (data->disp, "text", "0");
}
