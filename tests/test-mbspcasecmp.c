/* Test of case-insensitive string comparison function.
   Copyright (C) 2007-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <string.h>

#include <locale.h>

#include "macros.h"

static void
test_ascii (void)
{
  {
    const char string[] = "paragraph";
    ASSERT (mbspcasecmp (string, "Paragraph") == string + 9);
  }

  {
    const char string[] = "paragrapH";
    ASSERT (mbspcasecmp (string, "parAgRaph") == string + 9);
  }

  {
    const char string[] = "paragraph";
    ASSERT (mbspcasecmp (string, "paraLyzed") == NULL);
  }

  {
    const char string[] = "paraLyzed";
    ASSERT (mbspcasecmp (string, "paragraph") == NULL);
  }

  {
    const char string[] = "para";
    ASSERT (mbspcasecmp (string, "paragraph") == NULL);
  }

  {
    const char string[] = "paragraph";
    ASSERT (mbspcasecmp (string, "para") == string + 4);
  }
}

static void
test_utf_8 (bool turkish)
{
  /* The following tests shows how mbspcasecmp() is different from
     strncasecmp().  */

  if (turkish)
    {
      {
        const char string[] = "\303\266zg\303\274rt\303\274k"; /* özgürtük */
        ASSERT (mbspcasecmp (string, "\303\226ZG\303\234R") == string + 7); /* özgür */
      }

      {
        const char string[] = "\303\226ZG\303\234Rt\303\274k"; /* özgürtük */
        ASSERT (mbspcasecmp (string, "\303\266zg\303\274r") == string + 7); /* özgür */
      }

      /* This test shows how strings of different size can compare equal.  */

      {
        const char string[] = "turkishtime";
        ASSERT (mbspcasecmp (string, "TURK\304\260SH") == string + 7);
      }

      {
        const char string[] = "TURK\304\260SHK\303\234LT\303\234R";
        ASSERT (mbspcasecmp (string, "turkish") == string + 8);
      }
    }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  {
    const char string[] = "\341\200";
    ASSERT (mbspcasecmp (string, "\341\200") == string + 2);
  }
  {
    const char string[] = "\341\200X";
    ASSERT (mbspcasecmp (string, "\341\200x") == string + 3);
  }
  {
    const char string[] = "\341";
    ASSERT (mbspcasecmp (string, "\341") == string + 1);
  }
  {
    const char string[] = "\341X";
    ASSERT (mbspcasecmp (string, "\341x") == string + 2);
  }
  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  {
    const char string[] = "\360\221\222";
    ASSERT (mbspcasecmp (string, "\360\221\222") == string + 3);
  }
  {
    const char string[] = "\360\221\222X";
    ASSERT (mbspcasecmp (string, "\360\221\222x") == string + 4);
  }
  {
    const char string[] = "\360\221";
    ASSERT (mbspcasecmp (string, "\360\221") == string + 2);
  }
  {
    const char string[] = "\360\221X";
    ASSERT (mbspcasecmp (string, "\360\221x") == string + 3);
  }
  {
    const char string[] = "\360";
    ASSERT (mbspcasecmp (string, "\360") == string + 1);
  }
  {
    const char string[] = "\360X";
    ASSERT (mbspcasecmp (string, "\360x") == string + 2);
  }
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  test_ascii ();

  if (argc > 1)
    switch (argv[1][0])
      {
      case '3':
        /* Locale encoding is UTF-8, locale is not Turkish.  */
        test_utf_8 (false);
        return test_exit_status;

      case '4':
        /* Locale encoding is UTF-8, locale is Turkish.  */
        test_utf_8 (true);
        return test_exit_status;
      }

  return 1;
}
