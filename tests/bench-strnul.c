/* Benchmark for strnul().
   Copyright (C) 2026 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <string.h>

#include "bench.h"
#include "bench-multibyte.h"

const char * volatile input;
const char * volatile output;

static _GL_ATTRIBUTE_NOINLINE void
do_strnul_test (char test, int repeat, const char *text)
{
  printf ("Test %c\n", test);

  struct timings_state ts;
  timing_start (&ts);

  for (int count = 0; count < repeat; count++)
    {
      input = text;
      output = strnul (input);
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

static _GL_ATTRIBUTE_NOINLINE void
do_strlen_test (char test, int repeat, const char *text)
{
  printf ("Test %c\n", test);

  size_t (* volatile p_strlen) (const char *) = strlen;

  struct timings_state ts;
  timing_start (&ts);

  for (int count = 0; count < repeat; count++)
    {
      input = text;
      output = input + p_strlen (input);
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

static _GL_ATTRIBUTE_NOINLINE void
do_strchr_test (char test, int repeat, const char *text)
{
  printf ("Test %c\n", test);

  char * (* volatile p_strchr) (const char *, int) = strchr;

  struct timings_state ts;
  timing_start (&ts);

  for (int count = 0; count < repeat; count++)
    {
      input = text;
      output = p_strchr (input, '\0');
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

/* Performs some or all of the following tests:
     n - strnul
     l - strlen
     c - strchr
   Pass the tests to be performed as first argument.  */
int
main (int argc, char *argv[])
{
  if (argc != 4)
    {
      fprintf (stderr, "Usage: %s TESTS LENGTH REPETITIONS\n", argv[0]);
      fprintf (stderr, "Example: %s nlc 100 1000000\n", argv[0]);
      exit (1);
    }

  const char *tests = argv[1];
  int length = atoi (argv[2]);
  int repeat = atoi (argv[3]);

  text_init ();
  if (length > strlen (text_french_iso8859))
    {
      fprintf (stderr, "LENGTH too large, should be <= %zu\n",
               strlen (text_french_iso8859));
      exit (1);
    }
  const char *text =
    text_french_iso8859 + strlen (text_french_iso8859) - length;

  /* Execute each test.  */
  for (size_t i = 0; i < strlen (tests); i++)
    {
      char test = tests[i];

      switch (test)
        {
        case 'n':
          do_strnul_test (test, repeat, text);
          break;
        case 'l':
          do_strlen_test (test, repeat, text);
          break;
        case 'c':
          do_strchr_test (test, repeat, text);
          break;
        default:
          /* Ignore.  */
          ;
        }
    }

  return 0;
}
