/*
 * Copyright (C) 2004, 2007, 2008 Free Software Foundation
 * Written by Bruno Haible and Eric Blake
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <string.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int
main (int argc, char *argv[])
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  All known platforms that lack alarm also have
     a quadratic strstr, and the replacement strstr is known to not
     take too long.  */
  signal (SIGALRM, SIG_DFL);
  alarm (50);
#endif

  {
    const char input[] = "foo";
    const char *result = strstr (input, "");
    ASSERT (result == input);
  }

  {
    const char input[] = "foo";
    const char *result = strstr (input, "o");
    ASSERT (result == input + 1);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = strstr (input, "ABCDABD");
    ASSERT (result == input + 15);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = strstr (input, "ABCDABE");
    ASSERT (result == NULL);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = strstr (input, "ABCDABCD");
    ASSERT (result == input + 11);
  }

  /* Check that a very long haystack is handled quickly if the needle is
     short and occurs near the beginning.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    char *needle =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    char *haystack = (char *) malloc (m + 1);
    if (haystack != NULL)
      {
	memset (haystack, 'A', m);
	haystack[0] = 'B';
	haystack[m] = '\0';

	for (; repeat > 0; repeat--)
	  {
	    ASSERT (strstr (haystack, needle) == haystack + 1);
	  }

	free (haystack);
      }
  }

  /* Check that a very long needle is discarded quickly if the haystack is
     short.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    char *haystack =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
    char *needle = (char *) malloc (m + 1);
    if (needle != NULL)
      {
	memset (needle, 'A', m);
	needle[m] = '\0';

	for (; repeat > 0; repeat--)
	  {
	    ASSERT (strstr (haystack, needle) == NULL);
	  }

	free (needle);
      }
  }

  /* Check that the asymptotic worst-case complexity is not quadratic.  */
  {
    size_t m = 1000000;
    char *haystack = (char *) malloc (2 * m + 2);
    char *needle = (char *) malloc (m + 2);
    if (haystack != NULL && needle != NULL)
      {
	const char *result;

	memset (haystack, 'A', 2 * m);
	haystack[2 * m] = 'B';
	haystack[2 * m + 1] = '\0';

	memset (needle, 'A', m);
	needle[m] = 'B';
	needle[m + 1] = '\0';

	result = strstr (haystack, needle);
	ASSERT (result == haystack + m);
      }
    free (needle);
    free (haystack);
  }

  /* Sublinear speed is only possible in memmem; strstr must examine
     every character of haystack to find its length.  */

  return 0;
}
