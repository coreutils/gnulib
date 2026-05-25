/* Tests of removing leading and/or trailing whitespaces.
   Copyright (C) 2023-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include "trim.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

static void
test_ascii (void)
{
  {
    char *result = trim ("");
    ASSERT (streq (result, ""));
    free (result);
    result = trim_leading ("");
    ASSERT (streq (result, ""));
    free (result);
    result = trim_trailing ("");
    ASSERT (streq (result, ""));
    free (result);
  }

  {
    char *result = trim ("  ");
    ASSERT (streq (result, ""));
    free (result);
    result = trim_leading ("  ");
    ASSERT (streq (result, ""));
    free (result);
    result = trim_trailing ("  ");
    ASSERT (streq (result, ""));
    free (result);
  }

  {
    char *result = trim ("Hello world");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_leading ("Hello world");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_trailing ("Hello world");
    ASSERT (streq (result, "Hello world"));
    free (result);
  }

  {
    char *result = trim ("   Hello world");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_leading ("   Hello world");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_trailing ("   Hello world");
    ASSERT (streq (result, "   Hello world"));
    free (result);
  }

  {
    char *result = trim ("Hello world  ");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_leading ("Hello world  ");
    ASSERT (streq (result, "Hello world  "));
    free (result);
    result = trim_trailing ("Hello world  ");
    ASSERT (streq (result, "Hello world"));
    free (result);
  }

  {
    char *result = trim ("   Hello world  ");
    ASSERT (streq (result, "Hello world"));
    free (result);
    result = trim_leading ("   Hello world  ");
    ASSERT (streq (result, "Hello world  "));
    free (result);
    result = trim_trailing ("   Hello world  ");
    ASSERT (streq (result, "   Hello world"));
    free (result);
  }
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  /* Test ASCII arguments.  */
  test_ascii ();

  if (argc > 1)
    switch (argv[1][0])
      {
      case '1':
        /* C or POSIX locale.  */
        return test_exit_status;

      case '2':
        /* Locale encoding is UTF-8.  */
        { /* U+2002 EN SPACE */
          char *result = trim ("\342\200\202\302\267foo\342\200\202");
          ASSERT (streq (result, "\302\267foo"));
          free (result);
        }
        { /* U+3000 IDEOGRAPHIC SPACE */
          char *result = trim ("\343\200\200\302\267foo\343\200\200");
          ASSERT (streq (result, "\302\267foo"));
          free (result);
        }
        /* Incomplete characters.  See
           https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
           page 128 table 3-11.  */
        /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
        {
          char *result = trim ("\342\200\202\341\200\342\200\202");
          ASSERT (streq (result, "\341\200"));
          free (result);
        }
        {
          char *result = trim ("\342\200\202\341\342\200\202");
          ASSERT (streq (result, "\341"));
          free (result);
        }
        /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
        {
          char *result = trim ("\342\200\202\360\221\222\342\200\202");
          ASSERT (streq (result, "\360\221\222"));
          free (result);
        }
        {
          char *result = trim ("\342\200\202\360\221\342\200\202");
          ASSERT (streq (result, "\360\221"));
          free (result);
        }
        {
          char *result = trim ("\342\200\202\360\342\200\202");
          ASSERT (streq (result, "\360"));
          free (result);
        }
        return test_exit_status;

      case '3':
        /* Locale encoding is GB18030.  */
        #if !((defined __APPLE__ && defined __MACH__) || defined __FreeBSD__ || defined __DragonFly__ || defined __sun)
        { /* U+2002 EN SPACE */
          char *result = trim ("\201\066\243\070\241\244foo\201\066\243\070");
          ASSERT (streq (result, "\241\244foo"));
          free (result);
        }
        #endif
        #if !((defined __APPLE__ && defined __MACH__) || defined __FreeBSD__ || defined __DragonFly__ || defined __illumos__)
        { /* U+3000 IDEOGRAPHIC SPACE */
          char *result = trim ("\241\241\241\244foo\241\241");
          ASSERT (streq (result, "\241\244foo"));
          free (result);
        }
        #endif
        return test_exit_status;
      }

  return 1;
}
