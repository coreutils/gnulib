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

#include <errno.h>
#include <float.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "macros.h"
#include "minus-zero.h"
#include "infinity.h"
#include "nan.h"

/* The SGI MIPS floating-point format does not distinguish 0.0 and -0.0.  */
static int
have_minus_zero ()
{
  static double plus_zero = 0.0;
  double minus_zero = minus_zerod;
  return memcmp (&plus_zero, &minus_zero, sizeof (double)) != 0;
}

/* Representation of an 80-bit 'long double' as an initializer for a sequence
   of 'unsigned int' words.  */
#ifdef WORDS_BIGENDIAN
# define LDBL80_WORDS(exponent,manthi,mantlo) \
    { ((unsigned int) (exponent) << 16) | ((unsigned int) (manthi) >> 16), \
      ((unsigned int) (manthi) << 16) | ((unsigned int) (mantlo) >> 16),   \
      (unsigned int) (mantlo) << 16                                        \
    }
#else
# define LDBL80_WORDS(exponent,manthi,mantlo) \
    { mantlo, manthi, exponent }
#endif

static int
wcsmatch (const wchar_t *pattern, const wchar_t *string)
{
  if (wcslen (pattern) != wcslen (string))
    return 0;
  for (; *pattern != L'\0'; pattern++, string++)
    if (*pattern != L'*' && *string != *pattern)
      return 0;
  return 1;
}

/* Test whether string[start_index..end_index-1] is a valid textual
   representation of NaN.  */
static int
wcsisnan (const wchar_t *string, size_t start_index, size_t end_index, int uppercase)
{
  if (start_index < end_index)
    {
      if (string[start_index] == L'-')
        start_index++;
      if (start_index + 3 <= end_index
          && wmemcmp (string + start_index, uppercase ? L"NAN" : L"nan", 3) == 0)
        {
          start_index += 3;
          if (start_index == end_index
              || (string[start_index] == L'(' && string[end_index - 1] == L')'))
            return 1;
        }
    }
  return 0;
}

