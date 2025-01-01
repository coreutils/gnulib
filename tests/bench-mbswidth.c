/* Benchmarks for mbswidth().
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <uchar.h>

#include "bench.h"
#include "bench-multibyte.h"
#include "mbswidth.h"

static void
do_test (char test, int repeat, const char *locale_name, const char *text)
{
  printf ("Test %c\n", test);
  if (setlocale (LC_ALL, locale_name) != NULL)
    {
      struct timings_state ts;
      timing_start (&ts);

      int count;
      for (count = 0; count < repeat; count++)
        {
          mbswidth (text, 0);
        }

      timing_end (&ts);
      timing_output (&ts);
    }
  else
    {
      printf ("Skipping test: locale %s not installed.\n", locale_name);
    }
  printf ("\n");
}

/* Performs some or all of the following tests:
     a - ASCII text, C locale
     b - ASCII text, UTF-8 locale
     c - French text, C locale
     d - French text, ISO-8859-1 locale
     e - French text, UTF-8 locale
     f - Greek text, C locale
     g - Greek text, ISO-8859-7 locale
     h - Greek text, UTF-8 locale
     i - Chinese text, UTF-8 locale
     j - Chinese text, GB18030 locale
   Pass the tests to be performed as first argument.  */
int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf (stderr, "Usage: %s TESTS REPETITIONS\n", argv[0]);
      fprintf (stderr, "Example: %s abcdefghij 100000\n", argv[0]);
      exit (1);
    }

  const char *tests = argv[1];
  int repeat = atoi (argv[2]);

  text_init ();

  /* Execute each test.  */
  size_t i;
  for (i = 0; i < strlen (tests); i++)
    {
      char test = tests[i];

      switch (test)
        {
        case 'a':
          do_test (test, repeat, "C", text_latin_ascii);
          break;
        case 'b':
          do_test (test, repeat, "en_US.UTF-8", text_latin_ascii);
          break;
        case 'c':
          do_test (test, repeat, "C", text_french_iso8859);
          break;
        case 'd':
          do_test (test, repeat, "fr_FR.ISO-8859-1", text_french_iso8859);
          break;
        case 'e':
          do_test (test, repeat, "en_US.UTF-8", text_french_utf8);
          break;
        case 'f':
          do_test (test, repeat, "C", text_greek_iso8859);
          break;
        case 'g':
          do_test (test, repeat, "el_GR.ISO-8859-7", text_greek_iso8859);
          break;
        case 'h':
          do_test (test, repeat, "en_US.UTF-8", text_greek_utf8);
          break;
        case 'i':
          do_test (test, repeat, "en_US.UTF-8", text_chinese_utf8);
          break;
        case 'j':
          do_test (test, repeat, "zh_CN.GB18030", text_chinese_gb18030);
          break;
        default:
          /* Ignore.  */
          ;
        }
    }

  return 0;
}
