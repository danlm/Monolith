/* Animal, vegetable, mineral; or "20 questions"
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
 * $Id: 04_animal_vegetable_mineral.c,v 1.8 2003/02/08 15:53:32 rich Exp $
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <pool.h>
#include <pstring.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_window.h"
#include "ml_dialog.h"
#include "ml_form.h"
#include "ml_form_text.h"
#include "ml_form_textarea.h"
#include "ml_form_submit.h"
#include "ml_table_layout.h"
#include "ml_text_label.h"

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/* This is the location of the questions database. You might want to
 * put this somewhere more permanent than just in /tmp.
 */
#define DATABASE_FILE "/tmp/questions_database"

/* Private per-session data. */
struct data
{
  ml_window win;		/* Top-level window. */
  ml_dialog dialog;		/* Dialog widget. */
  ml_form_text input1;		/* Input fields on the final form. */
  ml_form_textarea input2;
  ml_form_submit input3, input4;
  int current_node;		/* The node we are currently displaying. */
};

static void start_game (ml_session, void *);

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  struct data *data;

  /* Create the per-session data area. */
  data = pmalloc (pool, sizeof *data);

  /* Create the top-level window. */
  data->win = new_ml_window (session, pool);

  /* Create the dialog widget. */
  data->dialog = new_ml_dialog (pool);
  ml_dialog_set_title (data->dialog, "Animal, Vegetable, Mineral game "
		                     "(a.k.a. \"20 questions\")");

  /* Start the game. This populates the initial dialog with the first
   * question.
   */
  start_game (session, data);

  /* Pack everything up. */
  ml_window_pack (data->win, data->dialog);
}

/* This data is shared between sessions. */
static pool avm_pool;		/* Pool for global allocations. */
static vector nodes;		/* List of nodes. */
static int a, v, m;		/* Index of animal, vegetable and mineral
				 * nodes in the list of nodes. */

static void avm_init (void) __attribute__((constructor));
static void avm_stop (void) __attribute__((destructor));

static void pressed_animal (ml_session, void *);
static void pressed_vegetable (ml_session, void *);
static void pressed_mineral (ml_session, void *);
static void pressed_yes (ml_session session, void *);
static void pressed_no (ml_session session, void *);
static void update_dialog (ml_session session, struct data *);
static void finished_game (ml_session session, void *);
static void unknown_thing (ml_session session, void *);
static void add_unknown_thing (ml_session session, void *);
static void save_database (void);
static int is_vowel (const char *);
static void remove_newlines (char *);

struct node
{
  int is_question;		/* Either a question or an answer node. */

  /* For question nodes, this is the text of the question. For answer
   * nodes, this is the expected answer word.
   */
  const char *text;

  /* For question nodes only, this is the index of the nodes to visit
   * if the user says 'yes' or 'no' respectively.
   */
  int yes, no;
};

static void
avm_init ()
{
  FILE *fp;
  struct node node;
  char buffer[512];

  avm_pool = new_subpool (global_pool);
  nodes = new_vector (avm_pool, struct node);

  fp = fopen (DATABASE_FILE, "r");
  if (fp == 0)
    {
      /* No initial database. Don't worry, just create one internally. */
      a = 1;
      v = 2;
      m = 3;

      /* Node 0 is a dummy node. */
      vector_push_back (nodes, node);

      /* Node 1 is the animal node. */
      node.is_question = 0;
      node.text = "horse";
      vector_push_back (nodes, node);

      /* Node 2 is the vegetable node. */
      node.is_question = 0;
      node.text = "leek";
      vector_push_back (nodes, node);

      /* Node 3 is the mineral node. */
      node.is_question = 0;
      node.text = "pumice stone";
      vector_push_back (nodes, node);

      return;
    }

  while (fgets (buffer, sizeof buffer, fp))
    {
      pchomp (buffer);

      if (vector_size (nodes) == 0) /* Node 0 is special. */
	{
	  if (sscanf (buffer, "q %d %d %d", &a, &v, &m)
	      != 3)
	    {
	      fprintf (stderr, "%s: database is corrupt\n", DATABASE_FILE);
	      abort ();
	    }
	  vector_push_back (nodes, node);

#if 0
	  fprintf (stderr, "a = %d, v = %d, m = %d\n",
		   a, v, m);
#endif
	}
      else
	{
	  if (buffer[0] == 'q')	/* Question node. */
	    {
	      int n;

	      node.is_question = 1;

	      if (sscanf (buffer, "q %d %d %n", &node.yes, &node.no, &n) < 2)
		{
		  fprintf (stderr, "%s: database is corrupt\n", DATABASE_FILE);
		  abort ();
		}

	      node.text = pstrdup (avm_pool, &buffer[n]);

	      vector_push_back (nodes, node);

#if 0
	      fprintf (stderr, "yes = %d, no = %d, text = %s\n",
		       node.yes, node.no, node.text);
#endif
	    }
	  else			/* Answer node. */
	    {
	      node.is_question = 0;

	      node.text = pstrdup (avm_pool, &buffer[2]);

	      vector_push_back (nodes, node);

#if 0
	      fprintf (stderr, "text = %s\n",
		       node.text);
#endif
	    }
	}
    }

#if 0
  fprintf (stderr, "finished\n");
#endif

  fclose (fp);
}

