/* Monolith login widget (email validation, no password).
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
 * $Id: ml_login_nopw.c,v 1.7 2002/12/01 14:58:30 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <pool.h>
#include <pstring.h>
#include <pthr_iolib.h>
#include <pthr_dbi.h>
#include <pthr_cgi.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_flow_layout.h"
#include "ml_table_layout.h"
#include "ml_text_label.h"
#include "ml_button.h"
#include "ml_window.h"
#include "ml_form.h"
#include "ml_form_text.h"
#include "ml_form_submit.h"
#include "ml_dialog.h"
#include "ml_login_nopw.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations login_nopw_ops =
  {
    repaint: repaint
  };

struct ml_login_nopw
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_session session;		/* Current session. */
  ml_dbh_factory dbf;		/* Database factory. */

  /* The following is displayed when no user is logged in: */
  ml_button out;

  /* The following is displayed when a user is logged in: */
  ml_flow_layout in;
  ml_text_label in_name;

  /* Form input for login form. */
  ml_form_text email_input;

  /* The email callback action - we need to unregister this before it
   * is re-registered, for security reasons.
   */
  const char *actionid;

  /* The email address which is trying to be registered. */
  const char *email;

  /* Secret string sent in the email. */
  const char *secret;
};

/* Callback function prototypes. */
static void login_button (ml_session, void *);
static void login_send_email (ml_session, void *);
static void login (ml_session, void *);
static void logout_button (ml_session, void *);

ml_login_nopw
new_ml_login_nopw (pool pool, ml_session session, ml_dbh_factory dbf)
{
  ml_login_nopw w = pmalloc (pool, sizeof *w);
  ml_button button;

  w->ops = &login_nopw_ops;
  w->pool = pool;
  w->session = session;
  w->dbf = dbf;
  w->actionid = 0;
  w->email = 0;
  w->secret = 0;

  /* Generate the widget which is displayed when no user is logged in: */
  w->out = new_ml_button (pool, "Login ...");
  ml_button_set_callback (w->out, login_button, session, w);
  ml_button_set_popup (w->out, "login_nopw_window");
  ml_button_set_popup_size (w->out, 400, 300);

  /* Generate the widget which is displayed when a user is logged in: */
  w->in = new_ml_flow_layout (pool);

  w->in_name = new_ml_text_label (pool, 0);
  ml_widget_set_property (w->in_name, "font.weight", "bold");
  ml_flow_layout_pack (w->in, w->in_name);

  button = new_ml_button (pool, "Logout");
  ml_button_set_callback (button, logout_button, session, w);
  ml_flow_layout_pack (w->in, button);

  return w;
}

static void
login_button (ml_session session, void *vw)
{
  ml_login_nopw w = (ml_login_nopw) vw;
  ml_window win;
  ml_form form;
  ml_table_layout tbl;
  ml_text_label text;
  ml_form_submit submit;

  /* Create the login window. */
  win = new_ml_window (session, w->pool);
  form = new_ml_form (w->pool);
  tbl = new_ml_table_layout (w->pool, 2, 2);

  ml_form_set_callback (form, login_send_email, session, w);
  ml_widget_set_property (form, "method", "GET");

  text = new_ml_text_label
    (w->pool,
     "To log in to this site, or to create a user account, please type "
     "your email address in the box below.\n\n"
     "You will be sent a single confirmation email which contains a "
     "special link that gives you access to the site.\n\n"
     "Your email address is stored in our database, but you will not "
     "receive any further emails, nor will your email address be shared "
     "with others or displayed on the site (unless you specifically "
     "ask for this in your settings).\n\n");
  ml_table_layout_pack (tbl, text, 0, 0);
  ml_table_layout_set_colspan (tbl, 0, 0, 2);

  w->email_input = new_ml_form_text (w->pool, form);
  ml_table_layout_pack (tbl, w->email_input, 1, 0);

  submit = new_ml_form_submit (w->pool, form, "Submit");
  ml_table_layout_pack (tbl, submit, 1, 1);

  ml_form_pack (form, tbl);
  ml_window_pack (win, form);
}

