/* Monolith server-parsed pages (.msp's).
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
 * $Id: ml_msp.c,v 1.9 2003/01/31 18:03:34 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <assert.h>

#include <pcre.h>

#include <pool.h>
#include <pstring.h>
#include <pre.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_widget.h"
#include "ml_flow_layout.h"
#include "ml_label.h"
#include "ml_msp.h"

static void repaint (void *, ml_session, const char *, io_handle);

struct ml_widget_operations msp_ops =
  {
    repaint: repaint
  };

struct ml_msp
{
  struct ml_widget_operations *ops;
  pool pool;			/* Pool for allocations. */
  ml_session session;		/* Current session. */
  ml_dbh_factory dbf;		/* Database factory. */
  const char *rootdir;		/* Document root. */
  const char *filename;		/* MSP file, relative to rootdir. */
  const char *pathname;		/* Full path to file. */
  vector widgetpath;		/* Path when loading widgets (may be NULL). */
  ml_flow_layout w;		/* MSP is really just a flow layout. */
};

static void init_msp (void) __attribute__((constructor));
static void free_msp (void) __attribute__((destructor));
static int  parse_file (ml_msp w, int fd);
static int  verify_relative_path (const char *s);
static int  verify_filename (const char *s);

/* Global variables. */
static pool msp_pool;
static const pcre *re_openclose, *re_ws;

/* Initialise the library. */
static void
init_msp ()
{
  msp_pool = new_subpool (global_pool);
  re_openclose = precomp (msp_pool, "<%|%>", 0);
  re_ws = precomp (msp_pool, "[ \t]+", 0);
}

/* Free up global memory used by the library. */
static void
free_msp ()
{
  delete_pool (msp_pool);
}

ml_msp
new_ml_msp (pool pool, ml_session session, ml_dbh_factory dbf,
	    const char *rootdir, const char *filename)
{
  ml_msp w = pmalloc (pool, sizeof *w);
  int fd;

  /* Security checks on the rootdir and filename. */
  if (rootdir[0] != '/')
    pth_die ("ml_msp.c: rootdir must start with '/'\n");

  if (!verify_relative_path (filename))
    return 0;

  w->ops = &msp_ops;
  w->pool = pool;
  w->session = session;
  w->dbf = dbf;
  w->rootdir = rootdir;
  w->filename = filename;
  w->widgetpath = 0;
  w->w = new_ml_flow_layout (pool);

  /* Create the full path to the file. */
  w->pathname = psprintf (pool, "%s/%s", rootdir, filename);

  /* Open it. */
  fd = open (w->pathname, O_RDONLY);
  if (fd == -1)
    {
      perror (psprintf (pool, "ml_msp.c: %s", w->pathname));
      return 0;
    }

  /* Read and parse the file. */
  if (parse_file (w, fd) == -1)
    {
      close (fd);
      return 0;			/* parse_file prints an error. */
    }

  close (fd);

  return w;
}

static int parse_directive (ml_msp w, const char *directive);
static const char *load_file (pool tmp, int fd);

static int
parse_file (ml_msp w, int fd)
{
  pool pool = w->pool, tmp = new_subpool (pool);
  ml_label label;
  vector v;
  const char *file;
  int i;
  char state = 'o';

  /* Load the file into a temporary buffer. */
  file = load_file (tmp, fd);
  if (!file) return -1;

  /* Using pstrresplit2 we can split up the file into units like
   * this: [ "some HTML", "<%", "directive", "%>", "some more HTML", ... ]
   * This makes parsing the file much simpler.
   */
  v = pstrresplit2 (pool, file, re_openclose);

  for (i = 0; i < vector_size (v); ++i)
    {
      const char *s;
      char type;

      vector_get (v, i, s);

#if 0				/* Debug. */
      fprintf (stderr, "ml_msp.c: reading %s\n", pstrndup (tmp, s, 20));
#endif

      if (strcmp (s, "<%") == 0) type = '(';
      else if (strcmp (s, "%>") == 0) type = ')';
      else type = '-';

      switch (state)
	{
	case 'o':		/* Waiting for opening <% */
	  if (type == '(')
	    /* Found it. We are now inside a directive. */
	    state = 'i';
	  else
	    {
	      /* Must be HTML. Output it as a label. */
	      label = new_ml_label (pool, s);
	      ml_flow_layout_push_back (w->w, label);
	    }
	  break;
	case 'i':		/* Inside a directive. */
	  if (type == '-')
	    {
	      /* Parse the directive. */
	      if (parse_directive (w, s) == -1)
		return -1;
	      state = 'c';
	    }
	  else if (type == ')')	/* Allow <%%> - just ignore it. */
	    state = 'o';
	  else if (type != ')')
	    {
	      /* It's an error. */
	      fprintf (stderr,
		       "ml_msp.c: %s: unexpected '%s' inside directive.\n",
		       w->filename, s);
	      return -1;
	    }
	  break;
	case 'c':		/* Expecting %>. */
	  if (type == ')')
	    /* Found it. We are now back in HTML. */
	    state = 'o';
	  else
	    {
	      fprintf (stderr,
		       "ml_msp.c: %s: unexpected '%s' inside directive.\n",
		       w->filename, s);
	      return -1;
	    }
	} /* switch (state) */
    } /* for */

  /* Check our final state, which must be 'o'. */
  if (state != 'o')
    {
      fprintf (stderr, "ml_msp.c: %s: unclosed '<%%' in file.\n", w->filename);
      return -1;
    }

  delete_pool (tmp);
  return 0;
}

