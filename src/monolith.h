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
 * $Id: monolith.h,v 1.18 2002/12/01 18:28:30 rich Exp $
 */

#ifndef MONOLITH_H
#define MONOLITH_H

#include <pool.h>
#include <vector.h>
#include <pthr_pseudothread.h>
#include <pthr_cgi.h>
#include <pthr_dbi.h>
#include <rws_request.h>

#include <stdlib.h>		/* For size_t */

/* Just like the semi-standard "offsetof" function. */
#ifdef offsetof
#define ml_offsetof(type,member) offset(type,member)
#else
#define ml_offsetof(type,member) ((size_t) &((type *)0)->member)
#endif

#include <sys/socket.h>

#include <ml_window.h>

/* Session object. */
struct ml_session;
typedef struct ml_session *ml_session;

/* Function: ml_entry_point - entry point into monolith from handle_request
 *
 * @code{ml_entry_point} is the entry point into the monolith core
 * library code, called from @code{handle_request}. The application's
 * @code{handle_request} function should contain just a single
 * call to @code{ml_entry_point} like this:
 *
 * @code{return ml_entry_point (rq, app_main);}
 *
 * where @code{rq} is the @code{rws_request} object passed by the
 * web server, and @code{app_main} is the application's main function.
 *
 * See @code{examples/01_label_and_button.c} for a simple monolith
 * application.
 *
 * See also: @ref{ml_session_pool(3)},
 * @ref{rws_request_pool(3)}, @ref{new_ml_window(3)},
 * @ref{ml_cfg_get_string(3)}.
 */
extern int ml_entry_point (rws_request rq, void (*app_main) (ml_session));

/* Function: ml_session_pool - get monolith per-session information
 * Function: ml_session_args
 * Function: ml_session_sessionid
 * Function: ml_session_host_header
 * Function: ml_session_canonical_path
 * Function: ml_session_script_name
 * Function: ml_session_user_agent
 * Function: ml_session_set_main_window
 * Function: ml_session_get_main_window
 * Function: ml_session_get_peername
 * Function: ml_session_get_peernamestr
 *
 * These functions extract the information from the opaque @code{ml_session}
 * object passed to most application functions and callbacks. Each separate
 * user session is tied to a separate @code{ml_session} object which
 * contains standard fields.
 *
 * @code{ml_session_pool} returns the session pool, which is a pool which
 * has the lifetime of the whole session.
 *
 * @code{ml_session_args} returns the arguments (a @code{cgi} object) which
 * were passed to the application when it started running. This is kind of
 * equivalent to @code{argc} and @code{argv} for a traditional application.
 *
 * @code{ml_session_sessionid} returns the session ID, a 32 character
 * string of random hex digits which also happens to be the cookie
 * passed by the browser.
 *
 * @code{ml_session_host_header} returns the Host: header of the
 * monolith application. For example @code{www.annexia.org}.
 *
 * @code{ml_session_canonical_path} returns the canonical path of the
 * monolith application, that is, the full path of the application as
 * it appears to the browser. For example @code{/so-bin/app.so}.
 *
 * @code{ml_session_script_name} returns just the name of the application
 * as it appears to the browser. For example @code{app.so}.
 *
 * @code{ml_session_user_agent} returns the identifying string for
 * the browser, sent in the HTTP @code{User-Agent} header. This can
 * be @code{NULL}.
 *
 * @code{ml_session_(set|get)_main_window} are a pair of esoteric
 * functions which you will not need to use in most applications. They
 * are useful, however, when you are building a website. They nominate
 * a particular window which will be the window displayed if the user
 * types the URL of the application directly into their browser
 * location bar, without the normally mandatory @code{ml_window}
 * parameter.
 *
 * @code{ml_session_get_peername} returns the peer address of the
 * socket. The peer address is the IP address of the user's browser
 * or proxy. This function returns @code{0} on success or @code{-1}
 * on failure (with the appropriate error code in @code{errno}).
 * @code{ml_session_get_peernamestr} returns the same
 * converted to a string, normally in "dotted quad" format, for example:
 * @code{"10.0.0.137"}. On failure, @code{ml_session_get_peernamestr}
 * returns @code{NULL}.
 *
 * See also: @ref{ml_entry_point(3)}, @ref{ml_die(3)}, @ref{new_cgi(3)}
 */