static inline const char *
generate_secret (pool pool)
{
  int fd, i;
  unsigned char buffer[16];
  char *secret = pmalloc (pool, 33 * sizeof (char));

  fd = open ("/dev/urandom", O_RDONLY);
  if (fd == -1) abort ();
  if (read (fd, buffer, 16) != 16) abort ();
  close (fd);

  for (i = 0; i < 16; ++i)
    sprintf (secret + i*2, "%02x", buffer[i]);

  return secret;
}

static const char *clean_up_string (pool pool, const char *text);

static void
login_send_email (ml_session session, void *vw)
{
  ml_login_nopw w = (ml_login_nopw) vw;
  const char *email_c, *windowid;
  char *email;
  io_handle sendmail;
  const char *sendmail_cmd
    = "/usr/sbin/sendmail -t -i -f do_not_reply@annexia.org";
  const char *site = ml_session_host_header (session);
  const char *canonical_path = ml_session_canonical_path (session);
  ml_window win;
  ml_dialog dlg;

  /* Create the window, and get the windowid which is passed in the
   * email.
   */
  win = new_ml_window (session, w->pool);
  windowid = _ml_window_get_windowid (win);

  /* Get the email address which was typed in, and tidy it up a bit. */
  email_c = ml_form_input_get_value (w->email_input);
  if (!email_c) return;
  email = pstrdup (w->pool, email_c);
  ptrim (email);
  pstrlwr (email);
  if (strlen (email) == 0) return;
  if (strchr (email, '@') == 0) return;
  w->email = clean_up_string (w->pool, email);

  /* Action IDs are predictable, so if all we did was to send back
   * an action ID, then this would not offer security, since someone
   * could guess the next action ID, and thus register as another
   * user. Thus we also generate and send back a secret random string,
   * and in the login() function we check that the secret string
   * was passed back to us, proving that the user really received
   * the email.
   */
  w->secret = generate_secret (w->pool);

  /* Unregister old actionid, if there was one. For security reasons, since
   * otherwise a user would be able to register as anyone in the following
   * way:
   * (1) Request registration for realname@example.com
   * (2) Request registration for rich@annexia.org
   * (3) Click confirmation of email from (1)
   * (4) Registered as 'rich@annexia.org'!
   * By unregistering the action, we remove this possibility (hopefully,
   * at least, but if anyone else can think of a way around this, please
   * tell me).
   */
  if (w->actionid) ml_unregister_action (session, w->actionid);

  /* Register a callback action. */
  w->actionid = ml_register_action (session, login, w);

  /* Run sendmail. */
  sendmail = io_popen (sendmail_cmd, "w");
  if (!sendmail)
    pth_die ("could not invoke sendmail");

  io_fprintf
    (sendmail,
     "X-Monolith-Trace: %s %s %s\n"
     "From: DO NOT REPLY TO THIS EMAIL <do_not_reply@annexia.org>\n"
     "To: %s\n"
     "Subject: Email validation from %s\n"
     "\n"
     "Someone, possibly you, typed your email address into %s.\n"
     "If this was not you, then our sincere apologies; please ignore this\n"
     "e-mail.\n"
     "\n"
     "To complete the log in to the site, you need to visit the link\n"
     "below:\n"
     "\n"
     "http://%s%s?ml_window=%s&ml_action=%s&secret=%s\n"
     "\n"
     "Do not reply to this email!\n"
     "\n"
     "Notes for geeks:\n"
     "\n"
     "1. You need to use the same browser to fetch this URL. So 'wget'\n"
     "   won't work. Instead, paste the URL if you cannot click on it\n"
     "   directly.\n"
     "2. This URL is valid until your current session ends.\n",
     ml_session_get_peernamestr (session),
     ml_session_host_header (session),
     ml_session_canonical_path (session),
     email, site, site, site, canonical_path, windowid, w->actionid,
     w->secret);

  io_pclose (sendmail);

  /* Display a confirmation page. */
  dlg = new_ml_dialog (w->pool);
  ml_dialog_set_text
    (dlg,
     "A confirmation email was sent.\n\n"
     "Click on the link in the email to complete your site registration / "
     "login.");
  ml_dialog_add_close_button (dlg, "Close window", 0);
}

