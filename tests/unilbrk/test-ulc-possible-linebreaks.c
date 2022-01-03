/* Test of line breaking of strings.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#include "unilbrk.h"

#include <stdlib.h>

#include "macros.h"

static void
test_function (void (*my_ulc_possible_linebreaks) (const char *, size_t, const char *, char *_UC_RESTRICT),
               int version)
{
  /* Test case n = 0.  */
  my_ulc_possible_linebreaks (NULL, 0, "GB18030", NULL);

#if HAVE_ICONV
  {
    static const char input[36] =
      /* "Grüß Gott. x=(-b±sqrt(b²-4ac))/(2a)" */
      "Gr\374\337 Gott. x=(-b\261sqrt(b\262-4ac))/(2a)\n";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_ulc_possible_linebreaks (input, SIZEOF (input), "ISO-8859-1", p);
    for (i = 0; i < 36; i++)
      {
        ASSERT (p[i] == (i == 35 ? UC_BREAK_MANDATORY :
                         i == 5 || i == 11 || i == 15
                         || i == 31 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test line breaking in a string with HTML markup.  */
  {
    static const char input[21] = "<P>Some sentence.</P>";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_ulc_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 21; i++)
      {
        ASSERT (p[i] == (i == 8 || i == 17 || i == 19 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }
#endif
}

int
main ()
{
  test_function (ulc_possible_linebreaks, 2);
#undef ulc_possible_linebreaks
  test_function (ulc_possible_linebreaks, 1);

  return 0;
}