static int
do_include (ml_msp w, const char *include_file)
{
  pool pool = w->pool;
  char *dir, *t, *try;
  int fd;

  /* Verify that this is a plain, ordinary filename. */
  if (!verify_filename (include_file))
    return -1;

  /* Locate the included file, relative to the current filename. Never leave
   * the current root, however.
   */
  dir = pstrdup (pool, w->filename);
  while (strlen (dir) > 0)
    {
      t = strrchr (dir, '/');
      if (t)
	{
	  *t = '\0';
	  try = psprintf (pool, "%s/%s/%s", w->rootdir, dir, include_file);
	}
      else
	{
	  *dir = '\0';
	  try = psprintf (pool, "%s/%s", w->rootdir, include_file);
	}

      fd = open (try, O_RDONLY);
      if (fd >= 0)
	goto found_it;
    }

  /* Not found. */
  fprintf (stderr, "ml_msp.c: include: %s: file not found.\n", include_file);
  return -1;

 found_it:
  /* Parse the included file. */
  if (parse_file (w, fd) == -1)
    {
      close (fd);
      return -1;		/* parse_file prints an error. */
    }

  close (fd);

  return 0;
}

static int
do_widget (ml_msp w, const char *libfile, const char *new_fn,
	   const vector args)
{
  void *lib, *new_sym;
  const char *filename, *error, *arg[5];
  ml_widget widget;
  int i;

  if (strcmp (libfile, "-") == 0)
    filename = 0;		/* Search in the current executable. */
  else if (libfile[0] != '/')	/* Relative to the widget path. */
    {
      filename = libfile;

      if (w->widgetpath)
	for (i = 0; i < vector_size (w->widgetpath); ++i)
	  {
	    const char *path;
	    const char *try;

	    vector_get (w->widgetpath, i, path);
	    try = psprintf (w->pool, "%s/%s", path, libfile);
	    if (access (try, X_OK) == 0)
	      {
		filename = try;
		break;
	      }
	  }
    }
  else				/* Absolute path. */
    filename = libfile;

  lib = dlopen (filename,
#ifndef __OpenBSD__
		RTLD_NOW
#else
		O_RDWR
#endif
		);
  if (lib == 0)
    {
      fprintf (stderr, "ml_msp.c: %s: %s\n", libfile, dlerror ());
      return -1;
    }

  /* Does the new function exist? */
  new_sym = dlsym (lib, new_fn);
  if ((error = dlerror ()) != 0)
    {
      fprintf (stderr, "ml_msp.c: %s: %s: %s\n", libfile, new_fn, error);
      dlclose (new_sym);
      return -1;
    }

  /* Make sure we close this library when the widget is deleted. */
  pool_register_cleanup_fn (w->pool, (void (*)(void *)) dlclose, lib);

  /* Formulate our call.
   * XXX There needs to be a generic method for doing this in c2lib XXX
   */
  arg[0] = arg[1] = arg[2] = arg[3] = arg[4] = 0;
  if (vector_size (args) >= 1) vector_get (args, 0, arg[0]);
  if (vector_size (args) >= 2) vector_get (args, 1, arg[1]);
  if (vector_size (args) >= 3) vector_get (args, 2, arg[2]);
  if (vector_size (args) >= 4) vector_get (args, 3, arg[3]);
  if (vector_size (args) >= 5) vector_get (args, 4, arg[4]);

  if (vector_size (args) == 1 && strcmp (arg[0], "pool") == 0)
    {
      ml_widget (*fn) (pool) = (ml_widget (*) (pool)) new_sym;

      widget = fn (w->pool);
      if (widget) ml_flow_layout_push_back (w->w, widget);
    }
  else if (vector_size (args) == 2 && strcmp (arg[0], "pool") == 0 &&
	   strcmp (arg[1], "session") == 0)
    {
      ml_widget (*fn) (pool, ml_session) =
	(ml_widget (*) (pool, ml_session)) new_sym;

      widget = fn (w->pool, w->session);
      if (widget) ml_flow_layout_push_back (w->w, widget);
    }
  else if (vector_size (args) == 3 && strcmp (arg[0], "pool") == 0 &&
	   strcmp (arg[1], "session") == 0 &&
	   strcmp (arg[2], "dbf") == 0)
    {
      ml_widget (*fn) (pool, ml_session, ml_dbh_factory) =
	(ml_widget (*) (pool, ml_session, ml_dbh_factory)) new_sym;

      widget = fn (w->pool, w->session, w->dbf);
      if (widget) ml_flow_layout_push_back (w->w, widget);
    }
  else if (vector_size (args) == 4 && strcmp (arg[0], "pool") == 0 &&
	   strcmp (arg[1], "session") == 0 &&
	   strcmp (arg[2], "dbf") == 0)
    {
      ml_widget (*fn) (pool, ml_session, ml_dbh_factory, const char *) =
      (ml_widget (*) (pool, ml_session, ml_dbh_factory, const char *)) new_sym;

      widget = fn (w->pool, w->session, w->dbf, arg[3]);
      if (widget) ml_flow_layout_push_back (w->w, widget);
    }
  else if (vector_size (args) == 5 && strcmp (arg[0], "pool") == 0 &&
	   strcmp (arg[1], "session") == 0 &&
	   strcmp (arg[2], "dbf") == 0)
    {
      ml_widget (*fn) (pool, ml_session, ml_dbh_factory, const char *, const char *) =
      (ml_widget (*) (pool, ml_session, ml_dbh_factory, const char *, const char *)) new_sym;

      widget = fn (w->pool, w->session, w->dbf, arg[3], arg[4]);
      if (widget) ml_flow_layout_push_back (w->w, widget);
    }
  else
    abort ();			/* XXX Not yet implemented */

  return 0;
}

