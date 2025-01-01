/* Benchmark mcel and some alternatives
   Copyright 2023-2025 Free Software Foundation, Inc.

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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <uchar.h>

#include "bench.h"
#include "bench-multibyte.h"
#include "mbiter.h"
#include "mbiterf.h"
#include "mbuiter.h"
#include "mbuiterf.h"
#include "mcel.h"

typedef unsigned long long (*test_function) (char const *, char const *, int);

static unsigned long long
noop_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    {
      char const *iter;
      for (iter = text; iter < text_end; iter++)
        sum += (uintptr_t) iter;
    }

  return sum;
}

static unsigned long long
single_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    for (char const *iter = text; iter < text_end; )
      {
        unsigned char c = *iter++;
        sum += c;
      }

  return sum;
}

static unsigned long long
mbiter_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  size_t text_len = text_end - text;
  for (int count = 0; count < repeat; count++)
    {
      mbi_iterator_t iter;
      for (mbi_init (iter, text, text_len); mbi_avail (iter); )
        {
          mbchar_t cur = mbi_cur (iter);
          mbi_advance (iter);
          sum += cur.wc_valid ? cur.wc : (unsigned char) *mb_ptr (cur) << 16;
        }
    }

  return sum;
}

static unsigned long long
mbiterf_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    {
      mbif_state_t state;
      char const *iter;
      for (mbif_init (state), iter = text; mbif_avail (state, iter, text_end); )
        {
          mbchar_t cur = mbif_next (state, iter, text_end);
          iter += mb_len (cur);
          sum += cur.wc_valid ? cur.wc : (unsigned char) *mb_ptr (cur) << 16;
        }
    }

  return sum;
}

static unsigned long long
mbuiter_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    for (char const *t = text; t < text_end; t++)
      {
        mbui_iterator_t iter;
        for (mbui_init (iter, t); mbui_avail (iter); )
          {
            mbchar_t cur = mbui_cur (iter);
            mbui_advance (iter);
            sum += cur.wc_valid ? cur.wc : (unsigned char) *mb_ptr (cur) << 16;
          }
        t = mbui_cur_ptr (iter);
      }

  return sum;
}

static unsigned long long
mbuiterf_test (char const *text, _GL_UNUSED char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    for (char const *t = text; t < text_end; t++)
      {
        mbuif_state_t state;
        char const *iter;
        for (mbuif_init (state), iter = t; mbuif_avail (state, iter); )
          {
            mbchar_t cur = mbuif_next (state, iter);
            iter += mb_len (cur);
            sum += cur.wc_valid ? cur.wc : (unsigned char) *mb_ptr (cur) << 16;
          }
        t = iter;
      }

  return sum;
}

static unsigned long long
mcel_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    for (char const *iter = text; iter < text_end; )
      {
        mcel_t g = mcel_scan (iter, text_end);
        iter += g.len;
        sum += g.ch | (g.err << 16);
      }

  return sum;
}

static unsigned long long
mcuel_test (char const *text, char const *text_end, int repeat)
{
  unsigned long long sum = 0;

  for (int count = 0; count < repeat; count++)
    for (char const *t = text; t < text_end; t++)
      {
        char const *iter = t;
        while (*iter)
          {
            mcel_t g = mcel_scanz (iter);
            iter += g.len;
            sum += g.ch | (g.err << 16);
          }
        t = iter;
      }

  return sum;
}

static unsigned long long
do_1_test (test_function test, char const *text,
           char const *text_end, int repeat, struct timings_state *ts)
{
  timing_start (ts);
  unsigned long long sum = test (text, text_end, repeat);
  timing_end (ts);
  return sum;
}

static void
do_test (char test, int repeat, char const *locale_name,
         char const *text, size_t text_len)
{
  if (setlocale (LC_ALL, locale_name) != NULL)
    {
      char const *text_end = text + text_len;

      static struct
      {
        char const *name;
        test_function fn;
        struct timings_state ts;
        unsigned long long volatile sum;
      } testdesc[] = {
        { "noop", noop_test },
        { "single", single_test },
        { "mbiter", mbiter_test },
        { "mbiterf", mbiterf_test },
        { "mbuiter", mbuiter_test },
        { "mbuiterf", mbuiterf_test },
        { "mcel", mcel_test },
        { "mcuel", mcuel_test },
      };
      int ntestdesc = sizeof testdesc / sizeof *testdesc;
      for (int i = 0; i < ntestdesc; i++)
        testdesc[i].sum =
          do_1_test (testdesc[i].fn, text, text_end, repeat, &testdesc[i].ts);

      setlocale (LC_ALL, "C");

      static bool header_printed;
      if (!header_printed)
        {
          printf (" ");
          for (int i = 0; i < ntestdesc; i++)
            printf (" %8s", testdesc[i].name);
          printf ("\n");
          header_printed = true;
        }

      printf ("%c", test);
      for (int i = 0; i < ntestdesc; i++)
        {
          double user_usec = testdesc[i].ts.user_usec;
          double sys_usec = testdesc[i].ts.sys_usec;
          printf (" %8.3f", (user_usec + sys_usec) / 1e6);
        }
      printf ("\n");
    }
  else
    {
      printf ("Skipping test: locale %s not installed.\n", locale_name);
    }
}

/* Performs some or all of the following tests:
     A - ASCII text, C locale
     B - ASCII text, UTF-8 locale
     C - French text, C locale
     D - French text, ISO-8859-1 locale
     E - French text, UTF-8 locale
     F - Greek text, C locale
     G - Greek text, ISO-8859-7 locale
     H - Greek text, UTF-8 locale
     I - Chinese text, UTF-8 locale
     J - Chinese text, GB18030 locale
     K - Random bytes, C locale
     L - Random bytes, UTF-8 locale
     a - short ASCII text, C locale
     b - short ASCII text, UTF-8 locale
     e - short French text, UTF-8 locale
     h - short Greek text, UTF-8 locale
     i - short Chinese text, UTF-8 locale
   Pass the tests to be performed as first argument.  */
