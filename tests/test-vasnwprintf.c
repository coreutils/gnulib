/* Test of vasnwprintf() and asnwprintf() functions.
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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "macros.h"

static void
test_function (wchar_t * (*my_asnwprintf) (wchar_t *, size_t *, const wchar_t *, ...))
{
  wchar_t buf[8];
  int size;

  for (size = 0; size <= 8; size++)
    {
      size_t length = size;
      wchar_t *result = my_asnwprintf (NULL, &length, L"%d", 12345);
      ASSERT (result != NULL);
      ASSERT (wcscmp (result, L"12345") == 0);
      ASSERT (length == 5);
      free (result);
    }

  for (size = 0; size <= 8; size++)
    {
      size_t length;
      wchar_t *result;

      wmemcpy (buf, L"DEADBEEF", 8);
      length = size;
      result = my_asnwprintf (buf, &length, L"%d", 12345);
      ASSERT (result != NULL);
      ASSERT (wcscmp (result, L"12345") == 0);
      ASSERT (length == 5);
      if (size < 5 + 1)
        ASSERT (result != buf);
      ASSERT (wmemcmp (buf + size, &L"DEADBEEF"[size], 8 - size) == 0);
      if (result != buf)
        free (result);
    }

  /* Note: This test assumes IEEE 754 representation of 'double' floats.  */
  for (size = 0; size <= 8; size++)
    {
      size_t length;
      wchar_t *result;

      wmemcpy (buf, L"DEADBEEF", 8);
      length = size;
      result = my_asnwprintf (buf, &length, L"%2.0f", 1.6314159265358979e+125);
      ASSERT (result != NULL);
      /* The exact result and the result on glibc systems is
         163141592653589790215729350939528493057529598899734151772468186268423257777068536614838678161083520756952076273094236944990208
         On Cygwin, the result is
         163141592653589790215729350939528493057529600000000000000000000000000000000000000000000000000000000000000000000000000000000000
         On HP-UX 11.31 / hppa and IRIX 6.5, the result is
         163141592653589790000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
       */
      ASSERT (wcslen (result) == 126);
      ASSERT (wmemcmp (result, L"163141592653589790", 18) == 0);
      ASSERT (length == 126);
      if (size < 126 + 1)
        ASSERT (result != buf);
      ASSERT (wmemcmp (buf + size, &L"DEADBEEF"[size], 8 - size) == 0);
      if (result != buf)
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

int
main (int argc, char *argv[])
{
  test_vasnwprintf ();
  test_asnwprintf ();
  return 0;
}
