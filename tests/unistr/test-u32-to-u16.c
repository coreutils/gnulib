/* Test of u32_to_u16() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include "unistr.h"

#include <errno.h>

#include "macros.h"

static int
check (const uint32_t *input, size_t input_length,
       const uint16_t *expected, size_t expected_length)
{
  size_t length;
  uint16_t *result;

  /* Test return conventions with resultbuf == NULL.  */
  result = u32_to_u16 (input, input_length, NULL, &length);
  if (!(result != NULL))
    return 1;
  if (!(length == expected_length))
    return 2;
  if (!(u16_cmp (result, expected, expected_length) == 0))
    return 3;
  free (result);

  /* Test return conventions with resultbuf too small.  */
  if (expected_length > 0)
    {
      uint16_t *preallocated;

      length = expected_length - 1;
      preallocated = (uint16_t *) malloc (length * sizeof (uint16_t));
      result = u32_to_u16 (input, input_length, preallocated, &length);
      if (!(result != NULL))
        return 4;
      if (!(result != preallocated))
        return 5;
      if (!(length == expected_length))
        return 6;
      if (!(u16_cmp (result, expected, expected_length) == 0))
        return 7;
      free (result);
      free (preallocated);
    }

  /* Test return conventions with resultbuf large enough.  */
  {
    uint16_t *preallocated;

    length = expected_length;
    preallocated = (uint16_t *) malloc (length * sizeof (uint16_t));
    result = u32_to_u16 (input, input_length, preallocated, &length);
    if (!(result != NULL))
      return 8;
    if (!(preallocated == NULL || result == preallocated))
      return 9;
    if (!(length == expected_length))
      return 10;
    if (!(u16_cmp (result, expected, expected_length) == 0))
      return 11;
    free (preallocated);
  }

  return 0;
}

int
main ()
{
  /* Empty string.  */
  ASSERT (check (NULL, 0, NULL, 0) == 0);

  /* Simple string.  */
  { /* "Grüß Gott. Здравствуйте! x=(-b±sqrt(b²-4ac))/(2a)  日本語,中文,한글" */
    static const uint32_t input[] =
      { 'G', 'r', 0x00FC, 0x00DF, ' ', 'G', 'o', 't', 't', '.', ' ',
        0x0417, 0x0434, 0x0440, 0x0430, 0x0432, 0x0441, 0x0442, 0x0432, 0x0443,
        0x0439, 0x0442, 0x0435, '!', ' ',
        'x', '=', '(', '-', 'b', 0x00B1, 's', 'q', 'r', 't', '(', 'b', 0x00B2,
        '-', '4', 'a', 'c', ')', ')', '/', '(', '2', 'a', ')', ' ', ' ',
        0x65E5, 0x672C, 0x8A9E, ',', 0x4E2D, 0x6587, ',', 0xD55C, 0xAE00, '\n'
      };
    static const uint16_t expected[] =
      { 'G', 'r', 0x00FC, 0x00DF, ' ', 'G', 'o', 't', 't', '.', ' ',
        0x0417, 0x0434, 0x0440, 0x0430, 0x0432, 0x0441, 0x0442, 0x0432, 0x0443,
        0x0439, 0x0442, 0x0435, '!', ' ',
        'x', '=', '(', '-', 'b', 0x00B1, 's', 'q', 'r', 't', '(', 'b', 0x00B2,
        '-', '4', 'a', 'c', ')', ')', '/', '(', '2', 'a', ')', ' ', ' ',
        0x65E5, 0x672C, 0x8A9E, ',', 0x4E2D, 0x6587, ',', 0xD55C, 0xAE00, '\n'
      };
    ASSERT (check (input, SIZEOF (input), expected, SIZEOF (expected)) == 0);
  }

  /* String with characters outside the BMP.  */
  {
    static const uint32_t input[] =
      { '-', '(', 0x1D51E, 0x00D7, 0x1D51F, ')', '=',
        0x1D51F, 0x00D7, 0x1D51E
      };
    static const uint16_t expected[] =
      { '-', '(', 0xD835, 0xDD1E, 0x00D7, 0xD835, 0xDD1F, ')', '=',
        0xD835, 0xDD1F, 0x00D7, 0xD835, 0xDD1E
      };
    ASSERT (check (input, SIZEOF (input), expected, SIZEOF (expected)) == 0);
  }

  /* Invalid input.  */
  {
    static const uint32_t input[] = { 'x', 0x340000, 0x50000000, 'y' };
#if 0 /* Currently invalid input is rejected, not accommodated.  */
    static const uint16_t expected[] = { 'x', 0xFFFD, 0xFFFD, 'y' };
    ASSERT (check (input, SIZEOF (input), expected, SIZEOF (expected)) == 0);
#else
    size_t length;
    uint16_t *result;
    uint16_t preallocated[10];

    /* Test return conventions with resultbuf == NULL.  */
    result = u32_to_u16 (input, SIZEOF (input), NULL, &length);
    ASSERT (result == NULL);
    ASSERT (errno == EILSEQ);

    /* Test return conventions with resultbuf too small.  */
    length = 1;
    result = u32_to_u16 (input, SIZEOF (input), preallocated, &length);
    ASSERT (result == NULL);
    ASSERT (errno == EILSEQ);

    /* Test return conventions with resultbuf large enough.  */
    length = SIZEOF (preallocated);
    result = u32_to_u16 (input, SIZEOF (input), preallocated, &length);
    ASSERT (result == NULL);
    ASSERT (errno == EILSEQ);
#endif
  }

  return test_exit_status;
}