extern pool ml_session_pool (ml_session);
extern cgi ml_session_args (ml_session);
extern const char *ml_session_sessionid (ml_session);
extern const char *ml_session_host_header (ml_session);
extern const char *ml_session_canonical_path (ml_session);
extern const char *ml_session_script_name (ml_session);
extern const char *ml_session_user_agent (ml_session);
extern void ml_session_set_main_window (ml_session, ml_window);
extern ml_window ml_session_get_main_window (ml_session);
extern int ml_session_get_peername (ml_session, struct sockaddr *name, socklen_t *namelen);
extern const char *ml_session_get_peernamestr (ml_session);

/* Function: ml_session_release_lock
 * Function: ml_session_acquire_lock
 *
 * These are advanced functions which you will probably never need to use.
 *
 * Monolith maintains a single session structure for each session. If
 * the browser were to try and fetch two pages with the same session ID
 * at the same time, then rws could create two threads running at the
 * same time, and these two threads could access the same session
 * structure. The result of this, under some circumstances, would be
 * that the session structure would be corrupted. There is obviously
 * potential for malicious abuse here, but this can also happen under
 * normal conditions, particularly in framesets (the browser fetches
 * each frame at the same time).
 *
 * To avoid this situation, monolith maintains a mutex lock on each
 * session structure, so that no two threads can try to access the
 * same session structure at the same time. (Different sessions can
 * execute concurrently, of course).
 *
 * Monolith's session locking is normally transparent to programmers
 * and users, but these functions allow you to bypass the session
 * lock under certain circumstances. Of course, when a programmer
 * does this, they can no longer rely on monolith to keep the session
 * structure consistent, and must instead guarantee this themselves.
 *
 * @code{ml_session_release_lock} releases the session lock. This
 * potentially allows other threads to run, overwriting parts of the
 * session structure.
 *
 * @code{ml_session_acquire_lock} reacquires the session lock, possibly
 * sleeping to do so. This does not restore the session structure which
 * will still be in a semi-corrupted state.
 *
 * In future we will add functions to allow @code{ml_session_acquire_lock}
 * to restore the session structure to a non-corrupt state, perhaps by
 * having a stack of session structures (or some parts of the session
 * structure).
 *
 * See also: @ref{new_pseudothread(3)}, @ref{new_mutex(3)}.
 */
extern void ml_session_release_lock (ml_session);
extern void ml_session_acquire_lock (ml_session);

/* Database factory object. */
struct ml_dbh_factory;
typedef struct ml_dbh_factory *ml_dbh_factory;

/* Function: new_ml_dbh_factory - database handle factories, database connection functions
 * Function: ml_get_dbh
 * Function: ml_put_dbh
 *
 * Monolith supports database connection pooling through the use
 * of these functions, which are implemented on top of the
 * @code{pthrlib} database interface (dbi) library (see
 * @ref{new_db_handle(3)}).
 *
 * For each database, monolith keeps a pool of pre-connected
 * database handles, thus avoiding the overhead of opening a database
 * connection on every request. Applications and widgets can
 * ask to "borrow" a database handle for a short period of time
 * using these functions. At the end of this period, the application
 * or widget can then either voluntarily give up the database
 * handle, or else monolith recovers the handle by force at
 * various well-defined points. This process is described below.
 *
 * The monolith database functions add a new object type, called
 * a database handle factory (type @code{ml_dbh_factory}). As
 * its name would indicate, this object can be used to produce
 * database handles (they are actually fetched out of the pool
 * of free handles, or created on demand if the pool is empty).
 * These database handles must NOT be stashed away by the
 * application/widget - for reasons which are described below. Instead
 * the application should actually pass the @code{ml_dbh_factory}
 * object itself to callbacks and widgets, and they can then request
 * to borrow a "real" database handle from the factory when they actually
 * require it.
 *
 * @code{new_ml_dbh_factory} should be called in @code{app_main} to
 * create the database handle factory. It returns a
 * @code{ml_dbh_factory} object. The @code{conninfo} string argument
 * is described in @ref{new_db_handle(3)}. The database handle factory object
 * should be passed to callbacks and widgets which require access
 * to the database (database handles themselves must NEVER be passed or
 * stashed in private session storage or global variables).
 *
 * @code{ml_get_dbh} produces a database handle from the factory. This
 * handle is valid until one of the following points in time:
 * (a) the code voluntarily gives up the handle by calling
 * @code{ml_put_dbh}, or (b) the @code{app_main} function returns,
 * or (c) the current action callback function returns, or
 * (d) the widget's @code{repaint} function returns. (In other words,
 * the database handle is automatically recovered at the end of the
 * current HTTP request).
 *
 * @code{ml_put_dbh} voluntarily relinquishes the database handle.
 *
 * When the database handle is recovered - voluntarily or automatically -
 * monolith performs a rollback on the handle, and then puts the handle
 * back into its pool, ready to be taken by another session or
 * application. If you need to keep data which was modified in the
 * database transaction, then remember to do a @code{db_commit} on
 * the handle.
 *
 * Applications running in the same @code{rws} instance may share
 * database handles (provided, of course, they are both connected to
 * the same database with identical @code{conninfo} strings).
 *
 * Calling @code{ml_get_dbh} twice in a row returns two different
 * and independent database handles.
 *
 * Because of the way monolith connection pooling works, it is not
 * possible to ask monolith for a database handle which is valid
 * throughout an entire session. This is undesirable because this
 * could lead to a database handle being required for every
 * session, including sessions which are idle waiting to be timed
 * out. If your site is popular, there would be thousands of database
 * handles open, and most databases simply cannot handle this sort
 * of load, PostgreSQL included. However, the downside of this is
 * that you cannot use monolith to help you if you want to have
 * a transaction which lasts over several callbacks, or if you want
 * to make extensive use of temporary tables across callbacks. If
 * you really really want to do this, and are aware of the consequences,
 * then you should manage your own handles with @code{new_db_handle}.
 *
 * See also: @ref{new_db_handle(3)}.
 */
