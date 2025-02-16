/* Test of strcasecmp() function.
   Copyright (C) 2008-2025 Free Software Foundation, Inc.

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

#include <strings.h>

#include "signature.h"
SIGNATURE_CHECK (strcasecmp, int, (const char *, const char *));

#include <locale.h>
#include <stdlib.h>

#include "macros.h"

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  ASSERT (strcasecmp ("paragraph", "Paragraph") == 0);

  ASSERT (strcasecmp ("paragrapH", "parAgRaph") == 0);

  ASSERT (strcasecmp ("paragraph", "paraLyzed") < 0);
  ASSERT (strcasecmp ("paraLyzed", "paragraph") > 0);

  ASSERT (strcasecmp ("para", "paragraph") < 0);
  ASSERT (strcasecmp ("paragraph", "para") > 0);

  if (argc > 1)
    switch (argv[1][0])
      {
      case '1':
        /* C or POSIX locale.  */
        return test_exit_status;

      case '2':
        /* Locale encoding is ISO-8859-1 or ISO-8859-15.  */

        /* U+00C9 LATIN CAPITAL LETTER E WITH ACUTE */
        /* U+00E9 LATIN SMALL LETTER E WITH ACUTE */
        ASSERT (strcasecmp ("Fej\311r", "Fej\351r") == 0);
        ASSERT (strcasecmp ("Fej\351r", "Fej\311r") == 0);
        ASSERT (strcasecmp ("Fejer", "Fej\311r") < 0);
        ASSERT (strcasecmp ("Fej\311r", "Fejer") > 0);

        /* Compare with U+00D7 MULTIPLICATION SIGN */
        ASSERT (strcasecmp ("Fej\311r", "Fej\327") > 0);
        ASSERT (strcasecmp ("Fej\327", "Fej\311r") < 0);
        ASSERT (strcasecmp ("Fej\351r", "Fej\327") > 0);
        ASSERT (strcasecmp ("Fej\327", "Fej\351r") < 0);

        return test_exit_status;
      }

  return 1;
}
