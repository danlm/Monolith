#include <pool.h>

#include <monolith.h>
#include <ml_window.h>
#include <ml_text_label.h>

/*----- The following standard boilerplate code must appear -----*/

/* Main entry point to the app. */
static void app_main (ml_session);

int
handle_request (rws_request rq)
{
  return ml_entry_point (rq, app_main);
}

/*----- End of standard boilerplate code -----*/

static void
app_main (ml_session session)
{
  pool pool = ml_session_pool (session);
  ml_window w;
  ml_text_label text;

  /* Create the top-level window. */
  w = new_ml_window (session, pool);

  /* Create the text widget. */
  text = new_ml_text_label (pool, "Hello, World!");

  /* Pack the text widget into the window. */
  ml_window_pack (w, text);
}
