/* Line breaking of strings.
   Copyright (C) 2001-2003, 2006-2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unilbrk.h"

#include <stdlib.h>
#include <string.h>
#if HAVE_ICONV
# include <iconv.h>
#endif

#include "c-ctype.h"
#include "streq.h"
#include "xsize.h"
#include "unilbrk/ulc-common.h"

/* Line breaking of a string in an arbitrary encoding.

   We convert the input string to Unicode.

   The standardized Unicode encodings are UTF-8, UCS-2, UCS-4, UTF-16,
   UTF-16BE, UTF-16LE, UTF-7.  UCS-2 supports only characters up to
   \U0000FFFF.  UTF-16 and variants support only characters up to
   \U0010FFFF.  UTF-7 is way too complex and not supported by glibc-2.1.
   UCS-4 specification leaves doubts about endianness and byte order mark.
   glibc currently interprets it as big endian without byte order mark,
   but this is not backed by an RFC.  So we use UTF-8. It supports
   characters up to \U7FFFFFFF and is unambiguously defined.  */

int
ulc_width_linebreaks (const char *s, size_t n,
		      int width, int start_column, int at_end_columns,
		      const char *o, const char *encoding,
		      char *p)
{
  if (n == 0)
    return start_column;
  if (is_utf8_encoding (encoding))
    return u8_width_linebreaks ((const uint8_t *) s, n, width, start_column, at_end_columns, o, encoding, p);
  else
    {
#if HAVE_ICONV
      iconv_t to_utf8;
      /* Avoid glibc-2.1 bug with EUC-KR.  */
# if (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) && !defined _LIBICONV_VERSION
      if (STREQ (encoding, "EUC-KR", 'E', 'U', 'C', '-', 'K', 'R', 0, 0, 0))
	to_utf8 = (iconv_t)(-1);
      else
# endif
      /* Avoid Solaris 9 bug with GB2312, EUC-TW, BIG5, BIG5-HKSCS, GBK,
	 GB18030.  */
# if defined __sun && !defined _LIBICONV_VERSION
      if (   STREQ (encoding, "GB2312", 'G', 'B', '2', '3', '1', '2', 0, 0, 0)
	  || STREQ (encoding, "EUC-TW", 'E', 'U', 'C', '-', 'T', 'W', 0, 0, 0)
	  || STREQ (encoding, "BIG5", 'B', 'I', 'G', '5', 0, 0, 0, 0, 0)
	  || STREQ (encoding, "BIG5-HKSCS", 'B', 'I', 'G', '5', '-', 'H', 'K', 'S', 'C')
	  || STREQ (encoding, "GBK", 'G', 'B', 'K', 0, 0, 0, 0, 0, 0)
	  || STREQ (encoding, "GB18030", 'G', 'B', '1', '8', '0', '3', '0', 0, 0))
	to_utf8 = (iconv_t)(-1);
      else
# endif
      to_utf8 = iconv_open (UTF8_NAME, encoding);
      if (to_utf8 != (iconv_t)(-1))
	{
	  /* Determine the length of the resulting UTF-8 string.  */
	  size_t m = iconv_string_length (to_utf8, s, n);
	  if (m != (size_t)(-1))
	    {
	      /* Convert the string to UTF-8 and build a translation table
		 from offsets into s to offsets into the translated string.  */
	      size_t memory_size =
		xsum4 (xtimes (n, sizeof (size_t)), m, m,
		       (o != NULL ? m : 0));
	      char *memory =
		(char *)
		(size_in_bounds_p (memory_size) ? malloc (memory_size) : NULL);
	      if (memory != NULL)
		{
		  size_t *offtable = (size_t *) memory;
		  char *t = (char *) (offtable + n);
		  char *q = (char *) (t + m);
		  char *o8 = (o != NULL ? (char *) (q + m) : NULL);
		  int res_column;
		  size_t i;

		  iconv_string_keeping_offsets (to_utf8, s, n, offtable, t, m);

		  /* Translate the overrides to the UTF-8 string.  */
		  if (o != NULL)
		    {
		      memset (o8, UC_BREAK_UNDEFINED, m);
		      for (i = 0; i < n; i++)
			if (offtable[i] != (size_t)(-1))
			  o8[offtable[i]] = o[i];
		    }

		  /* Determine the line breaks of the UTF-8 string.  */
		  res_column =
		    u8_width_linebreaks ((const uint8_t *) t, m, width, start_column, at_end_columns, o8, encoding, q);

		  /* Translate the result back to the original string.  */
		  memset (p, UC_BREAK_PROHIBITED, n);
		  for (i = 0; i < n; i++)
		    if (offtable[i] != (size_t)(-1))
		      p[i] = q[offtable[i]];

		  free (memory);
		  iconv_close (to_utf8);
		  return res_column;
		}
	    }
	  iconv_close (to_utf8);
	}
#endif
      /* Impossible to convert.  */
#if C_CTYPE_ASCII
      if (is_all_ascii (s, n))
	{
	  /* ASCII is a subset of UTF-8.  */
	  return u8_width_linebreaks ((const uint8_t *) s, n, width, start_column, at_end_columns, o, encoding, p);
	}
#endif
      /* We have a non-ASCII string and cannot convert it.
	 Don't produce line breaks except those already present in the
	 input string.  All we assume here is that the encoding is
	 minimally ASCII compatible.  */
      {
	const char *s_end = s + n;
	while (s < s_end)
	  {
	    *p = ((o != NULL && *o == UC_BREAK_MANDATORY) || *s == '\n'
		  ? UC_BREAK_MANDATORY
		  : UC_BREAK_PROHIBITED);
	    s++;
	    p++;
	    if (o != NULL)
	      o++;
	  }
	/* We cannot compute widths in this case.  */
	return start_column;
      }
    }
}