static void
test_function (wchar_t * (*my_asnwprintf) (wchar_t *, size_t *, const wchar_t *, ...))
{
  wchar_t buf[8];
  int size;

  /* Test return value convention.  */

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
      if (size < 6)
        ASSERT (result != buf);
      ASSERT (wmemcmp (buf + size, &L"DEADBEEF"[size], 8 - size) == 0);
      if (result != buf)
        free (result);
    }

  /* Test support of size specifiers as in C99.  */

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%ju %d", (uintmax_t) 12345671, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12345671 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%zu %d", (size_t) 12345672, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12345672 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%tu %d", (ptrdiff_t) 12345673, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12345673 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", (long double) 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.5 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.922p+1 33") == 0
            || wcscmp (result, L"0x3.244p+0 33") == 0
            || wcscmp (result, L"0x6.488p-1 33") == 0
            || wcscmp (result, L"0xc.91p-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0X1.922P+1 33") == 0
            || wcscmp (result, L"-0X3.244P+0 33") == 0
            || wcscmp (result, L"-0X6.488P-1 33") == 0
            || wcscmp (result, L"-0XC.91P-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x0p+0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", minus_zerod, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0x0p+0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%a %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding near the decimal point.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0a %d", 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1p+0 33") == 0
            || wcscmp (result, L"0x2p+0 33") == 0
            || wcscmp (result, L"0x3p-1 33") == 0
            || wcscmp (result, L"0x6p-2 33") == 0
            || wcscmp (result, L"0xcp-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x2p+0 33") == 0
            || wcscmp (result, L"0x3p-1 33") == 0
            || wcscmp (result, L"0x6p-2 33") == 0
            || wcscmp (result, L"0xcp-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.1a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.8p+0 33") == 0
            || wcscmp (result, L"0x3.0p-1 33") == 0
            || wcscmp (result, L"0x6.1p-2 33") == 0
            || wcscmp (result, L"0xc.1p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.83p+0 33") == 0
            || wcscmp (result, L"0x3.05p-1 33") == 0
            || wcscmp (result, L"0x6.0ap-2 33") == 0
            || wcscmp (result, L"0xc.14p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.3a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.829p+0 33") == 0
            || wcscmp (result, L"0x3.052p-1 33") == 0
            || wcscmp (result, L"0x6.0a4p-2 33") == 0
            || wcscmp (result, L"0xc.148p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.3a %d", 1.49999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.800p+0 33") == 0
            || wcscmp (result, L"0x3.000p-1 33") == 0
            || wcscmp (result, L"0x6.000p-2 33") == 0
            || wcscmp (result, L"0xc.000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a Mac OS X 10.3.9 (Darwin 7.9) bug.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.1a %d", 1.999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.0p+1 33") == 0
            || wcscmp (result, L"0x2.0p+0 33") == 0
            || wcscmp (result, L"0x4.0p-1 33") == 0
            || wcscmp (result, L"0x8.0p-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  0x1.cp+0 33") == 0
            || wcscmp (result, L"  0x3.8p-1 33") == 0
            || wcscmp (result, L"    0x7p-2 33") == 0
            || wcscmp (result, L"    0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*a %d", 10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  0x1.cp+0 33") == 0
            || wcscmp (result, L"  0x3.8p-1 33") == 0
            || wcscmp (result, L"    0x7p-2 33") == 0
            || wcscmp (result, L"    0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*a %d", -10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0   33") == 0
            || wcscmp (result, L"0x3.8p-1   33") == 0
            || wcscmp (result, L"0x7p-2     33") == 0
            || wcscmp (result, L"0xep-3     33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.c000000000p+0 33") == 0
            || wcscmp (result, L"0x3.8000000000p-1 33") == 0
            || wcscmp (result, L"0x7.0000000000p-2 33") == 0
            || wcscmp (result, L"0xe.0000000000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Large precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.50a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
            || wcscmp (result, L"0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
            || wcscmp (result, L"0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
            || wcscmp (result, L"0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0   33") == 0
            || wcscmp (result, L"0x3.8p-1   33") == 0
            || wcscmp (result, L"0x7p-2     33") == 0
            || wcscmp (result, L"0xep-3     33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+0x1.cp+0 33") == 0
            || wcscmp (result, L"+0x3.8p-1 33") == 0
            || wcscmp (result, L"+0x7p-2 33") == 0
            || wcscmp (result, L"+0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 0x1.cp+0 33") == 0
            || wcscmp (result, L" 0x3.8p-1 33") == 0
            || wcscmp (result, L" 0x7p-2 33") == 0
            || wcscmp (result, L" 0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0 33") == 0
            || wcscmp (result, L"0x3.8p-1 33") == 0
            || wcscmp (result, L"0x7.p-2 33") == 0
            || wcscmp (result, L"0xe.p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#a %d", 1.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.p+0 33") == 0
            || wcscmp (result, L"0x2.p-1 33") == 0
            || wcscmp (result, L"0x4.p-2 33") == 0
            || wcscmp (result, L"0x8.p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x001.cp+0 33") == 0
            || wcscmp (result, L"0x003.8p-1 33") == 0
            || wcscmp (result, L"0x00007p-2 33") == 0
            || wcscmp (result, L"0x0000ep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010a %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000inf 33" is not a valid result; see
       <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (wcscmp (result, L"       inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050a %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000nan 33" is not a valid result; see
       <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", 3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.922p+1 33") == 0
            || wcscmp (result, L"0x3.244p+0 33") == 0
            || wcscmp (result, L"0x6.488p-1 33") == 0
            || wcscmp (result, L"0xc.91p-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LA %d", -3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0X1.922P+1 33") == 0
            || wcscmp (result, L"-0X3.244P+0 33") == 0
            || wcscmp (result, L"-0X6.488P-1 33") == 0
            || wcscmp (result, L"-0XC.91P-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x0p+0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", minus_zerol, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0x0p+0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    /* Note: This assertion fails under valgrind.
       Reported at <https://bugs.kde.org/show_bug.cgi?id=424044>.  */
    ASSERT (wcscmp (result, L"inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#if CHECK_PRINTF_SAFE && ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  /* asnwprintf should print something for noncanonical values.  */
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%La %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
#endif

  { /* Rounding near the decimal point.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0La %d", 1.5L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x2p+0 33") == 0
            || wcscmp (result, L"0x3p-1 33") == 0
            || wcscmp (result, L"0x6p-2 33") == 0
            || wcscmp (result, L"0xcp-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x2p+0 33") == 0
            || wcscmp (result, L"0x3p-1 33") == 0
            || wcscmp (result, L"0x6p-2 33") == 0
            || wcscmp (result, L"0xcp-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.1La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.8p+0 33") == 0
            || wcscmp (result, L"0x3.0p-1 33") == 0
            || wcscmp (result, L"0x6.1p-2 33") == 0
            || wcscmp (result, L"0xc.1p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.83p+0 33") == 0
            || wcscmp (result, L"0x3.05p-1 33") == 0
            || wcscmp (result, L"0x6.0ap-2 33") == 0
            || wcscmp (result, L"0xc.14p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.3La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.829p+0 33") == 0
            || wcscmp (result, L"0x3.052p-1 33") == 0
            || wcscmp (result, L"0x6.0a4p-2 33") == 0
            || wcscmp (result, L"0xc.148p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.3La %d", 1.49999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.800p+0 33") == 0
            || wcscmp (result, L"0x3.000p-1 33") == 0
            || wcscmp (result, L"0x6.000p-2 33") == 0
            || wcscmp (result, L"0xc.000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a Mac OS X 10.3.9 (Darwin 7.9) bug and a
       glibc 2.4 bug <https://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.1La %d", 1.999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.0p+1 33") == 0
            || wcscmp (result, L"0x2.0p+0 33") == 0
            || wcscmp (result, L"0x4.0p-1 33") == 0
            || wcscmp (result, L"0x8.0p-2 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  0x1.cp+0 33") == 0
            || wcscmp (result, L"  0x3.8p-1 33") == 0
            || wcscmp (result, L"    0x7p-2 33") == 0
            || wcscmp (result, L"    0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*La %d", 10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  0x1.cp+0 33") == 0
            || wcscmp (result, L"  0x3.8p-1 33") == 0
            || wcscmp (result, L"    0x7p-2 33") == 0
            || wcscmp (result, L"    0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*La %d", -10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0   33") == 0
            || wcscmp (result, L"0x3.8p-1   33") == 0
            || wcscmp (result, L"0x7p-2     33") == 0
            || wcscmp (result, L"0xep-3     33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.c000000000p+0 33") == 0
            || wcscmp (result, L"0x3.8000000000p-1 33") == 0
            || wcscmp (result, L"0x7.0000000000p-2 33") == 0
            || wcscmp (result, L"0xe.0000000000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Large precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.50La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
            || wcscmp (result, L"0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
            || wcscmp (result, L"0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
            || wcscmp (result, L"0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0   33") == 0
            || wcscmp (result, L"0x3.8p-1   33") == 0
            || wcscmp (result, L"0x7p-2     33") == 0
            || wcscmp (result, L"0xep-3     33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+0x1.cp+0 33") == 0
            || wcscmp (result, L"+0x3.8p-1 33") == 0
            || wcscmp (result, L"+0x7p-2 33") == 0
            || wcscmp (result, L"+0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 0x1.cp+0 33") == 0
            || wcscmp (result, L" 0x3.8p-1 33") == 0
            || wcscmp (result, L" 0x7p-2 33") == 0
            || wcscmp (result, L" 0xep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.cp+0 33") == 0
            || wcscmp (result, L"0x3.8p-1 33") == 0
            || wcscmp (result, L"0x7.p-2 33") == 0
            || wcscmp (result, L"0xe.p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#La %d", 1.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x1.p+0 33") == 0
            || wcscmp (result, L"0x2.p-1 33") == 0
            || wcscmp (result, L"0x4.p-2 33") == 0
            || wcscmp (result, L"0x8.p-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x001.cp+0 33") == 0
            || wcscmp (result, L"0x003.8p-1 33") == 0
            || wcscmp (result, L"0x00007p-2 33") == 0
            || wcscmp (result, L"0x0000ep-3 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010La %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000inf 33" is not a valid result; see
       <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (wcscmp (result, L"       inf 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050La %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000nan 33" is not a valid result; see
       <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %f format directive.  */

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", 12.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", 1234567.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234567.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37, L"0.000000" },
        { 1.234321234321234e-36, L"0.000000" },
        { 1.234321234321234e-35, L"0.000000" },
        { 1.234321234321234e-34, L"0.000000" },
        { 1.234321234321234e-33, L"0.000000" },
        { 1.234321234321234e-32, L"0.000000" },
        { 1.234321234321234e-31, L"0.000000" },
        { 1.234321234321234e-30, L"0.000000" },
        { 1.234321234321234e-29, L"0.000000" },
        { 1.234321234321234e-28, L"0.000000" },
        { 1.234321234321234e-27, L"0.000000" },
        { 1.234321234321234e-26, L"0.000000" },
        { 1.234321234321234e-25, L"0.000000" },
        { 1.234321234321234e-24, L"0.000000" },
        { 1.234321234321234e-23, L"0.000000" },
        { 1.234321234321234e-22, L"0.000000" },
        { 1.234321234321234e-21, L"0.000000" },
        { 1.234321234321234e-20, L"0.000000" },
        { 1.234321234321234e-19, L"0.000000" },
        { 1.234321234321234e-18, L"0.000000" },
        { 1.234321234321234e-17, L"0.000000" },
        { 1.234321234321234e-16, L"0.000000" },
        { 1.234321234321234e-15, L"0.000000" },
        { 1.234321234321234e-14, L"0.000000" },
        { 1.234321234321234e-13, L"0.000000" },
        { 1.234321234321234e-12, L"0.000000" },
        { 1.234321234321234e-11, L"0.000000" },
        { 1.234321234321234e-10, L"0.000000" },
        { 1.234321234321234e-9, L"0.000000" },
        { 1.234321234321234e-8, L"0.000000" },
        { 1.234321234321234e-7, L"0.000000" },
        { 1.234321234321234e-6, L"0.000001" },
        { 1.234321234321234e-5, L"0.000012" },
        { 1.234321234321234e-4, L"0.000123" },
        { 1.234321234321234e-3, L"0.001234" },
        { 1.234321234321234e-2, L"0.012343" },
        { 1.234321234321234e-1, L"0.123432" },
        { 1.234321234321234, L"1.234321" },
        { 1.234321234321234e1, L"12.343212" },
        { 1.234321234321234e2, L"123.432123" },
        { 1.234321234321234e3, L"1234.321234" },
        { 1.234321234321234e4, L"12343.212343" },
        { 1.234321234321234e5, L"123432.123432" },
        { 1.234321234321234e6, L"1234321.234321" },
        { 1.234321234321234e7, L"12343212.343212" },
        { 1.234321234321234e8, L"123432123.432123" },
        { 1.234321234321234e9, L"1234321234.321234" },
        { 1.234321234321234e10, L"12343212343.2123**" },
        { 1.234321234321234e11, L"123432123432.123***" },
        { 1.234321234321234e12, L"1234321234321.23****" },
        { 1.234321234321234e13, L"12343212343212.3*****" },
        { 1.234321234321234e14, L"123432123432123.******" },
        { 1.234321234321234e15, L"1234321234321234.000000" },
        { 1.234321234321234e16, L"123432123432123**.000000" },
        { 1.234321234321234e17, L"123432123432123***.000000" },
        { 1.234321234321234e18, L"123432123432123****.000000" },
        { 1.234321234321234e19, L"123432123432123*****.000000" },
        { 1.234321234321234e20, L"123432123432123******.000000" },
        { 1.234321234321234e21, L"123432123432123*******.000000" },
        { 1.234321234321234e22, L"123432123432123********.000000" },
        { 1.234321234321234e23, L"123432123432123*********.000000" },
        { 1.234321234321234e24, L"123432123432123**********.000000" },
        { 1.234321234321234e25, L"123432123432123***********.000000" },
        { 1.234321234321234e26, L"123432123432123************.000000" },
        { 1.234321234321234e27, L"123432123432123*************.000000" },
        { 1.234321234321234e28, L"123432123432123**************.000000" },
        { 1.234321234321234e29, L"123432123432123***************.000000" },
        { 1.234321234321234e30, L"123432123432123****************.000000" },
        { 1.234321234321234e31, L"123432123432123*****************.000000" },
        { 1.234321234321234e32, L"123432123432123******************.000000" },
        { 1.234321234321234e33, L"123432123432123*******************.000000" },
        { 1.234321234321234e34, L"123432123432123********************.000000" },
        { 1.234321234321234e35, L"123432123432123*********************.000000" },
        { 1.234321234321234e36, L"123432123432123**********************.000000" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%f", data[k].value);
        ASSERT (result != NULL);
        ASSERT (wcsmatch (data[k].string, result));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", -0.03125, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.031250 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", minus_zerod, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%f %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*f %d", 10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*f %d", -10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.f %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2. 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015f %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00001234.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015f %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050f %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.f %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2f %d", 999.951, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2f %d", 999.996, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000.00 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", 12.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", 1234567.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234567.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37L, L"0.000000" },
        { 1.234321234321234e-36L, L"0.000000" },
        { 1.234321234321234e-35L, L"0.000000" },
        { 1.234321234321234e-34L, L"0.000000" },
        { 1.234321234321234e-33L, L"0.000000" },
        { 1.234321234321234e-32L, L"0.000000" },
        { 1.234321234321234e-31L, L"0.000000" },
        { 1.234321234321234e-30L, L"0.000000" },
        { 1.234321234321234e-29L, L"0.000000" },
        { 1.234321234321234e-28L, L"0.000000" },
        { 1.234321234321234e-27L, L"0.000000" },
        { 1.234321234321234e-26L, L"0.000000" },
        { 1.234321234321234e-25L, L"0.000000" },
        { 1.234321234321234e-24L, L"0.000000" },
        { 1.234321234321234e-23L, L"0.000000" },
        { 1.234321234321234e-22L, L"0.000000" },
        { 1.234321234321234e-21L, L"0.000000" },
        { 1.234321234321234e-20L, L"0.000000" },
        { 1.234321234321234e-19L, L"0.000000" },
        { 1.234321234321234e-18L, L"0.000000" },
        { 1.234321234321234e-17L, L"0.000000" },
        { 1.234321234321234e-16L, L"0.000000" },
        { 1.234321234321234e-15L, L"0.000000" },
        { 1.234321234321234e-14L, L"0.000000" },
        { 1.234321234321234e-13L, L"0.000000" },
        { 1.234321234321234e-12L, L"0.000000" },
        { 1.234321234321234e-11L, L"0.000000" },
        { 1.234321234321234e-10L, L"0.000000" },
        { 1.234321234321234e-9L, L"0.000000" },
        { 1.234321234321234e-8L, L"0.000000" },
        { 1.234321234321234e-7L, L"0.000000" },
        { 1.234321234321234e-6L, L"0.000001" },
        { 1.234321234321234e-5L, L"0.000012" },
        { 1.234321234321234e-4L, L"0.000123" },
        { 1.234321234321234e-3L, L"0.001234" },
        { 1.234321234321234e-2L, L"0.012343" },
        { 1.234321234321234e-1L, L"0.123432" },
        { 1.234321234321234L, L"1.234321" },
        { 1.234321234321234e1L, L"12.343212" },
        { 1.234321234321234e2L, L"123.432123" },
        { 1.234321234321234e3L, L"1234.321234" },
        { 1.234321234321234e4L, L"12343.212343" },
        { 1.234321234321234e5L, L"123432.123432" },
        { 1.234321234321234e6L, L"1234321.234321" },
        { 1.234321234321234e7L, L"12343212.343212" },
        { 1.234321234321234e8L, L"123432123.432123" },
        { 1.234321234321234e9L, L"1234321234.321234" },
        { 1.234321234321234e10L, L"12343212343.2123**" },
        { 1.234321234321234e11L, L"123432123432.123***" },
        { 1.234321234321234e12L, L"1234321234321.23****" },
        { 1.234321234321234e13L, L"12343212343212.3*****" },
        { 1.234321234321234e14L, L"123432123432123.******" },
        { 1.234321234321234e15L, L"1234321234321234.000000" },
        { 1.234321234321234e16L, L"123432123432123**.000000" },
        { 1.234321234321234e17L, L"123432123432123***.000000" },
        { 1.234321234321234e18L, L"123432123432123****.000000" },
        { 1.234321234321234e19L, L"123432123432123*****.000000" },
        { 1.234321234321234e20L, L"123432123432123******.000000" },
        { 1.234321234321234e21L, L"123432123432123*******.000000" },
        { 1.234321234321234e22L, L"123432123432123********.000000" },
        { 1.234321234321234e23L, L"123432123432123*********.000000" },
        { 1.234321234321234e24L, L"123432123432123**********.000000" },
        { 1.234321234321234e25L, L"123432123432123***********.000000" },
        { 1.234321234321234e26L, L"123432123432123************.000000" },
        { 1.234321234321234e27L, L"123432123432123*************.000000" },
        { 1.234321234321234e28L, L"123432123432123**************.000000" },
        { 1.234321234321234e29L, L"123432123432123***************.000000" },
        { 1.234321234321234e30L, L"123432123432123****************.000000" },
        { 1.234321234321234e31L, L"123432123432123*****************.000000" },
        { 1.234321234321234e32L, L"123432123432123******************.000000" },
        { 1.234321234321234e33L, L"123432123432123*******************.000000" },
        { 1.234321234321234e34L, L"123432123432123********************.000000" },
        { 1.234321234321234e35L, L"123432123432123*********************.000000" },
        { 1.234321234321234e36L, L"123432123432123**********************.000000" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%Lf", data[k].value);
        ASSERT (result != NULL);
        ASSERT (wcsmatch (data[k].string, result));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", -0.03125L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.031250 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", minus_zerol, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#if CHECK_PRINTF_SAFE && ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  /* asnwprintf should print something for noncanonical values.  */
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lf %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
#endif

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Lf %d", 10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"  1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Lf %d", -10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.Lf %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2. 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00001234.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015Lf %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050Lf %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2Lf %d", 999.951L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2Lf %d", 999.996L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000.00 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %F format directive.  */

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", 12.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", 1234567.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234567.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", -0.03125, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.031250 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", minus_zerod, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"INF 33") == 0
            || wcscmp (result, L"INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-INF 33") == 0
            || wcscmp (result, L"-INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%F %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 1)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015F %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00001234.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015F %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -INF 33") == 0
            || wcscmp (result, L"      -INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.F %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2F %d", 999.951, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2F %d", 999.996, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000.00 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", 12.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.750000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", 1234567.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234567.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", -0.03125L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.031250 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", minus_zerol, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"INF 33") == 0
            || wcscmp (result, L"INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-INF 33") == 0
            || wcscmp (result, L"-INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%LF %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 1)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015LF %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00001234.000000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015LF %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -INF 33") == 0
            || wcscmp (result, L"      -INFINITY 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.LF %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2LF %d", 999.951L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.2LF %d", 999.996L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000.00 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %e format directive.  */

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", 12.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.275000e+01 33") == 0
            || wcscmp (result, L"1.275000e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", 1234567.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.234567e+06 33") == 0
            || wcscmp (result, L"1.234567e+006 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37, L"1.234321e-37" },
        { 1.234321234321234e-36, L"1.234321e-36" },
        { 1.234321234321234e-35, L"1.234321e-35" },
        { 1.234321234321234e-34, L"1.234321e-34" },
        { 1.234321234321234e-33, L"1.234321e-33" },
        { 1.234321234321234e-32, L"1.234321e-32" },
        { 1.234321234321234e-31, L"1.234321e-31" },
        { 1.234321234321234e-30, L"1.234321e-30" },
        { 1.234321234321234e-29, L"1.234321e-29" },
        { 1.234321234321234e-28, L"1.234321e-28" },
        { 1.234321234321234e-27, L"1.234321e-27" },
        { 1.234321234321234e-26, L"1.234321e-26" },
        { 1.234321234321234e-25, L"1.234321e-25" },
        { 1.234321234321234e-24, L"1.234321e-24" },
        { 1.234321234321234e-23, L"1.234321e-23" },
        { 1.234321234321234e-22, L"1.234321e-22" },
        { 1.234321234321234e-21, L"1.234321e-21" },
        { 1.234321234321234e-20, L"1.234321e-20" },
        { 1.234321234321234e-19, L"1.234321e-19" },
        { 1.234321234321234e-18, L"1.234321e-18" },
        { 1.234321234321234e-17, L"1.234321e-17" },
        { 1.234321234321234e-16, L"1.234321e-16" },
        { 1.234321234321234e-15, L"1.234321e-15" },
        { 1.234321234321234e-14, L"1.234321e-14" },
        { 1.234321234321234e-13, L"1.234321e-13" },
        { 1.234321234321234e-12, L"1.234321e-12" },
        { 1.234321234321234e-11, L"1.234321e-11" },
        { 1.234321234321234e-10, L"1.234321e-10" },
        { 1.234321234321234e-9, L"1.234321e-09" },
        { 1.234321234321234e-8, L"1.234321e-08" },
        { 1.234321234321234e-7, L"1.234321e-07" },
        { 1.234321234321234e-6, L"1.234321e-06" },
        { 1.234321234321234e-5, L"1.234321e-05" },
        { 1.234321234321234e-4, L"1.234321e-04" },
        { 1.234321234321234e-3, L"1.234321e-03" },
        { 1.234321234321234e-2, L"1.234321e-02" },
        { 1.234321234321234e-1, L"1.234321e-01" },
        { 1.234321234321234, L"1.234321e+00" },
        { 1.234321234321234e1, L"1.234321e+01" },
        { 1.234321234321234e2, L"1.234321e+02" },
        { 1.234321234321234e3, L"1.234321e+03" },
        { 1.234321234321234e4, L"1.234321e+04" },
        { 1.234321234321234e5, L"1.234321e+05" },
        { 1.234321234321234e6, L"1.234321e+06" },
        { 1.234321234321234e7, L"1.234321e+07" },
        { 1.234321234321234e8, L"1.234321e+08" },
        { 1.234321234321234e9, L"1.234321e+09" },
        { 1.234321234321234e10, L"1.234321e+10" },
        { 1.234321234321234e11, L"1.234321e+11" },
        { 1.234321234321234e12, L"1.234321e+12" },
        { 1.234321234321234e13, L"1.234321e+13" },
        { 1.234321234321234e14, L"1.234321e+14" },
        { 1.234321234321234e15, L"1.234321e+15" },
        { 1.234321234321234e16, L"1.234321e+16" },
        { 1.234321234321234e17, L"1.234321e+17" },
        { 1.234321234321234e18, L"1.234321e+18" },
        { 1.234321234321234e19, L"1.234321e+19" },
        { 1.234321234321234e20, L"1.234321e+20" },
        { 1.234321234321234e21, L"1.234321e+21" },
        { 1.234321234321234e22, L"1.234321e+22" },
        { 1.234321234321234e23, L"1.234321e+23" },
        { 1.234321234321234e24, L"1.234321e+24" },
        { 1.234321234321234e25, L"1.234321e+25" },
        { 1.234321234321234e26, L"1.234321e+26" },
        { 1.234321234321234e27, L"1.234321e+27" },
        { 1.234321234321234e28, L"1.234321e+28" },
        { 1.234321234321234e29, L"1.234321e+29" },
        { 1.234321234321234e30, L"1.234321e+30" },
        { 1.234321234321234e31, L"1.234321e+31" },
        { 1.234321234321234e32, L"1.234321e+32" },
        { 1.234321234321234e33, L"1.234321e+33" },
        { 1.234321234321234e34, L"1.234321e+34" },
        { 1.234321234321234e35, L"1.234321e+35" },
        { 1.234321234321234e36, L"1.234321e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%e", data[k].value);
        const wchar_t *expected = data[k].string;
        ASSERT (result != NULL);
        ASSERT (wcscmp (result, expected) == 0
                /* Some implementations produce exponents with 3 digits.  */
                || (wcslen (result) == wcslen (expected) + 1
                    && wmemcmp (result, expected, wcslen (expected) - 2) == 0
                    && result[wcslen (expected) - 2] == '0'
                    && wcscmp (result + wcslen (expected) - 1,
                               expected + wcslen (expected) - 2)
                       == 0));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", -0.03125, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-3.125000e-02 33") == 0
            || wcscmp (result, L"-3.125000e-002 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000e+00 33") == 0
            || wcscmp (result, L"0.000000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", minus_zerod, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000e+00 33") == 0
              || wcscmp (result, L"-0.000000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%e %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%15e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   1.750000e+00 33") == 0
            || wcscmp (result, L"  1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*e %d", 15, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   1.750000e+00 33") == 0
            || wcscmp (result, L"  1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*e %d", -15, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00    33") == 0
            || wcscmp (result, L"1.750000e+000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-15e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00    33") == 0
            || wcscmp (result, L"1.750000e+000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.750000e+00 33") == 0
            || wcscmp (result, L"+1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.750000e+00 33") == 0
            || wcscmp (result, L" 1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00 33") == 0
            || wcscmp (result, L"1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.e %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2.e+00 33") == 0
            || wcscmp (result, L"2.e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.e %d", 9.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.e+01 33") == 0
            || wcscmp (result, L"1.e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015e %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0001.234000e+03 33") == 0
            || wcscmp (result, L"001.234000e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015e %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050e %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.e %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1e+03 33") == 0
            || wcscmp (result, L"1e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4e %d", 999.951, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"9.9995e+02 33") == 0
            || wcscmp (result, L"9.9995e+002 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4e %d", 999.996, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.0000e+03 33") == 0
            || wcscmp (result, L"1.0000e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", 12.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.275000e+01 33") == 0
            || wcscmp (result, L"1.275000e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", 1234567.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.234567e+06 33") == 0
            || wcscmp (result, L"1.234567e+006 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37L, L"1.234321e-37" },
        { 1.234321234321234e-36L, L"1.234321e-36" },
        { 1.234321234321234e-35L, L"1.234321e-35" },
        { 1.234321234321234e-34L, L"1.234321e-34" },
        { 1.234321234321234e-33L, L"1.234321e-33" },
        { 1.234321234321234e-32L, L"1.234321e-32" },
        { 1.234321234321234e-31L, L"1.234321e-31" },
        { 1.234321234321234e-30L, L"1.234321e-30" },
        { 1.234321234321234e-29L, L"1.234321e-29" },
        { 1.234321234321234e-28L, L"1.234321e-28" },
        { 1.234321234321234e-27L, L"1.234321e-27" },
        { 1.234321234321234e-26L, L"1.234321e-26" },
        { 1.234321234321234e-25L, L"1.234321e-25" },
        { 1.234321234321234e-24L, L"1.234321e-24" },
        { 1.234321234321234e-23L, L"1.234321e-23" },
        { 1.234321234321234e-22L, L"1.234321e-22" },
        { 1.234321234321234e-21L, L"1.234321e-21" },
        { 1.234321234321234e-20L, L"1.234321e-20" },
        { 1.234321234321234e-19L, L"1.234321e-19" },
        { 1.234321234321234e-18L, L"1.234321e-18" },
        { 1.234321234321234e-17L, L"1.234321e-17" },
        { 1.234321234321234e-16L, L"1.234321e-16" },
        { 1.234321234321234e-15L, L"1.234321e-15" },
        { 1.234321234321234e-14L, L"1.234321e-14" },
        { 1.234321234321234e-13L, L"1.234321e-13" },
        { 1.234321234321234e-12L, L"1.234321e-12" },
        { 1.234321234321234e-11L, L"1.234321e-11" },
        { 1.234321234321234e-10L, L"1.234321e-10" },
        { 1.234321234321234e-9L, L"1.234321e-09" },
        { 1.234321234321234e-8L, L"1.234321e-08" },
        { 1.234321234321234e-7L, L"1.234321e-07" },
        { 1.234321234321234e-6L, L"1.234321e-06" },
        { 1.234321234321234e-5L, L"1.234321e-05" },
        { 1.234321234321234e-4L, L"1.234321e-04" },
        { 1.234321234321234e-3L, L"1.234321e-03" },
        { 1.234321234321234e-2L, L"1.234321e-02" },
        { 1.234321234321234e-1L, L"1.234321e-01" },
        { 1.234321234321234L, L"1.234321e+00" },
        { 1.234321234321234e1L, L"1.234321e+01" },
        { 1.234321234321234e2L, L"1.234321e+02" },
        { 1.234321234321234e3L, L"1.234321e+03" },
        { 1.234321234321234e4L, L"1.234321e+04" },
        { 1.234321234321234e5L, L"1.234321e+05" },
        { 1.234321234321234e6L, L"1.234321e+06" },
        { 1.234321234321234e7L, L"1.234321e+07" },
        { 1.234321234321234e8L, L"1.234321e+08" },
        { 1.234321234321234e9L, L"1.234321e+09" },
        { 1.234321234321234e10L, L"1.234321e+10" },
        { 1.234321234321234e11L, L"1.234321e+11" },
        { 1.234321234321234e12L, L"1.234321e+12" },
        { 1.234321234321234e13L, L"1.234321e+13" },
        { 1.234321234321234e14L, L"1.234321e+14" },
        { 1.234321234321234e15L, L"1.234321e+15" },
        { 1.234321234321234e16L, L"1.234321e+16" },
        { 1.234321234321234e17L, L"1.234321e+17" },
        { 1.234321234321234e18L, L"1.234321e+18" },
        { 1.234321234321234e19L, L"1.234321e+19" },
        { 1.234321234321234e20L, L"1.234321e+20" },
        { 1.234321234321234e21L, L"1.234321e+21" },
        { 1.234321234321234e22L, L"1.234321e+22" },
        { 1.234321234321234e23L, L"1.234321e+23" },
        { 1.234321234321234e24L, L"1.234321e+24" },
        { 1.234321234321234e25L, L"1.234321e+25" },
        { 1.234321234321234e26L, L"1.234321e+26" },
        { 1.234321234321234e27L, L"1.234321e+27" },
        { 1.234321234321234e28L, L"1.234321e+28" },
        { 1.234321234321234e29L, L"1.234321e+29" },
        { 1.234321234321234e30L, L"1.234321e+30" },
        { 1.234321234321234e31L, L"1.234321e+31" },
        { 1.234321234321234e32L, L"1.234321e+32" },
        { 1.234321234321234e33L, L"1.234321e+33" },
        { 1.234321234321234e34L, L"1.234321e+34" },
        { 1.234321234321234e35L, L"1.234321e+35" },
        { 1.234321234321234e36L, L"1.234321e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%Le", data[k].value);
        const wchar_t *expected = data[k].string;
        ASSERT (result != NULL);
        ASSERT (wcscmp (result, expected) == 0
                /* Some implementations produce exponents with 3 digits.  */
                || (wcslen (result) == wcslen (expected) + 1
                    && wmemcmp (result, expected, wcslen (expected) - 2) == 0
                    && result[wcslen (expected) - 2] == '0'
                    && wcscmp (result + wcslen (expected) - 1,
                               expected + wcslen (expected) - 2)
                       == 0));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", -0.03125L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-3.125000e-02 33") == 0
            || wcscmp (result, L"-3.125000e-002 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0.000000e+00 33") == 0
            || wcscmp (result, L"0.000000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", minus_zerol, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0.000000e+00 33") == 0
              || wcscmp (result, L"-0.000000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#if CHECK_PRINTF_SAFE && ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  /* asnwprintf should print something even for noncanonical values.  */
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Le %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
#endif

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%15Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   1.750000e+00 33") == 0
            || wcscmp (result, L"  1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Le %d", 15, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   1.750000e+00 33") == 0
            || wcscmp (result, L"  1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Le %d", -15, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00    33") == 0
            || wcscmp (result, L"1.750000e+000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-15Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00    33") == 0
            || wcscmp (result, L"1.750000e+000   33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.750000e+00 33") == 0
            || wcscmp (result, L"+1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.750000e+00 33") == 0
            || wcscmp (result, L" 1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.750000e+00 33") == 0
            || wcscmp (result, L"1.750000e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.Le %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2.e+00 33") == 0
            || wcscmp (result, L"2.e+000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.Le %d", 9.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.e+01 33") == 0
            || wcscmp (result, L"1.e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015Le %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0001.234000e+03 33") == 0
            || wcscmp (result, L"001.234000e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015Le %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050Le %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.Le %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1e+03 33") == 0
            || wcscmp (result, L"1e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4Le %d", 999.951L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"9.9995e+02 33") == 0
            || wcscmp (result, L"9.9995e+002 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4Le %d", 999.996L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.0000e+03 33") == 0
            || wcscmp (result, L"1.0000e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %g format directive.  */

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", 12.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", 1234567.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.23457e+06 33") == 0
            || wcscmp (result, L"1.23457e+006 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37, L"1.23432e-37" },
        { 1.234321234321234e-36, L"1.23432e-36" },
        { 1.234321234321234e-35, L"1.23432e-35" },
        { 1.234321234321234e-34, L"1.23432e-34" },
        { 1.234321234321234e-33, L"1.23432e-33" },
        { 1.234321234321234e-32, L"1.23432e-32" },
        { 1.234321234321234e-31, L"1.23432e-31" },
        { 1.234321234321234e-30, L"1.23432e-30" },
        { 1.234321234321234e-29, L"1.23432e-29" },
        { 1.234321234321234e-28, L"1.23432e-28" },
        { 1.234321234321234e-27, L"1.23432e-27" },
        { 1.234321234321234e-26, L"1.23432e-26" },
        { 1.234321234321234e-25, L"1.23432e-25" },
        { 1.234321234321234e-24, L"1.23432e-24" },
        { 1.234321234321234e-23, L"1.23432e-23" },
        { 1.234321234321234e-22, L"1.23432e-22" },
        { 1.234321234321234e-21, L"1.23432e-21" },
        { 1.234321234321234e-20, L"1.23432e-20" },
        { 1.234321234321234e-19, L"1.23432e-19" },
        { 1.234321234321234e-18, L"1.23432e-18" },
        { 1.234321234321234e-17, L"1.23432e-17" },
        { 1.234321234321234e-16, L"1.23432e-16" },
        { 1.234321234321234e-15, L"1.23432e-15" },
        { 1.234321234321234e-14, L"1.23432e-14" },
        { 1.234321234321234e-13, L"1.23432e-13" },
        { 1.234321234321234e-12, L"1.23432e-12" },
        { 1.234321234321234e-11, L"1.23432e-11" },
        { 1.234321234321234e-10, L"1.23432e-10" },
        { 1.234321234321234e-9, L"1.23432e-09" },
        { 1.234321234321234e-8, L"1.23432e-08" },
        { 1.234321234321234e-7, L"1.23432e-07" },
        { 1.234321234321234e-6, L"1.23432e-06" },
        { 1.234321234321234e-5, L"1.23432e-05" },
        { 1.234321234321234e-4, L"0.000123432" },
        { 1.234321234321234e-3, L"0.00123432" },
        { 1.234321234321234e-2, L"0.0123432" },
        { 1.234321234321234e-1, L"0.123432" },
        { 1.234321234321234, L"1.23432" },
        { 1.234321234321234e1, L"12.3432" },
        { 1.234321234321234e2, L"123.432" },
        { 1.234321234321234e3, L"1234.32" },
        { 1.234321234321234e4, L"12343.2" },
        { 1.234321234321234e5, L"123432" },
        { 1.234321234321234e6, L"1.23432e+06" },
        { 1.234321234321234e7, L"1.23432e+07" },
        { 1.234321234321234e8, L"1.23432e+08" },
        { 1.234321234321234e9, L"1.23432e+09" },
        { 1.234321234321234e10, L"1.23432e+10" },
        { 1.234321234321234e11, L"1.23432e+11" },
        { 1.234321234321234e12, L"1.23432e+12" },
        { 1.234321234321234e13, L"1.23432e+13" },
        { 1.234321234321234e14, L"1.23432e+14" },
        { 1.234321234321234e15, L"1.23432e+15" },
        { 1.234321234321234e16, L"1.23432e+16" },
        { 1.234321234321234e17, L"1.23432e+17" },
        { 1.234321234321234e18, L"1.23432e+18" },
        { 1.234321234321234e19, L"1.23432e+19" },
        { 1.234321234321234e20, L"1.23432e+20" },
        { 1.234321234321234e21, L"1.23432e+21" },
        { 1.234321234321234e22, L"1.23432e+22" },
        { 1.234321234321234e23, L"1.23432e+23" },
        { 1.234321234321234e24, L"1.23432e+24" },
        { 1.234321234321234e25, L"1.23432e+25" },
        { 1.234321234321234e26, L"1.23432e+26" },
        { 1.234321234321234e27, L"1.23432e+27" },
        { 1.234321234321234e28, L"1.23432e+28" },
        { 1.234321234321234e29, L"1.23432e+29" },
        { 1.234321234321234e30, L"1.23432e+30" },
        { 1.234321234321234e31, L"1.23432e+31" },
        { 1.234321234321234e32, L"1.23432e+32" },
        { 1.234321234321234e33, L"1.23432e+33" },
        { 1.234321234321234e34, L"1.23432e+34" },
        { 1.234321234321234e35, L"1.23432e+35" },
        { 1.234321234321234e36, L"1.23432e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%g", data[k].value);
        const wchar_t *expected = data[k].string;
        ASSERT (result != NULL);
        ASSERT (wcscmp (result, expected) == 0
                /* Some implementations produce exponents with 3 digits.  */
                || (expected[wcslen (expected) - 4] == 'e'
                    && wcslen (result) == wcslen (expected) + 1
                    && wmemcmp (result, expected, wcslen (expected) - 2) == 0
                    && result[wcslen (expected) - 2] == '0'
                    && wcscmp (result + wcslen (expected) - 1,
                               expected + wcslen (expected) - 2)
                       == 0));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", -0.03125, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.03125 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", minus_zerod, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%g %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*g %d", 10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*g %d", -10, 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.g %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2. 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.g %d", 9.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.e+01 33") == 0
            || wcscmp (result, L"1.e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010g %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0000001234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015g %d", - Infinityd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050g %d", NaNd (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.g %d", 1234.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1e+03 33") == 0
            || wcscmp (result, L"1e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.5g %d", 999.951, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.5g %d", 999.996, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", 12.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"12.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A larger positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", 1234567.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.23457e+06 33") == 0
            || wcscmp (result, L"1.23457e+006 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const wchar_t *string; } data[] =
      {
        { 1.234321234321234e-37L, L"1.23432e-37" },
        { 1.234321234321234e-36L, L"1.23432e-36" },
        { 1.234321234321234e-35L, L"1.23432e-35" },
        { 1.234321234321234e-34L, L"1.23432e-34" },
        { 1.234321234321234e-33L, L"1.23432e-33" },
        { 1.234321234321234e-32L, L"1.23432e-32" },
        { 1.234321234321234e-31L, L"1.23432e-31" },
        { 1.234321234321234e-30L, L"1.23432e-30" },
        { 1.234321234321234e-29L, L"1.23432e-29" },
        { 1.234321234321234e-28L, L"1.23432e-28" },
        { 1.234321234321234e-27L, L"1.23432e-27" },
        { 1.234321234321234e-26L, L"1.23432e-26" },
        { 1.234321234321234e-25L, L"1.23432e-25" },
        { 1.234321234321234e-24L, L"1.23432e-24" },
        { 1.234321234321234e-23L, L"1.23432e-23" },
        { 1.234321234321234e-22L, L"1.23432e-22" },
        { 1.234321234321234e-21L, L"1.23432e-21" },
        { 1.234321234321234e-20L, L"1.23432e-20" },
        { 1.234321234321234e-19L, L"1.23432e-19" },
        { 1.234321234321234e-18L, L"1.23432e-18" },
        { 1.234321234321234e-17L, L"1.23432e-17" },
        { 1.234321234321234e-16L, L"1.23432e-16" },
        { 1.234321234321234e-15L, L"1.23432e-15" },
        { 1.234321234321234e-14L, L"1.23432e-14" },
        { 1.234321234321234e-13L, L"1.23432e-13" },
        { 1.234321234321234e-12L, L"1.23432e-12" },
        { 1.234321234321234e-11L, L"1.23432e-11" },
        { 1.234321234321234e-10L, L"1.23432e-10" },
        { 1.234321234321234e-9L, L"1.23432e-09" },
        { 1.234321234321234e-8L, L"1.23432e-08" },
        { 1.234321234321234e-7L, L"1.23432e-07" },
        { 1.234321234321234e-6L, L"1.23432e-06" },
        { 1.234321234321234e-5L, L"1.23432e-05" },
        { 1.234321234321234e-4L, L"0.000123432" },
        { 1.234321234321234e-3L, L"0.00123432" },
        { 1.234321234321234e-2L, L"0.0123432" },
        { 1.234321234321234e-1L, L"0.123432" },
        { 1.234321234321234L, L"1.23432" },
        { 1.234321234321234e1L, L"12.3432" },
        { 1.234321234321234e2L, L"123.432" },
        { 1.234321234321234e3L, L"1234.32" },
        { 1.234321234321234e4L, L"12343.2" },
        { 1.234321234321234e5L, L"123432" },
        { 1.234321234321234e6L, L"1.23432e+06" },
        { 1.234321234321234e7L, L"1.23432e+07" },
        { 1.234321234321234e8L, L"1.23432e+08" },
        { 1.234321234321234e9L, L"1.23432e+09" },
        { 1.234321234321234e10L, L"1.23432e+10" },
        { 1.234321234321234e11L, L"1.23432e+11" },
        { 1.234321234321234e12L, L"1.23432e+12" },
        { 1.234321234321234e13L, L"1.23432e+13" },
        { 1.234321234321234e14L, L"1.23432e+14" },
        { 1.234321234321234e15L, L"1.23432e+15" },
        { 1.234321234321234e16L, L"1.23432e+16" },
        { 1.234321234321234e17L, L"1.23432e+17" },
        { 1.234321234321234e18L, L"1.23432e+18" },
        { 1.234321234321234e19L, L"1.23432e+19" },
        { 1.234321234321234e20L, L"1.23432e+20" },
        { 1.234321234321234e21L, L"1.23432e+21" },
        { 1.234321234321234e22L, L"1.23432e+22" },
        { 1.234321234321234e23L, L"1.23432e+23" },
        { 1.234321234321234e24L, L"1.23432e+24" },
        { 1.234321234321234e25L, L"1.23432e+25" },
        { 1.234321234321234e26L, L"1.23432e+26" },
        { 1.234321234321234e27L, L"1.23432e+27" },
        { 1.234321234321234e28L, L"1.23432e+28" },
        { 1.234321234321234e29L, L"1.23432e+29" },
        { 1.234321234321234e30L, L"1.23432e+30" },
        { 1.234321234321234e31L, L"1.23432e+31" },
        { 1.234321234321234e32L, L"1.23432e+32" },
        { 1.234321234321234e33L, L"1.23432e+33" },
        { 1.234321234321234e34L, L"1.23432e+34" },
        { 1.234321234321234e35L, L"1.23432e+35" },
        { 1.234321234321234e36L, L"1.23432e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
        size_t length;
        wchar_t *result =
          my_asnwprintf (NULL, &length, L"%Lg", data[k].value);
        const wchar_t *expected = data[k].string;
        ASSERT (result != NULL);
        ASSERT (wcscmp (result, expected) == 0
                /* Some implementations produce exponents with 3 digits.  */
                || (expected[wcslen (expected) - 4] == 'e'
                    && wcslen (result) == wcslen (expected) + 1
                    && wmemcmp (result, expected, wcslen (expected) - 2) == 0
                    && result[wcslen (expected) - 2] == '0'
                    && wcscmp (result + wcslen (expected) - 1,
                               expected + wcslen (expected) - 2)
                       == 0));
        ASSERT (length == wcslen (result));
        free (result);
      }
  }

  { /* A negative number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", -0.03125L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-0.03125 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", minus_zerol, 33, 44, 55);
    ASSERT (result != NULL);
    if (have_minus_zero ())
      ASSERT (wcscmp (result, L"-0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"inf 33") == 0
            || wcscmp (result, L"infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"-inf 33") == 0
            || wcscmp (result, L"-infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#if CHECK_PRINTF_SAFE && ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) >= 3 + 3
            && wcsisnan (result, 0, wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
  /* asnwprintf should print something for noncanonical values.  */
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Lg %d", x.value, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (length == wcslen (result));
    ASSERT (3 < length && wcscmp (result + length - 3, L" 33") == 0);
    free (result);
  }
#endif

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Lg %d", 10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*Lg %d", -10, 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%+Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"+1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"% Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L" 1.75 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.75000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.Lg %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"2. 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.Lg %d", 9.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1.e+01 33") == 0
            || wcscmp (result, L"1.e+001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%010Lg %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0000001234 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015Lg %d", - Infinityl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"           -inf 33") == 0
            || wcscmp (result, L"      -infinity 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%050Lg %d", NaNl (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcslen (result) == 50 + 3
            && wcsisnan (result, wcsspn (result, L" "), wcslen (result) - 3, 0)
            && wcscmp (result + wcslen (result) - 3, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.Lg %d", 1234.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1e+03 33") == 0
            || wcscmp (result, L"1e+003 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with no rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.5Lg %d", 999.951L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"999.95 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision with rounding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.5Lg %d", 999.996L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1000 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %n format directive.  */

  {
    int count = -1;
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%d %n", 123, &count, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"123 ") == 0);
    ASSERT (length == wcslen (result));
    ASSERT (count == 4);
    free (result);
  }

  /* Test the support of the POSIX/XSI format strings with positions.  */

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%2$d %1$d", 33, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"55 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the grouping flag.  */

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%'d %d", 1234567, 99);
    ASSERT (result != NULL);
    ASSERT (result[wcslen (result) - 1] == '9');
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the left-adjust flag.  */

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"a%*sc", -3, "b");
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"ab  c") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"a%-*sc", 3, "b");
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"ab  c") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"a%-*sc", -3, "b");
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"ab  c") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of large precision.  */

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000d %d", 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[i] == '0');
    ASSERT (wcscmp (result + 4000 - 7, L"1234567 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.*d %d", 4000, 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[i] == '0');
    ASSERT (wcscmp (result + 4000 - 7, L"1234567 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000d %d", -1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    ASSERT (result[0] == '-');
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[1 + i] == '0');
    ASSERT (wcscmp (result + 1 + 4000 - 7, L"1234567 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000u %d", 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[i] == '0');
    ASSERT (wcscmp (result + 4000 - 7, L"1234567 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000o %d", 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[i] == '0');
    ASSERT (wcscmp (result + 4000 - 7, L"4553207 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000x %d", 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    for (i = 0; i < 4000 - 6; i++)
      ASSERT (result[i] == '0');
    ASSERT (wcscmp (result + 4000 - 6, L"12d687 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.4000x %d", 1234567, 99);
    size_t i;
    ASSERT (result != NULL);
    ASSERT (result[0] == '0');
    ASSERT (result[1] == 'x');
    for (i = 0; i < 4000 - 6; i++)
      ASSERT (result[2 + i] == '0');
    ASSERT (wcscmp (result + 2 + 4000 - 6, L"12d687 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.4000f %d", 1.0, 99);
    size_t i;
    ASSERT (result != NULL);
    ASSERT (result[0] == '1');
    ASSERT (result[1] == '.');
    for (i = 0; i < 4000; i++)
      ASSERT (result[2 + i] == '0');
    ASSERT (wcscmp (result + 2 + 4000, L" 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.511f %d", 1.0, 99);
    size_t i;
    ASSERT (result != NULL);
    ASSERT (result[0] == '1');
    ASSERT (result[1] == '.');
    for (i = 0; i < 511; i++)
      ASSERT (result[2 + i] == '0');
    ASSERT (wcscmp (result + 2 + 511, L" 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  {
    char input[5000];
    size_t length;
    wchar_t *result;
    wchar_t winput[5000];
    size_t i;

    for (i = 0; i < sizeof (input) - 1; i++)
      input[i] = 'a' + ((1000000 / (i + 1)) % 26);
    input[i] = '\0';
    result = my_asnwprintf (NULL, &length, L"%.4000s %d", input, 99);
    ASSERT (result != NULL);
    for (i = 0; i < sizeof (input); i++)
      winput[i] = (wchar_t) input[i];
    ASSERT (wmemcmp (result, winput, 4000) == 0);
    ASSERT (wcscmp (result + 4000, L" 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the %s format directive.  */

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10s %d", "xyz", 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"       xyz 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*s %d", 10, "xyz", 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"       xyz 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*s %d", -10, "xyz", 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"xyz        33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10s %d", "xyz", 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"xyz        33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

#if HAVE_WCHAR_T
  static wchar_t L_xyz[4] = { 'x', 'y', 'z', 0 };

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10ls %d", L_xyz, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"       xyz 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*ls %d", 10, L_xyz, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"       xyz 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*ls %d", -10, L_xyz, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"xyz        33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10ls %d", L_xyz, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"xyz        33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#endif

  /* To verify that these tests succeed, it is necessary to run them under
     a tool that checks against invalid memory accesses, such as ElectricFence
     or "valgrind --tool=memcheck".  */
  {
    size_t i;

    for (i = 1; i <= 8; i++)
      {
        char *block;
        size_t length;
        wchar_t *result;
        wchar_t *wblock;
        size_t j;

        block = (char *) malloc (i);
        memcpy (block, "abcdefgh", i);
        result = my_asnwprintf (NULL, &length, L"%.*s", (int) i, block);
        ASSERT (result != NULL);
        wblock = (wchar_t *) malloc (i * sizeof (wchar_t));
        for (j = 0; j < i; j++)
          wblock[j] = (wchar_t) block[j];
        ASSERT (wmemcmp (result, wblock, i) == 0);
        ASSERT (result[i] == '\0');
        ASSERT (length == wcslen (result));
        free (result);
        free (block);
      }
  }
#if HAVE_WCHAR_T
  {
    size_t i;

    for (i = 1; i <= 8; i++)
      {
        wchar_t *block;
        size_t j;
        size_t length;
        wchar_t *result;

        block = (wchar_t *) malloc (i * sizeof (wchar_t));
        for (j = 0; j < i; j++)
          block[j] = "abcdefgh"[j];
        result = my_asnwprintf (NULL, &length, L"%.*ls", (int) i, block);
        ASSERT (result != NULL);
        ASSERT (wmemcmp (result, L"abcdefgh", i) == 0);
        ASSERT (result[i] == '\0');
        ASSERT (length == wcslen (result));
        free (result);
        free (block);
      }
  }
#endif

#if HAVE_WCHAR_T
  /* Test that converting an invalid wchar_t[] to char[] fails with EILSEQ.  */
  {
    static const wchar_t input[] = { (wchar_t) 1702057263, 114, 0 };
    size_t length;
    wchar_t *result = my_asnwprintf (NULL, &length, L"%ls %d", input, 99);
    if (result == NULL)
      ASSERT (errno == EILSEQ);
    else
      free (result);
  }
  {
    static const wchar_t input[] = { (wchar_t) 1702057263, 114, 0 };
    size_t length;
    wchar_t *result = my_asnwprintf (NULL, &length, L"%3ls %d", input, 99);
    if (result == NULL)
      ASSERT (errno == EILSEQ);
    else
      free (result);
  }
  {
    static const wchar_t input[] = { (wchar_t) 1702057263, 114, 0 };
    size_t length;
    wchar_t *result = my_asnwprintf (NULL, &length, L"%.1ls %d", input, 99);
    if (result == NULL)
      ASSERT (errno == EILSEQ);
    else
      free (result);
  }
  {
    static const wchar_t input[] = { (wchar_t) 1702057263, 114, 0 };
    size_t length;
    wchar_t *result = my_asnwprintf (NULL, &length, L"%3.1ls %d", input, 99);
    if (result == NULL)
      ASSERT (errno == EILSEQ);
    else
      free (result);
  }
#endif

  /* Test the support of the %c format directive.  */

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"%10c %d", (unsigned char) 'x', 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"         x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"%*c %d", 10, (unsigned char) 'x', 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"         x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"%*c %d", -10, (unsigned char) 'x', 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"x          33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"%-10c %d", (unsigned char) 'x', 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"x          33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision is ignored.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"%.0c %d", (unsigned char) 'x', 33, 44, 55);
    ASSERT (wcscmp (result, L"x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NUL character.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length,
                     L"a%cz %d", '\0', 33, 44, 55);
    ASSERT (wmemcmp (result, L"a\0z 33\0", 6 + 1) == 0);
    ASSERT (length == 6);
    free (result);
  }

#if HAVE_WCHAR_T
  static wint_t L_x = (wchar_t) 'x';

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10lc %d", L_x, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"         x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*lc %d", 10, L_x, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"         x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*lc %d", -10, L_x, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"x          33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10lc %d", L_x, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"x          33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision is ignored.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0lc %d", L_x, 33, 44, 55);
    ASSERT (wcscmp (result, L"x 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* NUL character.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"a%lcz %d", (wint_t) L'\0', 33, 44, 55);
    ASSERT (wmemcmp (result, L"a\0z 33\0", 6 + 1) == 0);
    ASSERT (length == 6);
    free (result);
  }
#endif

  /* Test the support of the 'x' conversion specifier for hexadecimal output of
     integers.  */

  { /* Zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%x %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A large positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%x %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"fffffffe 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*x %d", 10, 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*x %d", -10, 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"303c       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"000000303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0x %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%15.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"     000000303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%015.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when a width
       and a precision are both present.  But most implementations do so.  */
    #if MUSL_LIBC || defined __MINGW32__
    ASSERT (wcscmp (result, L"00000000000303c 33") == 0);
    #else
    ASSERT (wcscmp (result, L"     000000303c 33") == 0);
    #endif
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"303c       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#x %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"    0x303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x0000303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0x000000303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#15.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   0x000000303c 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#15.10x %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when a width
       and a precision are both present.  But most implementations do so.  */
    #if MUSL_LIBC || defined __MINGW32__
    ASSERT (wcscmp (result, L"0x000000000303c 33") == 0);
    #else
    ASSERT (wcscmp (result, L"   0x000000303c 33") == 0);
    #endif
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.0x %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Uppercase 'X'.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%X %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"303C 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Uppercase 'X' with FLAG_ALT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#X %d", 12348, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0X303C 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Uppercase 'X' with FLAG_ALT and zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.0X %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  /* Test the support of the 'b' conversion specifier for binary output of
     integers.  */

  { /* Zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A large positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11111111111111111111111111111110 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*b %d", 20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*b %d", -20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"     00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%025.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (wcscmp (result, L"     00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"    0b11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (wcscmp (result, L"   0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 2)) && !defined __UCLIBC__
  /* Test that the 'I' flag is supported.  */
  {
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%Id %d", 1234567, 99);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"1234567 99") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
#endif
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