static void
avm_stop ()
{
  delete_pool (avm_pool);
}

static void
save_database ()
{
  FILE *fp;
  int i;
  struct node node;

  fp = fopen (DATABASE_FILE, "w");
  if (fp == 0)
    {
      perror (DATABASE_FILE);
      return;
    }

  /* Node 0 is special. */
  fprintf (fp, "q %d %d %d\n", a, v, m);

  /* Write out the other nodes in order. */
  for (i = 1; i < vector_size (nodes); ++i)
    {
      vector_get (nodes, i, node);

      if (node.is_question)
	fprintf (fp, "q %d %d %s\n", node.yes, node.no, node.text);
      else
	fprintf (fp, "a %s\n", node.text);
    }

  fclose (fp);
}

static void
start_game (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  data->current_node = 0;

  /* Ask the initial standard question. */
  ml_dialog_set_text (data->dialog,
		      "<p>Think of something, anything.</p>"
		      "<p>I will ask you questions about it "
		      "and try to guess what it is.</p>"
		      "<p>First question: Is it an animal, a vegetable "
		      "or a mineral?</p>");
  ml_dialog_clear_buttons (data->dialog);
  ml_dialog_add_button (data->dialog, "Animal",
			pressed_animal, session, data);
  ml_dialog_add_button (data->dialog, "Vegetable",
			pressed_vegetable, session, data);
  ml_dialog_add_button (data->dialog, "Mineral",
			pressed_mineral, session, data);
}

static void
pressed_animal (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  data->current_node = a;
  update_dialog (session, data);
}

static void
pressed_vegetable (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  data->current_node = v;
  update_dialog (session, data);
}

static void
pressed_mineral (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  data->current_node = m;
  update_dialog (session, data);
}

static void
pressed_yes (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;
  struct node node;

  vector_get (nodes, data->current_node, node);
  data->current_node = node.yes;
  update_dialog (session, data);
}

static void
pressed_no (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;
  struct node node;

  vector_get (nodes, data->current_node, node);
  data->current_node = node.no;
  update_dialog (session, data);
}

static void
update_dialog (ml_session session, struct data *data)
{
  pool pool = ml_session_pool (session);
  struct node node;

  /* Fetch the current node. */
  vector_get (nodes, data->current_node, node);

  if (node.is_question)		/* Question node. */
    {
      /* Present the dialog window. */
      ml_dialog_set_text (data->dialog, node.text);

      /* Update the buttons. */
      ml_dialog_clear_buttons (data->dialog);
      ml_dialog_add_button (data->dialog, "Yes",
			    pressed_yes, session, data);
      ml_dialog_add_button (data->dialog, "No",
			    pressed_no, session, data);
    }
  else				/* Answer node. */
    {
      /* Present the dialog window. */
      ml_dialog_set_text (data->dialog,
			  psprintf (pool,
				    "Is it %s %s?",
				    (!is_vowel (node.text) ? "a" : "an"),
				    node.text));

      /* Update the buttons. */
      ml_dialog_clear_buttons (data->dialog);
      ml_dialog_add_button (data->dialog, "Yes",
			    finished_game, session, data);
      ml_dialog_add_button (data->dialog, "No",
			    unknown_thing, session, data);
    }
}

static int
is_vowel (const char *text)
{
  while (*text)
    {
      if (isalpha ((int) *text))
	{
	  return *text == 'a' || *text == 'e' || *text == 'i' ||
	    *text == 'o' || *text == 'u' || *text == 'A' || *text == 'E' ||
	    *text == 'I' || *text == 'O' || *text == 'U';
	}

      text++;
    }
  return 0;
}

