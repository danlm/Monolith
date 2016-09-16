/* Monolith core code.
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
 * $Id: monolith.c,v 1.25 2003/01/31 18:03:33 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#include <pool.h>
#include <hash.h>
#include <vector.h>
#include <pstring.h>
#include <pthr_reactor.h>
#include <pthr_pseudothread.h>
#include <pthr_iolib.h>
#include <pthr_mutex.h>
#include <pthr_http.h>
#include <pthr_wait_queue.h>
#include <rws_request.h>

#include "ml_window.h"
#include "monolith.h"

#ifndef STRINGIFY
#ifdef HAVE_STRINGIZE
#define STRINGIFY(STRING) #STRING
#else
#define STRINGIFY(STRING) "STRING"
#endif
#endif /* ! STRINGIFY */

/* Define some RFC-compliant dates to represent past and future. */
#define DISTANT_PAST   "Thu, 01 Dec 1994 16:00:00 GMT"
#define DISTANT_FUTURE "Sun, 01 Dec 2030 16:00:00 GMT"

/* Headers which are sent to defeat caches. */
#define NO_CACHE_HEADERS "Cache-Control", "must-revalidate", \
                         "Expires", DISTANT_PAST, \
                         "Pragma", "no-cache"

/* These are the default session reaping parameters, all in seconds. */
#define SESSION_REAP_MIN 600
#define SESSION_REAP_MAX 3600
#define SESSION_REAP_INC 600

struct ml_session
{
  const char *sessionid;	/* Session ID (string of 32 hex digits). */
  pool session_pool;		/* Pool for this session. */
  mutex lock;			/* Lock for this session. */
  int hits;			/* Number of requests in this session. */
  reactor_time_t created, last_access; /* Session created, last accessed. */
  int reap_min;			/* Time to reap, if hits == 1. */
  int reap_max;			/* Maximum reap time. */
  int reap_inc;			/* Increment in reap time, per hit. */
  struct sockaddr_in original_ip; /* IP address of initial request. */
  cgi args;			/* Initial arguments. */
  cgi submitted_args;		/* Current arguments (short-lived). */
  rws_request rws_rq;		/* Current request (short-lived). */
  io_handle io;			/* Current IO handle (short-lived). */
  void (*app_main) (ml_session); /* Main entry point into the application. */
  ml_window current_window;	/* "Current" window for the application. */
  ml_window main_window;	/* Nominated main window for the application.*/
  shash windows;		/* Maps window IDs -> ml_window. */
  shash actions;		/* Maps action IDs -> callback functions. */
  const char *host_header;	/* Host header. */
  const char *canonical_path;	/* Full path to the script. */
  const char *script_name;	/* Just the name of the script. */
  const char *user_agent;	/* User agent. */
  hash dbhs;			/* Hash db_handle -> pools of
				 * handles given out in current session. */
  int userid;			/* Currently logged in user (0 = none). */
  const char *auth_cookie;      /* If set, send an auth cookie at the end
				 * of the current HTTP request. */
  const char *auth_cookie_path, *auth_cookie_expires;
  ml_dbh_factory auth_dbf;	/* Connection used for authentication. */
};

struct action
{
  void (*callback_fn) (ml_session, void *data);
  void *data;
};

static pool ml_pool;		/* Monolith library's own pool. */
static shash sessions;		/* Maps session IDs -> ml_session. */

/* Database handle factory. This stores a pool of database handles
 * for a given connection.
 */
struct ml_dbh_factory
{
  pool pool;			/* Pool for allocations. */
  const char *conninfo;		/* Connection info string. */
  int allocated;		/* Total number of handles open. */
  vector free_handles;		/* List of FREE handles. */
};

static shash dbh_factories;	/* Hash conninfo -> ml_dbh_factory. */

static void run_action (ml_session, const char *);
static int bad_request_error (rws_request rq, const char *text);
static int auth_to_userid (ml_session, const char *auth);
static void monolith_init (void) __attribute__ ((constructor));
static void monolith_stop (void) __attribute__ ((destructor));
static void kill_session (const char *sessionid);

static void
monolith_init ()
{
  ml_pool = new_subpool (global_pool);
  sessions = new_shash (ml_pool, ml_session);
  dbh_factories = new_shash (ml_pool, ml_dbh_factory);
}

static void
monolith_stop ()
{
  /* Note that this will also free up memory used by sessions, since
   * each session pool is a subpool of ml_pool.
   */
  delete_pool (ml_pool);
}

