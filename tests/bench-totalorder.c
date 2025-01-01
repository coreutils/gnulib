/* Benchmarks for totalorder*().
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

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "infinity.h"
#include "macros.h"
#include "minus-zero.h"
#include "signed-nan.h"
#include "signed-snan.h"

int volatile tmp;

#define FINITE_VALUES \
      { -1e37 }, \
      { -1 }, \
      { -1e-5 }, \
      { 0 }, \
      { 1e-5 }, \
      { 0.5772156649015328606 }, \
      { 0.6931471805599453094 }, \
      { 0.8346268416740731863 }, \
      { 0.91596559417721901505 }, \
      { 1 }, \
      { 1.2020569031595942854 }, \
      { 1.6066951524152917638 }, \
      { 1.6180339887498948482 }, \
      { 1.6449340668482264365 }, \
      { 2.6220575542921198105 }, \
      { 2.7182818284590452354 }, \
      { 3.1415926535897932385 }, \
      { 4.66920160910299067185 }, \
      { 262537412640768743.99999999999925007 }, \
      { 1e37 }

static void
do_float_test (char test, int repeat)
{
  printf ("Test %c\n", test);

  memory_float data[] =
    {
#if HAVE_SNANF
      memory_negative_SNaNf (),
      memory_positive_SNaNf (),
#endif
      { negative_NaNf () },
      { positive_NaNf () },
      { -Infinityf () },
      { Infinityf () },
      { minus_zerof },
      FINITE_VALUES
    };
  int n = sizeof (data) / sizeof (data[0]);

  struct timings_state ts;
  timing_start (&ts);

  int count;
  for (count = 0; count < repeat; count++)
    {
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
          tmp = totalorderf (&data[i].value, &data[j].value);
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

static void
do_double_test (char test, int repeat)
{
  printf ("Test %c\n", test);

  memory_double data[] =
    {
#if HAVE_SNAND
      memory_negative_SNaNd (),
      memory_positive_SNaNd (),
#endif
      { negative_NaNd () },
      { positive_NaNd () },
      { -Infinityd () },
      { Infinityd () },
      { minus_zerod },
      FINITE_VALUES
    };
  int n = sizeof (data) / sizeof (data[0]);

  struct timings_state ts;
  timing_start (&ts);

  int count;
  for (count = 0; count < repeat; count++)
    {
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
          tmp = totalorder (&data[i].value, &data[j].value);
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

static void
do_long_double_test (char test, int repeat)
{
  printf ("Test %c\n", test);

  memory_long_double data[] =
    {
#if HAVE_SNANL
      memory_negative_SNaNl (),
      memory_positive_SNaNl (),
#endif
      { negative_NaNl () },
      { positive_NaNl () },
      { -Infinityl () },
      { Infinityl () },
      { minus_zerol },
      FINITE_VALUES
    };
  int n = sizeof (data) / sizeof (data[0]);

  struct timings_state ts;
  timing_start (&ts);

  int count;
  for (count = 0; count < repeat; count++)
    {
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
          tmp = totalorderl (&data[i].value, &data[j].value);
    }

  timing_end (&ts);
  timing_output (&ts);
  printf ("\n");
}

/* Performs some or all of the following tests:
     f - float
     d - double
     l - long double
   Pass the tests to be performed as first argument.  */
int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf (stderr, "Usage: %s TESTS REPETITIONS\n", argv[0]);
      fprintf (stderr, "Example: %s fdl 1000000\n", argv[0]);
      exit (1);
    }

  const char *tests = argv[1];
  int repeat = atoi (argv[2]);

  /* Execute each test.  */
  size_t i;
  for (i = 0; i < strlen (tests); i++)
    {
      char test = tests[i];

      switch (test)
        {
        case 'f':
          do_float_test (test, repeat);
          break;
        case 'd':
          do_double_test (test, repeat);
          break;
        case 'l':
          do_long_double_test (test, repeat);
          break;
        default:
          /* Ignore.  */
          ;
        }
    }

  return 0;
}