static int
do_widgetpath (ml_msp w, const vector dirs)
{
  fprintf (stderr, "XXX not impl XXX\n");
  return -1;
}

static int
parse_directive (ml_msp w, const char *directive)
{
  vector tokens;
  pool pool = w->pool;
  const char *command;

  /* Split the directive up into tokens.
   * XXX This is presently not very intelligent. It will split
   * string arguments. There is a proposed solution for this in
   * the form of a 'pstrtok' function in c2lib. At the moment this
   * will suffice.
   */
  tokens = pstrresplit (pool, directive, re_ws);
  if (vector_size (tokens) < 1)
    return 0;			/* Just ignore it. */

  /* Get the command. */
  vector_pop_front (tokens, command);

  if (strcasecmp (command, "include") == 0)
    {
      const char *file;

      if (vector_size (tokens) != 1)
	{
	  fprintf (stderr, "ml_msp.c: %s: include: needs one filename\n",
		   w->filename);
	  return -1;
	}
      vector_pop_front (tokens, file);
      return do_include (w, file);
    }
  else if (strcasecmp (command, "widget") == 0)
    {
      const char *file;
      const char *new_fn;

      if (vector_size (tokens) < 3)
	{
	  fprintf (stderr, "ml_msp.c: %s: widget: bad parameters\n",
		   w->filename);
	  return -1;
	}
      vector_pop_front (tokens, file);
      vector_pop_front (tokens, new_fn);
      return do_widget (w, file, new_fn, tokens);
    }
  else if (strcasecmp (command, "widgetpath") == 0)
    {
      return do_widgetpath (w, tokens);
    }
  else
    {
      fprintf (stderr, "ml_msp.c: %s: %s: unknown directive\n",
	       w->filename, command);
      return -1;
    }
}

/* Load a file into memory from fd, allocating space from the
 * temporary pool tmp.
 */
static const char *
load_file (pool tmp, int fd)
{
  char *file = 0;
  int r, sz = 0;
  const int n = 16385;		/* [sic] - see comment at end */

  for (;;)
    {
      file = prealloc (tmp, file, sz + n);
      r = read (fd, file + sz, n-1);

      if (r < 0)
	{
	  perror ("read");
	  return 0;
	}
      if (r == 0) break;	/* end of file */

      sz += r;
    }

  /* Since tmp is a temporary pool, don't bother rounding the size of the
   * buffer down to match the file size. Just make sure it's null-terminated.
   * Note that one extra byte we reserved above.
   */
  file[sz] = '\0';

  return file;
}

static int
verify_relative_path (const char *s)
{
  if (s[0] == '/' ||
      strncmp (s, "..", 2) == 0 ||
      strstr (s, "/..") ||
      strlen (s) == 0)
    {
      fprintf (stderr,
	       "ml_msp.c: security error: string is not a relative path.\n");
      return 0;
    }
  return 1;
}

static int
verify_filename (const char *s)
{
  if (strchr (s, '/') != 0 ||
      s[0] == '.' ||
      strlen (s) == 0)
    {
      fprintf (stderr,
	       "ml_msp.c: security error: string is not a plain filename.\n");
      return 0;
    }
  return 1;
}

static void
repaint (void *vw, ml_session session, const char *windowid, io_handle io)
{
  ml_msp w = (ml_msp) vw;

  /* Repaint the flow layout. */
  if (w->w)
    ml_widget_repaint (w->w, session, windowid, io);
}