/* Remove CRs and LFs from the string. */
static const char *
clean_up_string (pool pool, const char *text)
{
  if (strpbrk (text, "\n\r"))
    {
      char *copy = pstrdup (pool, text);
      char *t = copy;

      while ((t = strpbrk (t, "\n\r")) != 0)
	*t++ = ' ';

      return copy;
    }
  else
    return text;		/* String is safe. */
}

static void
login (ml_session session, void *vw)
{
  ml_login_nopw w = (ml_login_nopw) vw;
  cgi submitted_args;
  const char *secret;
  db_handle dbh;
  st_handle sth;
  int userid;
  ml_window win;
  ml_dialog dlg;

  /* Before proceeding, check the secret. */
  submitted_args = _ml_session_submitted_args (session);
  secret = cgi_param (submitted_args, "secret");

  if (!secret || strlen (secret) == 0 ||
      !w->secret || strcmp (secret, w->secret) != 0)
    {
      /* Failed. */
      /* XXX Eventually ml_dialog will have a close window button. */
      win = new_ml_window (session, w->pool);
      dlg = new_ml_dialog (w->pool);
      ml_dialog_set_text
	(dlg,
	 "Email validation failed.\n\n"
	 "If you copied and pasted the URL out of an email message, please "
	 "make sure you copied it correctly.");
      ml_window_pack (win, dlg);

      return;
    }

  /* Secret is OK. Email address is valid. Log in or create a user account. */
  dbh = ml_get_dbh (session, w->dbf);

  sth = st_prepare_cached
    (dbh,
     "select userid from ml_users where email = ?", DBI_STRING);
  st_execute (sth, w->email);

  st_bind (sth, 0, userid, DBI_INT);

  if (st_fetch (sth))		/* Existing account. */
    {
      sth = st_prepare_cached
	(dbh,
	 "update ml_users set lastlogin_date = current_date, "
	 "nr_logins = nr_logins + 1 where userid = ?", DBI_INT);
      st_execute (sth, userid);
    }
  else				/* New account. */
    {
      char *username, *t;

      /* Extract the username from the email address. Usernames are not
       * unique (email addresses are), so just use the first part of
       * the email address, before the first '@'.
       */
      username = pstrdup (w->pool, w->email);
      if (!(t = strchr (username, '@'))) abort ();
      *t = '\0';

      sth = st_prepare_cached
	(dbh,
	 "insert into ml_users (email, username, lastlogin_date, nr_logins) "
	 "values (?, ?, current_date, 1)",
	 DBI_STRING, DBI_STRING);
      st_execute (sth, w->email, username);

      /* Get the userid. */
      userid = st_serial (sth, "ml_users_userid_seq");
    }

  /* Commit changes to the database. */
  db_commit (dbh);

  ml_session_login (session, userid, "/", "+1y");

  /* Success. */
  /* XXX Eventually ml_dialog will have a close window button. */
  win = new_ml_window (session, w->pool);
  dlg = new_ml_dialog (w->pool);
  ml_dialog_set_text
    (dlg,
     "You are now logged into this site.");
  ml_window_pack (win, dlg);
}

static void
logout_button (ml_session session, void *vw)
{
  //ml_login_nopw w = (ml_login_nopw) vw;

  ml_session_logout (session, "/");
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_login_nopw w = (ml_login_nopw) vw;
  int userid;

  /* Depending on whether a user is currently logged in or not, we display
   * different things. This has to happen in the 'repaint' function
   * because the currently logged in user can change unexpectedly
   * when other people log in to other applications.
   */
  userid = ml_session_userid (session);

  if (!userid)			/* Not logged in. */
    {
      ml_widget_repaint (w->out, session, windowid, io);
    }
  else				/* Logged in. */
    {
      db_handle dbh;
      st_handle sth;
      const char *email = 0;

      /* Update the current user information string. */
      dbh = ml_get_dbh (session, w->dbf);

      sth = st_prepare_cached
	(dbh,
	 "select email from ml_users where userid = ?",
	 DBI_INT);
      st_execute (sth, userid);

      st_bind (sth, 0, email, DBI_STRING);

      st_fetch (sth);		/* XXX or die ... */

      ml_widget_set_property (w->in_name, "text", email);

      /* Repaint. */
      ml_widget_repaint (w->in, session, windowid, io);
    }
}