int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf (stderr, "Usage: %s TESTS REPETITIONS\n", argv[0]);

      fprintf (stderr, "Example: %s ABCDEFGHIJKLabehi 100000\n", argv[0]);
      exit (1);
    }

  char const *tests = argv[1];
  int repeat = atoi (argv[2]);

  text_init ();

  /* Execute each test.  */
  size_t i;
  for (i = 0; i < strlen (tests); i++)
    {
      char test = tests[i];

      switch (test)
        {
        case 'A':
          do_test (test, repeat, "C", text_latin_ascii,
                   strlen (text_latin_ascii));
          break;
        case 'a':
          do_test (test, repeat, "C", TEXT_LATIN_ASCII_LINE1,
                   strlen (TEXT_LATIN_ASCII_LINE1));
          break;
        case 'B':
          do_test (test, repeat, "en_US.UTF-8", text_latin_ascii,
                   strlen (text_latin_ascii));
          break;
        case 'b':
          do_test (test, repeat, "en_US.UTF-8", TEXT_LATIN_ASCII_LINE1,
                   strlen (TEXT_LATIN_ASCII_LINE1));
          break;
        case 'C':
          do_test (test, repeat, "C", text_french_iso8859,
                   strlen (text_french_iso8859));
          break;
        case 'D':
          do_test (test, repeat, "fr_FR.ISO-8859-1", text_french_iso8859,
                   strlen (text_french_iso8859));
          break;
        case 'E':
          do_test (test, repeat, "en_US.UTF-8", text_french_utf8,
                   strlen (text_french_utf8));
          break;
        case 'e':
          do_test (test, repeat, "en_US.UTF-8", TEXT_FRENCH_UTF8_LINE1,
                   strlen (TEXT_FRENCH_UTF8_LINE1));
          break;
        case 'F':
          do_test (test, repeat, "C", text_greek_iso8859,
                   strlen (text_greek_iso8859));
          break;
        case 'G':
          do_test (test, repeat, "el_GR.ISO-8859-7", text_greek_iso8859,
                   strlen (text_greek_iso8859));
          break;
        case 'H':
          do_test (test, repeat, "en_US.UTF-8", text_greek_utf8,
                   strlen (text_greek_utf8));
          break;
        case 'h':
          do_test (test, repeat, "en_US.UTF-8", TEXT_GREEK_UTF8_LINE1,
                   strlen (TEXT_GREEK_UTF8_LINE1));
          break;
        case 'I':
          do_test (test, repeat, "en_US.UTF-8", text_chinese_utf8,
                   strlen (text_chinese_utf8));
          break;
        case 'i':
          do_test (test, repeat, "en_US.UTF-8", TEXT_CHINESE_UTF8_LINE1,
                   strlen (TEXT_CHINESE_UTF8_LINE1));
          break;
        case 'J':
          do_test (test, repeat, "zh_CN.GB18030", text_chinese_gb18030,
                   strlen (text_chinese_gb18030));
          break;
        case 'K':
          do_test (test, repeat, "C", text_random_bytes,
                   sizeof text_random_bytes - 1);
          break;
        case 'L':
          do_test (test, repeat, "en_US.UTF-8", text_random_bytes,
                   sizeof text_random_bytes - 1);
          break;
        default:
          /* Ignore.  */
          ;
        }
    }

  return 0;
}