static inline void
kill_old_sessions ()
{
  static reactor_time_t last_reap = 0;

  /* Only reap sessions every 10s. This also ensures that only one thread
   * will try to kill sessions.
   */
  if (reactor_time - last_reap > 10000LL)
    {
      vector session_list;
      int i;

      last_reap = reactor_time;

      session_list
	= shash_values_in_pool (sessions, pth_get_pool (current_pth));

      for (i = 0; i < vector_size (session_list); ++i)
	{
	  ml_session session;
	  int reap_age;

	  vector_get (session_list, i, session);

	  /* Calculate the age before reaping. */
	  reap_age = session->reap_min +
	    (session->hits - 1) * session->reap_inc;
	  if (reap_age > session->reap_max)
	    reap_age = session->reap_max;

	  /* Session is older than this? */
	  if (reactor_time - session->last_access > reap_age * 1000LL)
	    {
#if 0
	      fprintf (stderr,
		       "reaping session ID %s\n"
		       "current time = %llu, last access = %llu, diff = %llu",
		       session->sessionid,
		       reactor_time,
		       session->last_access,
		       reactor_time - session->last_access
		       );
#endif
	      kill_session (session->sessionid);
	    }
	}
    }
}

static inline int
my_isxdigit (char c)
{
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static inline const char *
get_sessionid_from_cookie (http_request http_request)
{
  const char *sessionid;
  int i;

  sessionid = http_request_get_cookie (http_request, "ml_sessionid");
  if (sessionid)
    {
      /* Check that the cookie has a valid form (32 hex digits). If
       * not, just ignore it.
       */
      if (strlen (sessionid) == 32)
	{
	  for (i = 0; i < 32; ++i)
	    if (!my_isxdigit (sessionid[i]))
	      break;
	  return sessionid;
	}
    }

  return 0;
}

/* This function is so useful, it should be part of c2lib (XXX)
 * See also the other occurrence in ml_login_nopw.c
 */
static inline const char *
generate_sessionid (pool pool)
{
  int fd, i;
  unsigned char buffer[16];
  char *sessionid = pmalloc (pool, 33 * sizeof (char));

  fd = open ("/dev/urandom", O_RDONLY);
  if (fd == -1) abort ();
  if (read (fd, buffer, 16) != 16) abort ();
  close (fd);

  for (i = 0; i < 16; ++i)
    sprintf (sessionid + i*2, "%02x", buffer[i]);

  return sessionid;
}

static const char *
get_script_name (pool pool, const char *canonical_path)
{
  char *t = strrchr (canonical_path, '/');
  return t ? pstrdup (pool, t+1) : canonical_path;
}

int
ml_entry_point (rws_request rq, void (*app_main) (ml_session))
{
  pool thread_pool = pth_get_pool (current_pth);
  io_handle io = rws_request_io (rq);
  http_request http_request = rws_request_http_request (rq);
  const char *host_header = rws_request_host_header (rq);
  const char *canonical_path = rws_request_canonical_path (rq);
  const char *sessionid;
  cgi cgi;
  ml_session session;
  int send_sessionid = 0;
  http_response http_response;
  int close;
  const char *actionid, *windowid, *auth;

  /* Look for old sessions and kill them. */
  kill_old_sessions ();

  /* Get the sessionid, if there is one. */
  sessionid = get_sessionid_from_cookie (http_request);

  /* Parse the CGI parameters, and extract the monolith-specific
   * parameters. Note that these are parsed into the thread pool,
   * not into the session pool (we don't even know yet if we have
   * a session pool, so that's not an option).
   */
  cgi = new_cgi (thread_pool, http_request, io);

  /* If ml_reset passed, begin a new session regardless. */
  if (cgi_param (cgi, "ml_reset"))
    {
      /* But if there was an existing session, delete it now. */
      if (sessionid)
	kill_session (sessionid);

      sessionid = 0;
    }

  if (sessionid && shash_get (sessions, sessionid, session))
    {
      /* It's an existing, valid session. */

      /* Acquire the lock before accessing any parts of the session
       * structure.
       */
      mutex_enter (session->lock);

      /* Update the access time. */
      session->last_access = reactor_time;

      /* Hit. */
      session->hits++;

      /* Get the current window, from the ml_window parameter. If there
       * is no ml_window parameter (can happen when opening new windows),
       * then set current window to NULL and expect that the action will
       * set the current window.
       */
      session->current_window = session->main_window;
      windowid = cgi_param (cgi, "ml_window");
      if (windowid &&
	  ! shash_get (session->windows, windowid, session->current_window))
	{
	  return bad_request_error (rq,
				    psprintf (thread_pool,
					      "invalid window ID: %s",
					      windowid));
	}

      /* Set the rws_rq field to the current request. */
      session->rws_rq = rq;

      /* Set the current IO handle. */
      session->io = io;

      /* Get ready for sending back auth cookie. */
      session->auth_cookie = 0;

      /* If the userid is set, check to see if there is a "poison" cookie.
       * If so, then we log out the user.
       */
      if (session->userid != 0 &&
	  (auth = http_request_get_cookie (http_request, "ml_auth")) != 0
	  && strcmp (auth, "poison") == 0)
	{
	  session->userid = 0;
	}
      else if (session->userid == 0 &&
	       (auth = http_request_get_cookie (http_request, "ml_auth")) != 0)
	{
	  session->userid = auth_to_userid (session, auth);
	}

      /* If the ml_action parameter is given, invoke the appropriate
       * function.
       */
      actionid = cgi_param (cgi, "ml_action");

      /* Save the submitted args, for forms. */
      session->submitted_args = cgi;

      if (actionid)
	run_action (session, actionid);
    }
  else
    {
      /* Start a new session. */
      pool session_pool;
      socklen_t namelen;
      const char *ua;

      /* Create a pool for this session. */
      session_pool = new_subpool (ml_pool);

      /* Create some state for this session. */
      session = pmalloc (session_pool, sizeof *session);
      session->lock = new_mutex (session_pool);
      session->hits = 1;
      session->last_access = session->created = reactor_time;
      session->reap_min = SESSION_REAP_MIN;
      session->reap_max = SESSION_REAP_MAX;
      session->reap_inc = SESSION_REAP_INC;
      session->session_pool = session_pool;
      session->app_main = app_main;
      session->current_window = 0;
      session->main_window = 0;
      session->windows = new_shash (session_pool, ml_window);
      session->sessionid = sessionid = generate_sessionid (session_pool);
      session->actions = new_shash (session_pool, struct action);
      session->host_header = pstrdup (session_pool, host_header);
      session->canonical_path = pstrdup (session_pool, canonical_path);
      session->script_name =
	get_script_name (session_pool, session->canonical_path);

      /* Get the User-Agent header (if any). */
      ua = http_request_get_header (http_request, "User-Agent");
      session->user_agent = ua ? pstrdup (session_pool, ua) : 0;

      /* Get the IP address of the first request. */
      namelen = sizeof (session->original_ip);
      getpeername (io_fileno (io),
		   (struct sockaddr *) &session->original_ip, &namelen);

      /* Take the initial arguments and copy them into the
       * session pool, dropping the private ml_* parameters.
       */
      session->args = copy_cgi (session_pool, cgi);
      cgi_erase (session->args, "ml_reset");
      cgi_erase (session->args, "ml_window");
      cgi_erase (session->args, "ml_action");

      /* Set the rws_rq field to the current request. */
      session->rws_rq = rq;

      /* Set the current IO handle. */
      session->io = io;

      /* Initialize the list of database handles. */
      session->dbhs = new_hash (session_pool, db_handle, pool);

      /* No initial authentication connection. */
      session->auth_dbf = 0;

      /* See if there's an ml_auth cookie. If so, and it's valid, then
       * we initialize the session->userid from this. Otherwise we
       * set session->userid to 0.
       */
      if ((auth = http_request_get_cookie (http_request, "ml_auth")) != 0)
	session->userid = auth_to_userid (session, auth);
      else
	session->userid = 0;

      /* Get ready for sending back auth cookie. */
      session->auth_cookie = 0;

      /* Remember to send back the ml_sessionid cookie. */
      send_sessionid = 1;

      /* Save the session. */
      shash_insert (sessions, sessionid, session);

      /* Acquire the lock. (Actually we don't strictly need to do this
       * until after we have sent the cookie, but it makes the code
       * simpler).
       */
      mutex_enter (session->lock);

      /* Run the "main" program. */
      app_main (session);
    }

  if (! session->current_window)
    {
      return bad_request_error (rq, "no current window");
    }

  /* Begin the response. */
  http_response = new_http_response
    (thread_pool, http_request, io,
     _ml_window_get_response_code (session->current_window),
     _ml_window_get_response_name (session->current_window));

  http_response_send_headers (http_response,
			      /* Send headers to defeat caching. */
			      NO_CACHE_HEADERS,
			      /* End of headers. */
			      NULL);

  /* Send the session cookie if necessary. */
  if (send_sessionid)
    {
      http_response_send_header (http_response,
				 "Set-Cookie",
				 psprintf (thread_pool,
					   "ml_sessionid=%s; path=%s",
					   sessionid,
					   canonical_path));
    }

  /* Send the auth cookie if necessary. */
  if (session->auth_cookie)
    {
      http_response_send_header (http_response,
				 "Set-Cookie",
				 psprintf (thread_pool,
					   "ml_auth=%s; path=%s; expires=%s",
					   session->auth_cookie,
					   session->auth_cookie_path
					   ? : canonical_path,
					   session->auth_cookie_expires
					   ? : ""));

      session->auth_cookie = 0;
    }

  /* Send any additional headers required by the current window. */
  _ml_window_send_headers (session->current_window, thread_pool,
			   http_response);

  close = http_response_end_headers (http_response);

  if (!http_request_is_HEAD (http_request))
    {
      /* Display the main window. */
      _ml_window_repaint (session->current_window, session, io);
    }

  /* XXX We might need to recover database handles here, particularly
   * if we implement chunked encoding, and we start to process many
   * requests over the same connection.
   */

  /* Free the session lock. */
  mutex_leave (session->lock);

  return close;
}

/* Delete a session.
 *
 * Killing a session is a non-trivial task, because we must make sure
 * at all costs that no other thread is using the session structure, or
 * might be waiting on the mutex to enter the session.
 *
 * The procedure is as follows. If any step fails, then we need to go
 * back round to the top and try again. Eventually this function will
 * delete the session.
 *
 * - Acquire the mutex.
 * - Check if any other threads are waiting to enter the mutex.
 * - If none, then remove the session from the sessions hash (this ensures
 *   that no other thread will try to use the session - particularly
 *   important if the session deletion is protracted and involves I/O).
 * - Release the mutex (no other thread will try to acquire it).
 * - Delete the session pool, which invokes any session finalisers.
 */
static void
kill_session (const char *sessionid)
{
  ml_session session;

  /* Get the session structure. */
  if (!shash_get (sessions, sessionid, session))
    return;			/* No such session - ignore it. */

 again:
  /* Acquire the session lock. */
  mutex_enter (session->lock);

  /* Any other threads waiting to enter the mutex? */
  if (mutex_nr_sleepers (session->lock) > 0)
    {
      /* Release the lock and try again later. */
      mutex_leave (session->lock);
      pth_millisleep (100);	/* To be on the safe side ... */
      goto again;
    }

  /* Remove the session from the list of sessions. After this, no
   * other threads can find or enter this session.
   */
  assert (shash_erase (sessions, sessionid));

  /* Release the lock. */
  mutex_leave (session->lock);

  /* Finally, we can delete the thread. */
  delete_pool (session->session_pool);
}

pool
ml_session_pool (ml_session session)
{
  return session->session_pool;
}

cgi
ml_session_args (ml_session session)
{
  return session->args;
}

const char *
ml_session_sessionid (ml_session session)
{
  return session->sessionid;
}

const char *
ml_session_host_header (ml_session session)
{
  return session->host_header;
}

const char *
ml_session_canonical_path (ml_session session)
{
  return session->canonical_path;
}

const char *
ml_session_script_name (ml_session session)
{
  return session->script_name;
}

const char *
ml_session_user_agent (ml_session session)
{
  return session->user_agent;
}

void
ml_session_set_main_window (ml_session session, ml_window win)
{
  session->main_window = win;
}

ml_window
ml_session_get_main_window (ml_session session)
{
  return session->main_window;
}

int
ml_session_get_peername (ml_session session,
			 struct sockaddr *name, socklen_t *namelen)
{
  int s;

  s = io_fileno (session->io);
  return getpeername (s, name, namelen);
}

const char *
ml_session_get_peernamestr (ml_session session)
{
  struct sockaddr_in addr;
  socklen_t len = sizeof (addr);

  if (ml_session_get_peername (session, (struct sockaddr *) &addr, &len) == -1)
    {
      perror ("getpeername");
      return 0;
    }

  assert (addr.sin_family == AF_INET);
  return pstrdup (session->session_pool, inet_ntoa (addr.sin_addr));
}

const vector
_ml_get_sessions (pool pool)
{
  return shash_keys_in_pool (sessions, pool);
}

ml_session
_ml_get_session (const char *sessionid)
{
  ml_session session;

  if (shash_get (sessions, sessionid, session))
    return session;
  else
    return 0;
}

int
_ml_session_get_hits (ml_session session)
{
  return session->hits;
}

reactor_time_t
_ml_session_get_last_access (ml_session session)
{
  return session->last_access;
}

reactor_time_t
_ml_session_get_created (ml_session session)
{
  return session->created;
}

struct sockaddr_in
_ml_session_get_original_ip (ml_session session)
{
  return session->original_ip;
}

void *
_ml_session_get_app_main (ml_session session)
{
  return session->app_main;
}

const vector
_ml_session_get_windows (ml_session session, pool pool)
{
  return shash_keys_in_pool (session->windows, pool);
}

ml_window
_ml_session_get_window (ml_session session, const char *windowid)
{
  ml_window win;

  if (shash_get (session->windows, windowid, win))
    return win;
  else
    return 0;
}

const vector
_ml_session_get_actions (ml_session session, pool pool)
{
  return shash_keys_in_pool (session->actions, pool);
}

int
_ml_session_get_action (ml_session session, const char *actionid,
			void **fn_rtn, void **data_rtn)
{
  struct action action;

  if (shash_get (session->actions, actionid, action))
    {
      *fn_rtn = action.callback_fn;
      *data_rtn = action.data;
      return 1;
    }
  else
    return 0;
}

const vector
_ml_session_get_dbhs (ml_session session, pool pool)
{
  return hash_keys_in_pool (session->dbhs, pool);
}

ml_dbh_factory
_ml_session_get_auth_dbf (ml_session session)
{
  return session->auth_dbf;
}

const vector
_ml_get_dbh_factories (pool pool)
{
  return shash_keys_in_pool (dbh_factories, pool);
}

ml_dbh_factory
_ml_get_dbh_factory (const char *conninfo)
{
  ml_dbh_factory dbf;

  if (shash_get (dbh_factories, conninfo, dbf))
    return dbf;
  else
    return 0;
}

int
_ml_dbh_factory_get_nr_allocated_handles (ml_dbh_factory dbf)
{
  return dbf->allocated;
}

int
_ml_dbh_factory_get_nr_free_handles (ml_dbh_factory dbf)
{
  return vector_size (dbf->free_handles);
}

void
ml_session_release_lock (ml_session session)
{
  mutex_leave (session->lock);
}

void
ml_session_acquire_lock (ml_session session)
{
  mutex_enter (session->lock);

  /* We've probably been sleeping for a while, so update the last access
   * time to reflect this.
   */
  session->last_access = reactor_time;
}

cgi
_ml_session_submitted_args (ml_session session)
{
  return session->submitted_args;
}

ml_dbh_factory
new_ml_dbh_factory (ml_session session, const char *conninfo)
{
  ml_dbh_factory dbf;

  if (!shash_get (dbh_factories, conninfo, dbf))
    {
      pool dbf_pool = new_subpool (ml_pool);

      /* Create the factory. */
      dbf = pmalloc (dbf_pool, sizeof *dbf);
      dbf->pool = dbf_pool;
      dbf->conninfo = pstrdup (dbf_pool, conninfo);
      dbf->allocated = 0;
      dbf->free_handles = new_vector (dbf_pool, db_handle);

      shash_insert (dbh_factories, conninfo, dbf);
    }

  return dbf;
}

struct recover_dbh_args
{
  ml_session session;
  ml_dbh_factory dbf;
  db_handle dbh;
};

static void recover_dbh (void *vargs);

db_handle
ml_get_dbh (ml_session session, ml_dbh_factory dbf)
{
  db_handle dbh;
  pool pool;
  struct recover_dbh_args *args;

  /* If a free handle is available in the factory, grab it and return it. */
  if (vector_size (dbf->free_handles) > 0)
    {
      vector_pop_back (dbf->free_handles, dbh);

    finish:
      /* Allocate a pool to this handle, as a subpool of the thread pool, so
       * if the thread finishes without giving up the handle, we get it back.
       */
      pool = new_subpool (pth_get_pool (current_pth));
      args = pmalloc (pool, sizeof *args);
      args->session = session;
      args->dbf = dbf;
      args->dbh = dbh;
      pool_register_cleanup_fn (pool, recover_dbh, args);
      hash_insert (session->dbhs, dbh, pool);

      return dbh;
    }

  /* Allocate one.
   *
   * Note here that dbf->pool is a subpool of ml_pool, so when we
   * free up ml_pool, we will close the connections.
   */
  dbh = new_db_handle (dbf->pool,
		       dbf->conninfo, DBI_THROW_ERRORS);
  if (!dbh) pth_die (dbf->conninfo);
  dbf->allocated++;
  goto finish;
}

void
ml_put_dbh (ml_session session, db_handle dbh)
{
  pool pool;

  /* If this fails, then it's likely that they're trying to 'put' a
   * database handle which they don't own, or 'put'ting a handle twice.
   */
  assert (hash_get (session->dbhs, dbh, pool));

  /* This calls recover_dbh. */
  delete_pool (pool);
}

static void
recover_dbh (void *vargs)
{
  struct recover_dbh_args *args = (struct recover_dbh_args *) vargs;

  /* Roll back the handle. */
  db_rollback (args->dbh);

  /* Push it onto the list of free handles. */
  vector_push_back (args->dbf->free_handles, args->dbh);
  assert (hash_erase (args->session->dbhs, args->dbh));
}

static void get_auth_dbf (ml_session);
static const char *parse_expires_header (pool pool, const char *expires);

void
ml_session_login (ml_session session, int userid,
		  const char *path, const char *expires)
{
  pool thread_pool = pth_get_pool (current_pth);
  db_handle dbh;
  st_handle sth;
  const char *cookie;

  /* Parse the expires header. */
  expires = parse_expires_header (thread_pool, expires);

  get_auth_dbf (session);
  dbh = ml_get_dbh (session, session->auth_dbf);

  /* Generate a suitable cookie and insert it into the database. */
  cookie = generate_sessionid (thread_pool);
  sth = st_prepare_cached
    (dbh,
     "delete from ml_user_cookie where userid = ?",
     DBI_INT);
  st_execute (sth, userid);
  sth = st_prepare_cached
    (dbh,
     "insert into ml_user_cookie (userid, cookie) values (?, ?)",
     DBI_INT, DBI_STRING);
  st_execute (sth, userid, cookie);
  db_commit (dbh);
  ml_put_dbh (session, dbh);

  /* User is logged in. */
  session->userid = userid;

  /* Remember to send back a cookie. */
  session->auth_cookie = cookie;
  session->auth_cookie_path = path;
  session->auth_cookie_expires = expires;
}

void
ml_session_logout (ml_session session, const char *path)
{
  pool thread_pool = pth_get_pool (current_pth);
  db_handle dbh;
  st_handle sth;
  int old_userid = session->userid;
  const char *expires;

  /* Set the expires header. */
  expires = parse_expires_header (thread_pool, "+1y");

  /* If no one is actually logged in, do nothing. */
  if (!old_userid) return;

  get_auth_dbf (session);
  dbh = ml_get_dbh (session, session->auth_dbf);

  sth = st_prepare_cached
    (dbh,
     "delete from ml_user_cookie where userid = ?",
     DBI_INT);
  st_execute (sth, old_userid);
  db_commit (dbh);
  ml_put_dbh (session, dbh);

  /* User is logged out. */
  session->userid = 0;

  /* Remember to send back the poison cookie. */
  session->auth_cookie = "poison";
  session->auth_cookie_path = path;
  session->auth_cookie_expires = expires;
}

/* Convert auth cookie to userid, if possible. If not valid, returns 0. */
static int
auth_to_userid (ml_session session, const char *auth)
{
  db_handle dbh;
  st_handle sth;
  int userid, fetched;

  get_auth_dbf (session);
  dbh = ml_get_dbh (session, session->auth_dbf);

  sth = st_prepare_cached
    (dbh,
     "select userid from ml_user_cookie where cookie = ?",
     DBI_STRING);
  st_execute (sth, auth);

  st_bind (sth, 0, userid, DBI_INT);

  fetched = st_fetch (sth);

  ml_put_dbh (session, dbh);

  if (fetched)
    return userid;
  else
    return 0;
}

static void
get_auth_dbf (ml_session session)
{
  /* Check monolith is configured to handle user authentication. */
  if (!session->auth_dbf)
    {
      const char *conninfo;

      conninfo = ml_cfg_get_string (session, "monolith user database", 0);
      if (!conninfo)
	pth_die ("missing 'monolith user database' key "
		 "in the rws configuration file");

      session->auth_dbf = new_ml_dbh_factory (session, conninfo);
    }
}

/* XXX We should share this code with rws. */
static const char *
parse_expires_header (pool pool, const char *expires)
{
  char pm, unit;
  int length;

  if (expires == 0) return 0;

  /* Is it like "+1y"? */
  if (sscanf (expires, "%c%d%c", &pm, &length, &unit) == 3 &&
      (pm == '+' || pm == '-') &&
      length > 0 &&
      (unit == 's' || unit == 'm' || unit == 'h' ||
       unit == 'd' || unit == 'y'))
    {
      time_t t;
      struct tm *tm;
      char header[64];

      time (&t);

      if (pm == '+')
	{
	  switch (unit)
	    {
	    case 's': t += length; break;
	    case 'm': t += length * 60; break;
	    case 'h': t += length * (60 * 60); break;
	    case 'd': t += length * (60 * 60 * 24); break;
	    case 'y': t += length * (60 * 60 * 24 * 366); break;
	    }
	}
      else
	{
	  switch (unit)
	    {
	    case 's': t -= length; break;
	    case 'm': t -= length * 60; break;
	    case 'h': t -= length * (60 * 60); break;
	    case 'd': t -= length * (60 * 60 * 24); break;
	    case 'y': t -= length * (60 * 60 * 24 * 366); break;
	    }
	}

      tm = gmtime (&t);
      strftime (header, sizeof header, "%a, %d %b %Y %H:%M:%S GMT", tm);

      return pstrdup (pool, header);
    }

  /* Otherwise, assume it's in RFC 2616 format. */
  return expires;
}

int
ml_session_userid (ml_session session)
{
  return session->userid;
}

const char *
ml_cfg_get_string (ml_session session,
		   const char *key, const char *default_value)
{
  return rws_request_cfg_get_string (session->rws_rq, key, default_value);
}

int
ml_cfg_get_int (ml_session session, const char *key, int default_value)
{
  return rws_request_cfg_get_int (session->rws_rq, key, default_value);
}

int
ml_cfg_get_bool (ml_session session, const char *key, int default_value)
{
  return rws_request_cfg_get_bool (session->rws_rq, key, default_value);
}

void
_ml_session_set_current_window (ml_session session, ml_window window,
				const char *windowid)
{
  shash_insert (session->windows, windowid, window);
  session->current_window = window;
}

static void
run_action (ml_session session, const char *action_id)
{
  struct action a;

  /* Ignore unknown action IDs. */
  if (shash_get (session->actions, action_id, a))
    a.callback_fn (session, a.data);
}

const char *
ml_register_action (ml_session session,
		    void (*callback_fn) (ml_session, void *data), void *data)
{
  static int action_id = 1;
  const char *action_str;
  struct action a;

  action_str = pitoa (session->session_pool, action_id);
  action_id++;

  a.callback_fn = callback_fn;
  a.data = data;

  shash_insert (session->actions, action_str, a);

  return action_str;
}

void
ml_unregister_action (ml_session session, const char *action_id)
{
  shash_erase (session->actions, action_id);
}

#define CRLF "\015\012"

static int
bad_request_error (rws_request rq, const char *text)
{
  pool thread_pool = pth_get_pool (current_pth);
  io_handle io = rws_request_io (rq);
  http_request http_request = rws_request_http_request (rq);
  const char *canonical_path = rws_request_canonical_path (rq);
  http_response http_response;
  int close;

  http_response = new_http_response (thread_pool, http_request, io,
				     500, "Internal server error");
  http_response_send_headers (http_response,
			      /* Content type. */
			      "Content-Type", "text/html",
			      NO_CACHE_HEADERS,
			      /* End of headers. */
			      NULL);
  close = http_response_end_headers (http_response);

  if (http_request_is_HEAD (http_request)) return close;

  io_fprintf (io,
	      "<html><head><title>Internal server error</title></head>" CRLF
	      "<body bgcolor=\"#ffffff\">" CRLF
	      "<h1>500 Internal server error</h1>" CRLF
	      "<p>There was an error serving this request: %s</p>" CRLF
	      "<hr>" CRLF
	      "<p><a href=\"%s?ml_reset=1\">Restart your session</a></p>" CRLF
	      "</body></html>",
	      text, canonical_path);

  return close;
}
