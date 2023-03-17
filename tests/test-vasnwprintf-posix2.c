/* Test of POSIX compatible vasnwprintf() and asnwprintf() functions.
   Copyright (C) 2007-2023 Free Software Foundation, Inc.

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

#include "vasnwprintf.h"

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "macros.h"

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  /* Test that a locale dependent decimal point is used.  */
  {
    size_t length;
    wchar_t *result = asnwprintf (NULL, &length, L"%.1a", 1.0);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1,0p+0") == 0
            || wcscmp (result, L"0x2,0p-1") == 0
            || wcscmp (result, L"0x4,0p-2") == 0
            || wcscmp (result, L"0x8,0p-3") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test that a locale dependent decimal point is used.  */
  {
    size_t length;
    wchar_t *result = asnwprintf (NULL, &length, L"%.1La", 1.0L);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1,0p+0") == 0
            || wcscmp (result, L"0x2,0p-1") == 0
            || wcscmp (result, L"0x4,0p-2") == 0
            || wcscmp (result, L"0x8,0p-3") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  return 0;
}
