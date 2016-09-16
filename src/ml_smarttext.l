/* Monolith smart text. -*- C -*-
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
 * $Id: ml_smarttext.l,v 1.1 2002/12/08 17:29:17 rich Exp $
 */

%{
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>

#include "monolith.h"
#include "ml_smarttext.h"

/* This removes a warning message. */
#define YY_NO_UNPUT 1

static void emit_str (const char *s);
static void emit_strn (const char *str, const char *end);
static void emit_char (char c);
%}

%option noyywrap

	/* Characters in URLs. */
URLCHAR [^ \t\)<>]

	/* Characters in bold/italic/monospace text. */
ORDCHARS [[:alnum:] \t,\']

%%

	/* URLs. */
[[:lower:]]+"://"{URLCHAR}+ {
  emit_str ("<a href=\"");
  emit_str (yytext);
  emit_str ("\" target=\"_new\">");
  emit_str (yytext);
  emit_str ("</a>");
}

	/* Special mailto: URLs. */
	/* XXX Email addresses. */
mailto:{URLCHAR}+ {
  emit_str ("<a href=\"");
  emit_str (yytext);
  emit_str ("\" target=\"_new\"><img src=\"/ml-icons/envelope.gif\">&nbsp;");
  emit_str (yytext+7);
  emit_str ("</a>");
}

	/* Fractions - must appear before italics. */
"1/4"	emit_str ("&frac14;");
"1/2"	emit_str ("&frac12;");
"3/4"	emit_str ("&frac34;");

	/* Special characters. */
"(C)"	emit_str ("&copy;");
"(R)"	emit_str ("&reg;");
"(TM)"	emit_str ("<sup>TM</sup>");

	/* Bold, italic, monospace text. */
"*"{ORDCHARS}+"*" {
  emit_str ("<b>");
  emit_strn (yytext+1, &yytext[strlen (yytext)-1]);
  emit_str ("</b>");
}
"/"{ORDCHARS}+"/" {
  emit_str ("<i>");
  emit_strn (yytext+1, &yytext[strlen (yytext)-1]);
  emit_str ("</i>");
}
"="{ORDCHARS}+"=" {
  emit_str ("<code>");
  emit_strn (yytext+1, &yytext[strlen (yytext)-1]);
  emit_str ("</code>");
}

	/* Smileys. */
[:;][-oO][\)>] {
  emit_str ("<img src=\"/ml-icons/smiley.gif\">");
}
[:;][-oO][\(<] {
  emit_str ("<img src=\"/ml-icons/sad.gif\">");
}
[:;][-oO][pP] {
  emit_str ("<img src=\"/ml-icons/tongue.gif\">");
}

	/* HTML entities which need to be escaped. */
"&"	emit_str ("&amp;");
"<"	emit_str ("&lt;");
">"	emit_str ("&gt;");
\"	emit_str ("&quot;");
\n	emit_str ("<br>");

	/* Anything else just gets emitted as a normal character. */
.	emit_char (yytext[0]);

%%

/* These global variables control where the output gets sent.
 * Note that because none of this code blocks, we don't need to
 * worry about these globals getting corrupted.
 */
static pool out_pool;		/* Pool for allocations. */
static char *out_str;		/* Output string. */
static int used, allocated;	/* Space used and allocated in the string. */

const char *
ml_smarttext_to_html (pool pool, const char *text)
{
  YY_BUFFER_STATE buf;

  /* Set up the global variables so that output goes to our string. */
  out_pool = pool;
  out_str = 0;
  used = allocated = 0;

  /* We're going to scan from this buffer (instead of stdin). */
  buf = yy_scan_string (text);

  /* Run the scanner. */
  ml_smarttext_lex ();

  /* Clean up. */
  yy_delete_buffer (buf);

  /* Tack an ASCII NUL on the end of the string to terminate it. */
  emit_char ('\0');

  return out_str;
}

static void
emit_str (const char *s)
{
  while (*s) emit_char (*s++);
}

static void
emit_strn (const char *str, const char *end)
{
  while (str < end) emit_char (*str++);
}

static void
emit_char (char c)
{
  if (used >= allocated)
    {
      allocated += 32;
      out_str = prealloc (out_pool, out_str, allocated);
    }

  out_str[used++] = c;
}
