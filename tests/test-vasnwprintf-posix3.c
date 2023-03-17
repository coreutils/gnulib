/* Test of POSIX compatible vasnwprintf() and asnwprintf() functions.
   Copyright (C) 2010-2023 Free Software Foundation, Inc.

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

#include "vasnwprintf.h"

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "macros.h"

/* glibc >= 2.2 supports the 'I' flag, and in glibc >= 2.2.3 the fa_IR
   locale defines the 'outdigits' to be U+06F0..U+06F9.
   So we test for glibc >= 2.3.  */
#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 3)) && !defined __UCLIBC__

static void
test_function (wchar_t * (*my_asnwprintf) (wchar_t *, size_t *, const wchar_t *, ...))
{
  /* Test that the 'I' flag is supported.  */
  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Id %d", 1234567, 99);
    static const wchar_t expected[] = /* "۱۲۳۴۵۶۷ 99" */
      {
        0x06F1, 0x06F2, 0x06F3, 0x06F4, 0x06F5, 0x06F6, 0x06F7, L' ',
        L'9', L'9', L'\0'
      };
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, expected) == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
}

static wchar_t *
my_asnwprintf (wchar_t *resultbuf, size_t *lengthp, const wchar_t *format, ...)
{
  va_list args;
  wchar_t *ret;

  va_start (args, format);
  ret = vasnwprintf (resultbuf, lengthp, format, args);
  va_end (args);
  return ret;
}

static void
test_vasnwprintf ()
{
  test_function (my_asnwprintf);
}

static void
test_asnwprintf ()
{
  test_function (asnwprintf);
}

#endif

int
main (int argc, char *argv[])
{
#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 3)) && !defined __UCLIBC__
  /* Select a locale with Arabic 'outdigits'.  */
  if (setlocale (LC_ALL, "fa_IR.UTF-8") == NULL)
    {
      fprintf (stderr, "Skipping test: no Iranian locale is installed\n");
      return 77;
    }

  test_vasnwprintf ();
  test_asnwprintf ();

  return 0;
#else
  fprintf (stderr, "Skipping test: not a glibc >= 2.3 system\n");
  return 77;
#endif
}