static void
finished_game (ml_session session, void *vp)
{
  struct data *data = (struct data *) vp;

  ml_dialog_set_text (data->dialog, "Thanks for playing!");
  ml_dialog_clear_buttons (data->dialog);
  ml_dialog_add_button (data->dialog, "Play again",
			start_game, session, data);
}

static void
unknown_thing (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  ml_form form;
  ml_table_layout tbl;
  ml_text_label text1, text2, text3;
  struct node node;

  vector_get (nodes, data->current_node, node);  /* Fetch the node. */

  /* Generate the form used to ask the user to input a new question. */
  form = new_ml_form (pool);
  ml_form_set_callback (form, add_unknown_thing, session, data);

  /* The form contains a table layout ... */
  tbl = new_ml_table_layout (pool, 6, 2);

  /* Populate the form. */
  text1 = new_ml_text_label (pool,
			     "I give up! What was the thing you were "
			     "thinking of?");
  ml_table_layout_pack (tbl, text1, 0, 0);
  ml_table_layout_set_colspan (tbl, 0, 0, 2);

  data->input1 = new_ml_form_text (pool, form);
  ml_table_layout_pack (tbl, data->input1, 1, 0);
  ml_table_layout_set_colspan (tbl, 1, 0, 2);

  text2 = new_ml_text_label
    (pool,
     psprintf (pool,
	       "Give me a simple yes/no question which I can "
	       "ask people, to distinguish between your thing "
	       "and %s %s.",
	       (!is_vowel (node.text) ? "a" : "an"),
	       node.text));
  ml_table_layout_pack (tbl, text2, 2, 0);
  ml_table_layout_set_colspan (tbl, 2, 0, 2);

  data->input2 = new_ml_form_textarea (pool, form, 4, 60);
  ml_table_layout_pack (tbl, data->input2, 3, 0);
  ml_table_layout_set_colspan (tbl, 3, 0, 2);

  text3 = new_ml_text_label
    (pool,
     "And if I asked the question above of someone, and they "
     "were thinking of your thing, would they answer Yes or No?");
  ml_table_layout_pack (tbl, text3, 4, 0);
  ml_table_layout_set_colspan (tbl, 4, 0, 2);

  data->input3 = new_ml_form_submit (pool, form, "Yes");
  ml_table_layout_pack (tbl, data->input3, 5, 0);

  data->input4 = new_ml_form_submit (pool, form, "No");
  ml_table_layout_pack (tbl, data->input4, 5, 1);

  /* Pack. */
  ml_form_pack (form, tbl);
  ml_window_pack (data->win, form);
}

static void
add_unknown_thing (ml_session session, void *vp)
{
  pool pool = ml_session_pool (session);
  struct data *data = (struct data *) vp;
  int old_ans, new_ans;
  struct node old_ans_node, new_q_node, new_ans_node;
  char *name, *question;
  int yes_button, no_button;

  vector_get (nodes, data->current_node, old_ans_node); /* Fetch the node. */

  /* Extract the values of the input fields. */
  name = pstrdup (pool, ml_form_input_get_value (data->input1));
  question = pstrdup (pool, ml_form_input_get_value (data->input2));
  yes_button = ml_form_input_get_value (data->input3) ? 1 : 0;
  no_button = ml_form_input_get_value (data->input4) ? 1 : 0;

  assert ((yes_button || no_button) && ! (yes_button && no_button));

  /* Remove newlines from the fields (replace with whitespace). This is
   * because the database file cannot handle newlines.
   */
  remove_newlines (name);
  remove_newlines (question);

  /* Update the tree. */
  old_ans = vector_size (nodes);
  vector_push_back (nodes, old_ans_node);

  new_ans = vector_size (nodes);
  assert (new_ans == old_ans + 1);
  new_ans_node.is_question = 0;
  new_ans_node.text = pstrdup (avm_pool, name);
  vector_push_back (nodes, new_ans_node);

  new_q_node.is_question = 1;
  new_q_node.text = pstrdup (avm_pool, question);
  new_q_node.yes = yes_button ? new_ans : old_ans;
  new_q_node.no = no_button ? new_ans : old_ans;
  vector_replace (nodes, data->current_node, new_q_node);

  save_database ();

  /* Replace the form with the original dialog. */
  ml_window_pack (data->win, data->dialog);

  /* Restart the game. */
  start_game (session, data);
}

static void
remove_newlines (char *str)
{
  while (*str)
    {
      if (*str == '\n') *str = ' ';
      str++;
    }
}
