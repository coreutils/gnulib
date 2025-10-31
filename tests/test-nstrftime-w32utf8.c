/* Test of nstrftime on native Windows in the UTF-8 environment.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include "strftime.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "macros.h"

int
main (int argc, char *argv[])
{
#ifdef _UCRT
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  /* Test that MB_CUR_MAX works as expected in a UTF-8 locale.  */
  ASSERT (MB_CUR_MAX >= 4);

  time_t t = 1738750272; /* 2025-02-05 10:11:12 */
  struct tm *tm = gmtime (&t);
  int ns = 123456789;
  char buf[100];

  if (strcmp (argv[1], "1") == 0)
    {
      /* Test a non-ASCII French month name.  */
      ptrdiff_t n = nstrftime (buf, sizeof (buf), "%B", tm, NULL, ns);
      ASSERT (n > 0);
      printf ("buf = |%s|\n", buf);
      fflush (stdout);
      ASSERT (STREQ (buf, "f\303\251vrier")); /* "février" */

      return test_exit_status;
    }

  if (strcmp (argv[1], "2") == 0)
    {
      /* Test a non-ASCII Japanese weekday name.  */
      ptrdiff_t n = nstrftime (buf, sizeof (buf), "%A", tm, NULL, ns);
      ASSERT (n > 0);
      printf ("buf = |%s|\n", buf);
      fflush (stdout);
      ASSERT (STREQ (buf, "\346\260\264\346\233\234\346\227\245")); /* "水曜日" */

      return test_exit_status;
    }

  return test_exit_status + 2;
#else
  fputs ("Skipping test: not using the UCRT runtime\n", stderr);
  return 77;
#endif
}
