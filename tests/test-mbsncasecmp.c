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
  ASSERT (mbsncasecmp ("paragraph", "Paragraph", 1000000) == 0);
  ASSERT (mbsncasecmp ("paragraph", "Paragraph", 9) == 0);

  ASSERT (mbsncasecmp ("paragrapH", "parAgRaph", 1000000) == 0);
  ASSERT (mbsncasecmp ("paragrapH", "parAgRaph", 9) == 0);

  ASSERT (mbsncasecmp ("paragraph", "paraLyzed", 10) < 0);
  ASSERT (mbsncasecmp ("paragraph", "paraLyzed", 9) < 0);
  ASSERT (mbsncasecmp ("paragraph", "paraLyzed", 5) < 0);
  ASSERT (mbsncasecmp ("paragraph", "paraLyzed", 4) == 0);
  ASSERT (mbsncasecmp ("paraLyzed", "paragraph", 10) > 0);
  ASSERT (mbsncasecmp ("paraLyzed", "paragraph", 9) > 0);
  ASSERT (mbsncasecmp ("paraLyzed", "paragraph", 5) > 0);
  ASSERT (mbsncasecmp ("paraLyzed", "paragraph", 4) == 0);

  ASSERT (mbsncasecmp ("para", "paragraph", 10) < 0);
  ASSERT (mbsncasecmp ("para", "paragraph", 9) < 0);
  ASSERT (mbsncasecmp ("para", "paragraph", 5) < 0);
  ASSERT (mbsncasecmp ("para", "paragraph", 4) == 0);
  ASSERT (mbsncasecmp ("paragraph", "para", 10) > 0);
  ASSERT (mbsncasecmp ("paragraph", "para", 9) > 0);
  ASSERT (mbsncasecmp ("paragraph", "para", 5) > 0);
  ASSERT (mbsncasecmp ("paragraph", "para", 4) == 0);
}

static void
test_utf_8 (bool turkish)
{
  /* The following tests shows how mbsncasecmp() is different from
     strncasecmp().  */

  if (turkish)
    {
      ASSERT (mbsncasecmp ("\303\266zg\303\274r", "\303\226ZG\303\234R", 99) == 0); /* özgür */
      ASSERT (mbsncasecmp ("\303\226ZG\303\234R", "\303\266zg\303\274r", 99) == 0); /* özgür */

      /* This test shows how strings of different size can compare equal.  */
      ASSERT (mbsncasecmp ("turkish", "TURK\304\260SH", 7) == 0);
      ASSERT (mbsncasecmp ("TURK\304\260SH", "turkish", 7) == 0);
    }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  ASSERT (mbsncasecmp ("\341\200", "\341\200", 99) == 0);
  ASSERT (mbsncasecmp ("\341\200X", "\341\200x", 99) == 0);
  ASSERT (mbsncasecmp ("\341", "\341", 99) == 0);
  ASSERT (mbsncasecmp ("\341X", "\341x", 99) == 0);
  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  ASSERT (mbsncasecmp ("\360\221\222", "\360\221\222", 99) == 0);
  ASSERT (mbsncasecmp ("\360\221\222X", "\360\221\222x", 99) == 0);
  ASSERT (mbsncasecmp ("\360\221", "\360\221", 99) == 0);
  ASSERT (mbsncasecmp ("\360\221X", "\360\221x", 99) == 0);
  ASSERT (mbsncasecmp ("\360", "\360", 99) == 0);
  ASSERT (mbsncasecmp ("\360X", "\360x", 99) == 0);
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
