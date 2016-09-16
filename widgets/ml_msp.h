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
 * $Id: ml_msp.h,v 1.3 2002/10/15 21:28:34 rich Exp $
 */

#ifndef ML_MSP_H
#define ML_MSP_H

#include "monolith.h"

struct ml_msp;
typedef struct ml_msp *ml_msp;

/* Function: new_ml_msp - monolith server-parsed pages (.msp)
 *
 * Monolith server-parsed pages (@code{.msp} files) are ordinary
 * HTML files which contain additional markup for embedding other
 * monolith widgets and a few other useful features like server-side
 * includes.
 *
 * A @code{msp} widget is a layout widget which can, like any other
 * widget, be embedded anywhere. However, normally you will not be
 * using the @code{msp} widget directly, but instead will use the
 * @code{msp.so} application (in the @code{apps/} directory). You
 * use the @code{msp.so} application by placing the following lines
 * in the rws host configuration file:
 *
 * @code{begin rewrite}
 *
 * @code{ ...}
 *
 * @code{^/(.*\\.msp)$  /so-bin/msp.so?page=$1  last}
 *
 * @code{end rewrite}
 *
 * @code{msp root: /var/www/html}
 *
 * When the user requests @code{/dir/file.msp}, this is internally
 * rewritten to a request for @code{/so-bin/msp.so?page=/dir/file.msp}.
 * The @code{msp.so} program creates the @code{msp} widget with the
 * arguments @code{rootdir = /var/www/html} and
 * @code{filename = dir/file.msp}.
 *
 * The @code{msp} widget fetches @code{/var/www/html/dir/file.msp},
 * interprets any special markup, and displays the page.
 *
 * The following special markup can appear in @code{msp} files:
 *
 * @code{<% widget [libfile.so|-] new_fn [param1 [param2 [...]]] %>}: Place a
 * monolith widget here. The widget is loaded from file
 * @code{libfile.so} which can either be a name (relative to
 * the current widget path) or a pathname. To load a widget
 * from core or the msp library itself, there is no need
 * to specify the library file. Just use the special name
 * @code{-}. Function @code{new_fn} is called to create the widget
 * with the list of parameters given. Each parameter can either be
 * a string (in "double quotes"), an integer, or one of the
 * special identifiers @code{pool} or @code{session}.
 *
 * @code{<% widgetpath dir1 [dir2 [dir3 [...]]] %>}: Adds the directories
 * listed to the current path which is searched for widget libraries. The
 * standard library directories (eg. @code{/usr/lib}, @code{/lib} and
 * others depending on the platform) are automatically included.
 *
 * @code{<% include file %>} includes a file. The file is searched
 * for in the document root, starting at the same directory as the
 * @code{.msp} file, and going up one directory at a time until we
 * reach the document root. For security reasons, the filename
 * cannot contain @code{/} or begin with a dot. The contents of the
 * file can contain @code{msp} directives.
 *
 * @code{new_ml_msp} creates a new @code{msp} widget. The @code{rootdir}
 * parameter is the document root. @code{msp} promises never to attempt
 * to read or include a file outside the document root, although other
 * types of file such as widget libraries can come from outside the
 * document root. The @code{filename} parameter is the name of the
 * @code{msp} file, taken relative to @code{rootdir}. For security
 * reasons @code{filename} cannot contain any @code{..} elements, else
 * @code{new_ml_msp} returns @code{NULL}.
 *
 * An @code{msp} widget is really just a specialised form of flow
 * layout (see @ref{new_ml_flow_layout(3)}).
 */
extern ml_msp new_ml_msp (pool pool, ml_session session, ml_dbh_factory dbf, const char *rootdir, const char *filename);

#endif /* ML_MSP_H */