#ifdef TEST

#include <stdio.h>
#include <locale.h>

/* Read the contents of an input stream, and return it, terminated with a NUL
   byte. */
char *
read_file (FILE *stream)
{
#define BUFSIZE 4096
  char *buf = NULL;
  int alloc = 0;
  int size = 0;
  int count;

  while (! feof (stream))
    {
      if (size + BUFSIZE > alloc)
	{
	  alloc = alloc + alloc / 2;
	  if (alloc < size + BUFSIZE)
	    alloc = size + BUFSIZE;
	  buf = realloc (buf, alloc);
	  if (buf == NULL)
	    {
	      fprintf (stderr, "out of memory\n");
	      exit (1);
	    }
	}
      count = fread (buf + size, 1, BUFSIZE, stream);
      if (count == 0)
	{
	  if (ferror (stream))
	    {
	      perror ("fread");
	      exit (1);
	    }
	}
      else
	size += count;
    }
  buf = realloc (buf, size + 1);
  if (buf == NULL)
    {
      fprintf (stderr, "out of memory\n");
      exit (1);
    }
  buf[size] = '\0';
  return buf;
#undef BUFSIZE
}

int
main (int argc, char * argv[])
{
  setlocale (LC_CTYPE, "");
  if (argc == 2)
    {
      /* Insert line breaks for a given width.  */
      int width = atoi (argv[1]);
      char *input = read_file (stdin);
      int length = strlen (input);
      char *breaks = malloc (length);
      int i;

      ulc_width_linebreaks (input, length, width, 0, 0, NULL, locale_charset (), breaks);

      for (i = 0; i < length; i++)
	{
	  switch (breaks[i])
	    {
	    case UC_BREAK_POSSIBLE:
	      putc ('\n', stdout);
	      break;
	    case UC_BREAK_MANDATORY:
	      break;
	    case UC_BREAK_PROHIBITED:
	      break;
	    default:
	      abort ();
	    }
	  putc (input[i], stdout);
	}

      free (breaks);

      return 0;
    }
  else
    return 1;
}

#endif /* TEST */
