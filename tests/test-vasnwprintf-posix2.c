/* Test of POSIX compatible vasnwprintf() and asnwprintf() functions.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

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

#include <float.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#if __GLIBC__ >= 2 || defined __CYGWIN__
# include <langinfo.h>
# include <stdint.h>
#endif

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

  /* Test that a locale dependent grouping separator is used correctly.  */
  {
    wchar_t separator[10];
#if __GLIBC__ >= 2 || defined __CYGWIN__
    /* On glibc, in the unibyte locale fr_FR, the *wprintf routines use U+202F
       as separator, which cannot be represented in the locale encoding.  */
    separator[0] = (wchar_t) (uintptr_t) nl_langinfo (_NL_NUMERIC_THOUSANDS_SEP_WC);
    separator[1] = L'\0';
#else
    const char *separator_mb =
      localeconv ()->thousands_sep; /* = nl_langinfo (THOUSEP) */
    mbstowcs (separator, separator_mb, SIZEOF (separator));
#endif
    size_t separator_len = wcslen (separator);

    /* Tests of %d directive with grouping separator.  */
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'d", 1000);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (wmemcmp (result, L"1", 1) == 0
              && wmemcmp (result + 1, separator, separator_len) == 0
              && wcscmp (result + 1 + separator_len, L"000") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'d", -1000);
      ASSERT (result != NULL);
      ASSERT (length == 5 + separator_len);
      ASSERT (wmemcmp (result, L"-1", 2) == 0
              && wmemcmp (result + 2, separator, separator_len) == 0
              && wcscmp (result + 2 + separator_len, L"000") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'d", -142857);
      ASSERT (result != NULL);
      ASSERT (length == 7 + separator_len);
      ASSERT (wmemcmp (result, L"-142", 4) == 0
              && wmemcmp (result + 4, separator, separator_len) == 0
              && wcscmp (result + 4 + separator_len, L"857") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'d", 2147483647);
      ASSERT (result != NULL);
      ASSERT (length == 10 + 3 * separator_len);
      ASSERT (wmemcmp (result, L"2", 1) == 0
              && wmemcmp (result + 1, separator, separator_len) == 0
              && wmemcmp (result + 1 + separator_len, L"147", 3) == 0
              && wmemcmp (result + 4 + separator_len, separator, separator_len) == 0
              && wmemcmp (result + 4 + 2 * separator_len, L"483", 3) == 0
              && wmemcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
              && wcscmp (result + 7 + 3 * separator_len, L"647") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'d", -2147483647);
      ASSERT (result != NULL);
      ASSERT (length == 11 + 3 * separator_len);
      ASSERT (wmemcmp (result, L"-2", 2) == 0
              && wmemcmp (result + 2, separator, separator_len) == 0
              && wmemcmp (result + 2 + separator_len, L"147", 3) == 0
              && wmemcmp (result + 5 + separator_len, separator, separator_len) == 0
              && wmemcmp (result + 5 + 2 * separator_len, L"483", 3) == 0
              && wmemcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && wcscmp (result + 8 + 3 * separator_len, L"647") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.10d", 1000);
      ASSERT (result != NULL);
      if (length == 10 + separator_len)
        {
          /* Most implementations don't add the grouping character within the
             leading zeroes.  */
          ASSERT (length == 10 + separator_len);
          ASSERT (wmemcmp (result, L"0000001", 7) == 0
                  && wmemcmp (result + 7, separator, separator_len) == 0
                  && wcscmp (result + 7 + separator_len, L"000") == 0);
        }
      else
        {
          /* But there are good arguments for adding the grouping character
             within the leading zeroes too.
             <https://sourceware.org/PR23432>  */
          ASSERT (length == 10 + 3 * separator_len);
          ASSERT (wmemcmp (result, L"0", 1) == 0
                  && wmemcmp (result + 1, separator, separator_len) == 0
                  && wmemcmp (result + 1 + separator_len, L"000", 3) == 0
                  && wmemcmp (result + 4 + separator_len, separator, separator_len) == 0
                  && wmemcmp (result + 4 + 2 * separator_len, L"001", 3) == 0
                  && wmemcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
                  && wcscmp (result + 7 + 3 * separator_len, L"000") == 0);
        }
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.10d", -1000);
      ASSERT (result != NULL);
      if (length == 11 + separator_len)
        {
          /* Most implementations don't add the grouping character within the
             leading zeroes.  */
          ASSERT (length == 11 + separator_len);
          ASSERT (wmemcmp (result, L"-0000001", 8) == 0
                  && wmemcmp (result + 8, separator, separator_len) == 0
                  && wcscmp (result + 8 + separator_len, L"000") == 0);
        }
      else
        {
          /* But there are good arguments for adding the grouping character
             within the leading zeroes too.
             <https://sourceware.org/PR23432>  */
          ASSERT (length == 11 + 3 * separator_len);
          ASSERT (wmemcmp (result, L"-0", 2) == 0
                  && wmemcmp (result + 2, separator, separator_len) == 0
                  && wmemcmp (result + 2 + separator_len, L"000", 3) == 0
                  && wmemcmp (result + 5 + separator_len, separator, separator_len) == 0
                  && wmemcmp (result + 5 + 2 * separator_len, L"001", 3) == 0
                  && wmemcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
                  && wcscmp (result + 8 + 3 * separator_len, L"000") == 0);
        }
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'10d", 1000);
      ASSERT (result != NULL);
      ASSERT (length == 10);
      ASSERT (wmemcmp (result, L"      1" + separator_len, 7 - separator_len) == 0
              && wmemcmp (result + 7 - separator_len, separator, separator_len) == 0
              && wcscmp (result + 7, L"000") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'10d", -1000);
      ASSERT (result != NULL);
      ASSERT (length == 10);
      ASSERT (wmemcmp (result, L"     -1" + separator_len, 7 - separator_len) == 0
              && wmemcmp (result + 7 - separator_len, separator, separator_len) == 0
              && wcscmp (result + 7, L"000") == 0);
      free (result);
    }

    /* Tests of %f directive with grouping separator.  */
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.0f", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (wmemcmp (result, L"1", 1) == 0
              && wmemcmp (result + 1, separator, separator_len) == 0
              && wcscmp (result + 1 + separator_len, L"000") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.5f", 3141592653.5897932386);
      ASSERT (result != NULL);
      ASSERT (length == 16 + 3 * separator_len);
      ASSERT (wmemcmp (result, L"3", 1) == 0
              && wmemcmp (result + 1, separator, separator_len) == 0
              && wmemcmp (result + 1 + separator_len, L"141", 3) == 0
              && wmemcmp (result + 4 + separator_len, separator, separator_len) == 0
              && wmemcmp (result + 4 + 2 * separator_len, L"592", 3) == 0
              && wmemcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
              && wcscmp (result + 7 + 3 * separator_len, L"653,58979") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.5f", -3141592653.5897932386);
      ASSERT (result != NULL);
      ASSERT (length == 17 + 3 * separator_len);
      ASSERT (wmemcmp (result, L"-3", 2) == 0
              && wmemcmp (result + 2, separator, separator_len) == 0
              && wmemcmp (result + 2 + separator_len, L"141", 3) == 0
              && wmemcmp (result + 5 + separator_len, separator, separator_len) == 0
              && wmemcmp (result + 5 + 2 * separator_len, L"592", 3) == 0
              && wmemcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && wcscmp (result + 8 + 3 * separator_len, L"653,58979") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'010.0f", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 10);
      ASSERT (wmemcmp (result, L"0000001" + separator_len, 7 - separator_len) == 0
              && wmemcmp (result + 7 - separator_len, separator, separator_len) == 0
              && wcscmp (result + 7, L"000") == 0);
      free (result);
    }

    /* Tests of %g directive with grouping separator.  */
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'g", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (wmemcmp (result, L"1", 1) == 0
              && wmemcmp (result + 1, separator, separator_len) == 0
              && wcscmp (result + 1 + separator_len, L"000") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'g", -142857.0);
      ASSERT (result != NULL);
      ASSERT (length == 7 + separator_len);
      ASSERT (wmemcmp (result, L"-142", 4) == 0
              && wmemcmp (result + 4, separator, separator_len) == 0
              && wcscmp (result + 4 + separator_len, L"857") == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.5g", 3141592653.5897932386);
      ASSERT (result != NULL);
      #if defined _WIN32 && !defined __CYGWIN__
        /* Native Windows uses 3 digits for the exponent.  */
        ASSERT (length == 11);
        ASSERT (wcscmp (result, L"3,1416e+009") == 0);
      #else
        ASSERT (length == 10);
        ASSERT (wcscmp (result, L"3,1416e+09") == 0);
      #endif
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.5g", -3141592653.5897932386);
      ASSERT (result != NULL);
      #if defined _WIN32 && !defined __CYGWIN__
        /* Native Windows uses 3 digits for the exponent.  */
        ASSERT (length == 12);
        ASSERT (wcscmp (result, L"-3,1416e+009") == 0);
      #else
        ASSERT (length == 11);
        ASSERT (wcscmp (result, L"-3,1416e+09") == 0);
      #endif
      free (result);
    }
    {
      size_t length;
      wchar_t *result =
        asnwprintf (NULL, &length, L"%'.50g",
                    42351647362715016953416125033982098102569580078125.0);
      ASSERT (result != NULL);
      ASSERT (length == 50 + 16 * separator_len);
      ASSERT (wmemcmp (result, L"42", 2) == 0
              && wmemcmp (result + 2, separator, separator_len) == 0
              && wmemcmp (result + 2 + separator_len, L"351", 3) == 0
              && wmemcmp (result + 5 + separator_len, separator, separator_len) == 0
              && wmemcmp (result + 5 + 2 * separator_len, L"647", 3) == 0
              && wmemcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && wmemcmp (result + 8 + 3 * separator_len, L"362", 3) == 0
              && wmemcmp (result + 11 + 3 * separator_len, separator, separator_len) == 0
              && wmemcmp (result + 11 + 4 * separator_len, L"715", 3) == 0
              && wmemcmp (result + 14 + 4 * separator_len, separator, separator_len) == 0);
      free (result);
    }
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'010g", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 10);
      ASSERT (wmemcmp (result, L"0000001" + separator_len, 7 - separator_len) == 0
              && wmemcmp (result + 7 - separator_len, separator, separator_len) == 0
              && wcscmp (result + 7, L"000") == 0);
      free (result);
    }
    #if LDBL_MAX_10_EXP > 500
    {
      size_t length;
      wchar_t *result = asnwprintf (NULL, &length, L"%'.500Lg\n", 42351647362715016953416125033982098102569580078125000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0L);
      free (result);
    }
    #endif
  }

  return test_exit_status;
}