extern ml_dbh_factory new_ml_dbh_factory (ml_session, const char *conninfo);
extern db_handle ml_get_dbh (ml_session, ml_dbh_factory);
extern void ml_put_dbh (ml_session, db_handle);

/* Function: ml_session_login - user authentication, log in, log out
 * Function: ml_session_logout
 * Function: ml_session_userid
 *
 * These functions provide a low-level, database-independent,
 * authentication-method-agnostic way to handle the problem
 * of user authentication in monolith applications.
 *
 * All of these functions require the schema in
 * @code{sql/monolith_auth_create.sql}. This schema contains a
 * table which stores a mapping from cookies to user IDs. The
 * @code{rws} configuration file must contain a line which allows
 * monolith to find the database containing this table:
 *
 * @code{monolith user database: CONNINFO}
 *
 * (@code{CONNINFO} is the PostgreSQL connection info string).
 *
 * Each user must be identified by a unique numeric userid >= 1.
 *
 * Once a user has logged in, a cookie (@code{ml_auth}) is sent back
 * to their browser. The reason for sending back a cookie, rather than
 * just storing the userid in the session state, is that it might be
 * necessary for the user to be automatically logged in to other
 * applications on the same server. By sending the cookie back to,
 * say, the @code{/} path, a user effectively logs into all
 * applications whenever they log into any one application. Also,
 * by controlling the expiry time of the cookie, it is possible
 * to allow a user to remain logged in across several browser
 * sessions.
 *
 * The session stores the currently logged in user (as a userid)
 * or @code{0} if no user is currently logged in. @code{ml_session_userid}
 * retrieves the currently logged in userid or @code{0}. Note that
 * the currently logged in user can change unexpectedly, so you
 * must NOT stash the userid away. Call @code{ml_session_userid}
 * every time you want to check the currently logged in user. See
 * below for more details.
 *
 * When a user logs in to a particular application, the application
 * should call @code{ml_session_login}. The actual method of login
 * is not defined at this level (but monolith provides some higher-level
 * widgets which you may use). The @code{userid} parameter is the
 * user who has logged in. The @code{path} and @code{expires} parameters
 * are used to control the @code{ml_auth} cookie sent back to the
 * browser. If @code{path} is @code{NULL}, then the cookie only applies
 * to the current application. If @code{path} is @code{"/"}, then the
 * user will be automatically logged into all applications running
 * at the same address. Other @code{path}s may also be specified to
 * restrict authentication to some subset of the path space. If
 * @code{expires} is @code{NULL}, then the cookie will expire at
 * the end of the current browser session (when the user closes their
 * browser). This means that the next time they visit the site, they
 * will need to log in again, if they have closed their browser in
 * the meantime. @code{expires} may also be set to a date in
 * RFC 2616 compliant format, or to a relative time such as
 * @code{"+1y"} (meaning 1 year from now). Relative times are
 * specified in exactly the same way as the @code{rws} @code{expires}
 * configuration option.
 *
 * There are various browser-related problems which mean that you
 * should not mix and match different @code{path}s unless you really
 * know what you are doing. In general, you should always set @code{path}
 * to either @code{"/"} everywhere, or to @code{NULL} everywhere. The
 * simplest thing is to always set @code{path} to @code{"/"} everywhere.
 *
 * If @code{path} is not @code{NULL}, then calling @code{ml_session_login}
 * may also cause other monolith applications to become logged in.
 * This is because they notice the @code{ml_auth} cookie, verify it
 * against the database, and if it is verified, change their state
 * to logged in. For this reason, applications should not stash away
 * the result of @code{ml_session_userid}, but should check it each
 * time they need it, because it can change unexpectedly.
 *
 * @code{ml_session_logout} logs the current user out. @code{path} should
 * be the same as above.
 *
 * As before, if @code{path} is not @code{NULL}, then calling
 * @code{ml_session_logout} may cause other monolith applications to
 * become logged out (@code{ml_session_userid} will start to return
 * @code{0}). This is because monolith overwrites the @code{ml_auth}
 * cookie with a "poisoned" cookie which other applications may notice.
 */
