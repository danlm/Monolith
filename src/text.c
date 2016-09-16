/* Monolith text processing functions.
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
 * $Id: text.c,v 1.1 2002/12/08 17:29:17 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <pthr_iolib.h>

#include "monolith.h"
#include "ml_smarttext.h"

void
ml_plaintext_print (io_handle io, const char *text)
{
  while (*text)
    {
      switch (*text)
	{
	default: io_fputc (*text, io); break;
	case '<': io_fputs ("&lt;", io); break;
	case '>': io_fputs ("&gt;", io); break;
	case '\n': io_fputs ("<br>", io); break;
	case '&': io_fputs ("&amp;", io); break;
	case '"': io_fputs ("&quot;", io); break;
	}
      text++;
    }
}

const char *
ml_plaintext_to_html (pool pool, const char *text)
{
  char *new_text;
  int len = strlen (text);
  int new_len = 0;
  int i, j;

  /* Work out how long the escaped string will be. Escaped strings get
   * bigger.
   */
  for (i = 0; i < len; ++i)
    if (text[i] == '<' || text[i] == '>' || text[i] == '\n')
      new_len += 4;
    else if (text[i] == '&')
      new_len += 5;
    else if (text[i] == '"')
      new_len += 6;
    else
      new_len++;

  new_text = pmalloc (pool, sizeof (char) * (new_len+1));

  /* Now copy and escape the string. */
  for (i = j = 0; i < len; ++i)
    switch (text[i])
      {
      default:
	new_text[j++] = text[i];
	break;
      case '<':
	new_text[j++] = '&';
	new_text[j++] = 'l';
	new_text[j++] = 't';
	new_text[j++] = ';';
	break;
      case '>':
	new_text[j++] = '&';
	new_text[j++] = 'g';
	new_text[j++] = 't';
	new_text[j++] = ';';
	break;
      case '\n':
	new_text[j++] = '<';
	new_text[j++] = 'b';
	new_text[j++] = 'r';
	new_text[j++] = '>';
	break;
      case '&':
	new_text[j++] = '&';
	new_text[j++] = 'a';
	new_text[j++] = 'm';
	new_text[j++] = 'p';
	new_text[j++] = ';';
	break;
      case '"':
	new_text[j++] = '&';
	new_text[j++] = 'q';
	new_text[j++] = 'u';
	new_text[j++] = 'o';
	new_text[j++] = 't';
	new_text[j++] = ';';
	break;
      }

  new_text[j++] = '\0';

  return new_text;
}

void
ml_smarttext_print (io_handle io, const char *text)
{
  pool tmp = new_subpool (global_pool);
  const char *s;

  s = ml_smarttext_to_html (tmp, text);
  io_fputs (s, io);
  delete_pool (tmp);
}

void
ml_filterhtml_print (io_handle io, const char *text)
{
  /* XXX NOT IMPLEMENTED XXX */
  io_fputs (text, io);
}

const char *
ml_filterhtml_to_html (pool pool, const char *text)
{
  /* XXX NOT IMPLEMENTED XXX */
  return text;
}

void
ml_anytext_print (io_handle io, const char *text, char type)
{
  switch (type)
    {
    case 'p':
      ml_plaintext_print (io, text);
      break;
    case 's':
      ml_smarttext_print (io, text);
      break;
    case 'h':
      ml_filterhtml_print (io, text);
      break;
    }
}

const char *
ml_anytext_to_html (pool pool, const char *text, char type)
{
  switch (type)
    {
    case 'p':
      text = ml_plaintext_to_html (pool, text);
      break;
    case 's':
      text = ml_smarttext_to_html (pool, text);
      break;
    case 'h':
      text = ml_filterhtml_to_html (pool, text);
      break;
    }

  return text;
}
