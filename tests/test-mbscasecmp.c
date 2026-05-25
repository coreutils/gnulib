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
  ASSERT (mbscasecmp ("paragraph", "Paragraph") == 0);

  ASSERT (mbscasecmp ("paragrapH", "parAgRaph") == 0);

  ASSERT (mbscasecmp ("paragraph", "paraLyzed") < 0);
  ASSERT (mbscasecmp ("paraLyzed", "paragraph") > 0);

  ASSERT (mbscasecmp ("para", "paragraph") < 0);
  ASSERT (mbscasecmp ("paragraph", "para") > 0);
}

static void
test_utf_8 (bool turkish)
{
  /* The following tests shows how mbscasecmp() is different from
     strcasecmp().  */

  if (turkish)
    {
      ASSERT (mbscasecmp ("\303\266zg\303\274r", "\303\226ZG\303\234R") == 0); /* özgür */
      ASSERT (mbscasecmp ("\303\226ZG\303\234R", "\303\266zg\303\274r") == 0); /* özgür */

      /* This test shows how strings of different size can compare equal.  */
      ASSERT (mbscasecmp ("turkish", "TURK\304\260SH") == 0);
      ASSERT (mbscasecmp ("TURK\304\260SH", "turkish") == 0);
    }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  ASSERT (mbscasecmp ("\341\200", "\341\200") == 0);
  ASSERT (mbscasecmp ("\341\200X", "\341\200x") == 0);
  ASSERT (mbscasecmp ("\341", "\341") == 0);
  ASSERT (mbscasecmp ("\341X", "\341x") == 0);
  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  ASSERT (mbscasecmp ("\360\221\222", "\360\221\222") == 0);
  ASSERT (mbscasecmp ("\360\221\222X", "\360\221\222x") == 0);
  ASSERT (mbscasecmp ("\360\221", "\360\221") == 0);
  ASSERT (mbscasecmp ("\360\221X", "\360\221x") == 0);
  ASSERT (mbscasecmp ("\360", "\360") == 0);
  ASSERT (mbscasecmp ("\360X", "\360x") == 0);
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