extern void ml_session_login (ml_session, int userid, const char *path, const char *expires);
extern void ml_session_logout (ml_session, const char *path);
extern int ml_session_userid (ml_session);

/* Function: ml_cfg_get_string - get values from the configuration file
 * Function: ml_cfg_get_int
 * Function: ml_cfg_get_bool
 *
 * @code{rws_request_cfg_get_string} returns the configuration file
 * string for @code{key}. If there is no entry in the configuration
 * file, this returns @code{default_value}.
 *
 * @code{rws_request_cfg_get_int} returns the string converted to
 * an integer.
 *
 * @code{rws_request_cfg_get_bool} returns the string converted to
 * a boolean.
 *
 * See also: @ref{ml_entry_point(3)}, @ref{rws_request_cfg_get_string(3)}.
 *
 */
extern const char *ml_cfg_get_string (ml_session, const char *key, const char *default_value);
extern int ml_cfg_get_int (ml_session, const char *key, int default_value);
extern int ml_cfg_get_bool (ml_session, const char *key, int default_value);

/* Function: ml_register_action - register callbacks
 * Function: ml_unregister_action
 *
 * Widgets such as buttons may register actions (callback functions)
 * to be run when a user clicks a button or submits a form. Each
 * action is associated with a unique action ID (a string). A separate
 * set of actions is registered within each session. The callback
 * function is invoked as @code{void callback_fn (ml_session
 * session, void *data)}.
 *
 * @code{ml_register_action} registers an action within a given
 * session and returns the action ID string.
 *
 * @code{ml_unregister_action} unregisters an action.
 *
 * See also: @ref{new_ml_button(3)}.
 */
extern const char *ml_register_action (ml_session session, void (*callback_fn) (ml_session, void *), void *data);
extern void ml_unregister_action (ml_session session, const char *action_id);

/* Private function used by forms to get the arguments passed just to
 * this request. The returned value is on the short-lived thread pool,
 * so interesting stuff must be copied to the session pool.
 */
extern cgi _ml_session_submitted_args (ml_session);

/* Private function used by ml_window to register the current window. */
extern void _ml_session_set_current_window (ml_session, ml_window, const char *windowid);

/* Some private functions used by the stats package to inspect the internals
 * of monolith. These functions are subject to change and should not be used
 * in ordinary applications.
 */
extern const vector _ml_get_sessions (pool);
extern ml_session _ml_get_session (const char *sessionid);
extern int _ml_session_get_hits (ml_session);
extern reactor_time_t _ml_session_get_last_access (ml_session);
extern reactor_time_t _ml_session_get_created (ml_session);
extern struct sockaddr_in _ml_session_get_original_ip (ml_session);
extern void *_ml_session_get_app_main (ml_session);
extern const vector _ml_session_get_windows (ml_session, pool);
extern ml_window _ml_session_get_window (ml_session, const char *windowid);
extern const vector _ml_session_get_actions (ml_session, pool);
extern int _ml_session_get_action (ml_session, const char *actionid, void **fn_rtn, void **data_rtn);
extern const vector _ml_session_get_dbhs (ml_session, pool);
extern ml_dbh_factory _ml_session_get_auth_dbf (ml_session);
extern const vector _ml_get_dbh_factories (pool);
extern ml_dbh_factory _ml_get_dbh_factory (const char *conninfo);
extern int _ml_dbh_factory_get_nr_allocated_handles (ml_dbh_factory);
extern int _ml_dbh_factory_get_nr_free_handles (ml_dbh_factory);

#endif /* MONOLITH_H */
