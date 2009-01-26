/* Test of quotearg family of functions.
   Copyright (C) 2008-2009 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Eric Blake <ebb9@byu.net>, 2008.  */

#include <config.h>

#include "quotearg.h"

#include <ctype.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "progname.h"
#include "gettext.h"

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
	{								     \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
	  fflush (stderr);						     \
	  abort ();							     \
	}								     \
    }									     \
  while (0)

struct result_strings {
  char const *str1; /* Translation of "".  */
  char const *str2; /* Translation of "\0""1\0".  */
  size_t len2; /* Length of str2.  */
  char const *str3; /* Translation of "simple".  */
  char const *str4; /* Translation of " \t\n'\"\033?""?/\\".  */
  char const *str5; /* Translation of "a:b".  */
  char const *str6; /* Translation of "a\\b".  */
};

struct result_groups {
  struct result_strings group1; /* Via quotearg_buffer.  */
  struct result_strings group2; /* Via quotearg{,_mem}.  */
  struct result_strings group3; /* Via quotearg_colon{,_mem}.  */
};

static struct result_strings inputs = {
  "", "\0001\0", 3, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b"
};

static struct result_groups results_g[] = {
  /* literal_quoting_style */
  { { "", "\0""1\0", 3, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" },
    { "", "1", 1, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" },
    { "", "1", 1, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" } },

  /* shell_quoting_style */
  { { "''", "\0""1\0", 3, "simple", "' \t\n'\\''\"\033?""?/\\'", "a:b",
      "'a\\b'" },
    { "''", "1", 1, "simple", "' \t\n'\\''\"\033?""?/\\'", "a:b",
      "'a\\b'" },
    { "''", "1", 1, "simple", "' \t\n'\\''\"\033?""?/\\'", "'a:b'",
      "'a\\b'" } },

  /* shell_always_quoting_style */
  { { "''", "'\0""1\0'", 5, "'simple'", "' \t\n'\\''\"\033?""?/\\'", "'a:b'",
      "'a\\b'" },
    { "''", "'1'", 3, "'simple'", "' \t\n'\\''\"\033?""?/\\'", "'a:b'",
      "'a\\b'" },
    { "''", "'1'", 3, "'simple'", "' \t\n'\\''\"\033?""?/\\'", "'a:b'",
      "'a\\b'" } },

  /* c_quoting_style */
  { { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a\\:b\"", "\"a\\\\b\"" } },

  /* c_maybe_quoting_style */
  { { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "a:b", "a\\b" },
    { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "a:b", "a\\b" },
    { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "\"a:b\"", "a\\b" } },

  /* escape_quoting_style */
  { { "", "\\0001\\0", 7, "simple", " \\t\\n'\"\\033?""?/\\\\", "a:b",
      "a\\\\b" },
    { "", "\\0001\\0", 7, "simple", " \\t\\n'\"\\033?""?/\\\\", "a:b",
      "a\\\\b" },
    { "", "\\0001\\0", 7, "simple", " \\t\\n'\"\\033?""?/\\\\", "a\\:b",
      "a\\\\b" } },

  /* locale_quoting_style */
  { { "`'", "`\\0001\\0'", 9, "`simple'", "` \\t\\n\\'\"\\033?""?/\\\\'",
      "`a:b'", "`a\\\\b'" },
    { "`'", "`\\0001\\0'", 9, "`simple'", "` \\t\\n\\'\"\\033?""?/\\\\'",
      "`a:b'", "`a\\\\b'" },
    { "`'", "`\\0001\\0'", 9, "`simple'", "` \\t\\n\\'\"\\033?""?/\\\\'",
      "`a\\:b'", "`a\\\\b'" } },

  /* clocale_quoting_style */
  { { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?""?/\\\\\"", "\"a\\:b\"", "\"a\\\\b\"" } }
};

static struct result_groups flag_results[] = {
  /* literal_quoting_style and QA_ELIDE_NULL_BYTES */
  { { "", "1", 1, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" },
    { "", "1", 1, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" },
    { "", "1", 1, "simple", " \t\n'\"\033?""?/\\", "a:b", "a\\b" } },

  /* c_quoting_style and QA_ELIDE_OUTER_QUOTES */
  { { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "a:b", "a\\b" },
    { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "a:b", "a\\b" },
    { "", "\"\\0001\\0\"", 9, "simple", "\" \\t\\n'\\\"\\033?""?/\\\\\"",
      "\"a:b\"", "a\\b" } },

  /* c_quoting_style and QA_SPLIT_TRIGRAPHS */
  { { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?\"\"?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?\"\"?/\\\\\"", "\"a:b\"", "\"a\\\\b\"" },
    { "\"\"", "\"\\0001\\0\"", 9, "\"simple\"",
      "\" \\t\\n'\\\"\\033?\"\"?/\\\\\"", "\"a\\:b\"", "\"a\\\\b\"" } }
};

#if ENABLE_NLS

/* These quotes are borrowed from a pt_PT.utf8 translation.  */
# define LQ "\302\253"
# define RQ "\302\273"

static struct result_groups locale_results[] = {
  /* locale_quoting_style */
  { { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a:b" RQ, LQ "a\\\\b" RQ },
    { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a:b" RQ, LQ "a\\\\b" RQ },
    { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a\\:b" RQ, LQ "a\\\\b" RQ } },

  /* clocale_quoting_style */
  { { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a:b" RQ, LQ "a\\\\b" RQ },
    { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a:b" RQ, LQ "a\\\\b" RQ },
    { LQ RQ, LQ "\\0001\\0" RQ, 11, LQ "simple" RQ,
      LQ " \\t\\n'\"\\033?""?/\\\\" RQ, LQ "a\\:b" RQ, LQ "a\\\\b" RQ } }
};

#endif /* ENABLE_NLS */

static void
compare (char const *a, size_t la, char const *b, size_t lb)
{
  ASSERT (la == lb);
  ASSERT (memcmp (a, b, la) == 0);
  ASSERT (b[lb] == '\0');
}

static void
compare_strings (char *(func) (char const *, size_t *),
		 struct result_strings *results)
{
  size_t len;
  char *p;

  len = 0;
  p = func (inputs.str1, &len);
  compare (results->str1, strlen (results->str1), p, len);

  len = inputs.len2;
  p = func (inputs.str2, &len);
  compare (results->str2, results->len2, p, len);

  len = SIZE_MAX;
  p = func (inputs.str3, &len);
  compare (results->str3, strlen (results->str3), p, len);

  len = strlen (inputs.str4);
  p = func (inputs.str4, &len);
  compare (results->str4, strlen (results->str4), p, len);

  len = SIZE_MAX;
  p = func (inputs.str5, &len);
  compare (results->str5, strlen (results->str5), p, len);

  len = strlen (inputs.str6);
  p = func (inputs.str6, &len);
  compare (results->str6, strlen (results->str6), p, len);
}

static char *
use_quotearg_buffer (const char *str, size_t *len)
{
  static char buf[100];
  size_t size;
  memset (buf, 0xa5, 100);
  size = quotearg_buffer (buf, 100, str, *len, NULL);
  *len = size;
  ASSERT ((unsigned char) buf[size + 1] == 0xa5);
  return buf;
}

static char *
use_quotearg (const char *str, size_t *len)
{
  char *p = *len == SIZE_MAX ? quotearg (str) : quotearg_mem (str, *len);
  *len = strlen (p);
  return p;
}

static char *
use_quotearg_colon (const char *str, size_t *len)
{
  char *p = (*len == SIZE_MAX ? quotearg_colon (str)
	     : quotearg_colon_mem (str, *len));
  *len = strlen (p);
  return p;
}

int
main (int argc, char *argv[])
{
  int i;

  set_program_name (argv[0]);

  /* This program part is hard-wired to the C locale since it does not call
     setlocale.  */
  ASSERT (!isprint ('\033'));
  for (i = literal_quoting_style; i <= clocale_quoting_style; i++)
    {
      set_quoting_style (NULL, i);
      compare_strings (use_quotearg_buffer, &results_g[i].group1);
      compare_strings (use_quotearg, &results_g[i].group2);
      compare_strings (use_quotearg_colon, &results_g[i].group3);
    }

  set_quoting_style (NULL, literal_quoting_style);
  ASSERT (set_quoting_flags (NULL, QA_ELIDE_NULL_BYTES) == 0);
  compare_strings (use_quotearg_buffer, &flag_results[0].group1);
  compare_strings (use_quotearg, &flag_results[0].group2);
  compare_strings (use_quotearg_colon, &flag_results[0].group3);

  set_quoting_style (NULL, c_quoting_style);
  ASSERT (set_quoting_flags (NULL, QA_ELIDE_OUTER_QUOTES)
	  == QA_ELIDE_NULL_BYTES);
  compare_strings (use_quotearg_buffer, &flag_results[1].group1);
  compare_strings (use_quotearg, &flag_results[1].group2);
  compare_strings (use_quotearg_colon, &flag_results[1].group3);

  ASSERT (set_quoting_flags (NULL, QA_SPLIT_TRIGRAPHS)
	  == QA_ELIDE_OUTER_QUOTES);
  compare_strings (use_quotearg_buffer, &flag_results[2].group1);
  compare_strings (use_quotearg, &flag_results[2].group2);
  compare_strings (use_quotearg_colon, &flag_results[2].group3);

  ASSERT (set_quoting_flags (NULL, 0) == QA_SPLIT_TRIGRAPHS);

#if ENABLE_NLS
  /* Clean up environment.  */
  unsetenv ("LANGUAGE");
  unsetenv ("LC_ALL");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LC_CTYPE");
  unsetenv ("LANG");
  unsetenv ("OUTPUT_CHARSET");

  /* This program part runs in a French UTF-8 locale.  It uses
     the test-quotearg.mo message catalog.  */
  {
    const char *locale_name = getenv ("LOCALE");

    if (locale_name != NULL && strcmp (locale_name, "none") != 0
	&& setenv ("LC_ALL", locale_name, 1) == 0
	&& setlocale (LC_ALL, "") != NULL)
      {
	textdomain ("test-quotearg");
	bindtextdomain ("test-quotearg", getenv ("LOCALEDIR"));

	set_quoting_style (NULL, locale_quoting_style);
	compare_strings (use_quotearg_buffer, &locale_results[0].group1);
	compare_strings (use_quotearg, &locale_results[0].group2);
	compare_strings (use_quotearg_colon, &locale_results[0].group3);

	set_quoting_style (NULL, clocale_quoting_style);
	compare_strings (use_quotearg_buffer, &locale_results[1].group1);
	compare_strings (use_quotearg, &locale_results[1].group2);
	compare_strings (use_quotearg_colon, &locale_results[1].group3);
      }
  }
#endif /* ENABLE_NLS */

  quotearg_free ();
  return 0;
}
