/* Test of POSIX compatible vasnprintf() and asnprintf() functions.
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

#include "vasnprintf.h"

#include <float.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

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
    char *result = asnprintf (NULL, &length, "%.1a", 1.0);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1,0p+0") == 0
            || strcmp (result, "0x2,0p-1") == 0
            || strcmp (result, "0x4,0p-2") == 0
            || strcmp (result, "0x8,0p-3") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  /* Test that a locale dependent decimal point is used.  */
  {
    size_t length;
    char *result = asnprintf (NULL, &length, "%.1La", 1.0L);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1,0p+0") == 0
            || strcmp (result, "0x2,0p-1") == 0
            || strcmp (result, "0x4,0p-2") == 0
            || strcmp (result, "0x8,0p-3") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  /* Test that a locale dependent grouping separator is used correctly.  */
  {
    const char *separator =
      localeconv ()->thousands_sep; /* = nl_langinfo (THOUSEP) */
    size_t separator_len = strlen (separator);

    /* Tests of %d directive with grouping separator.  */
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'d", 1000);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (memcmp (result, "1", 1) == 0
              && memcmp (result + 1, separator, separator_len) == 0
              && strcmp (result + 1 + separator_len, "000") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'d", -1000);
      ASSERT (result != NULL);
      ASSERT (length == 5 + separator_len);
      ASSERT (memcmp (result, "-1", 2) == 0
              && memcmp (result + 2, separator, separator_len) == 0
              && strcmp (result + 2 + separator_len, "000") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'d", -142857);
      ASSERT (result != NULL);
      ASSERT (length == 7 + separator_len);
      ASSERT (memcmp (result, "-142", 4) == 0
              && memcmp (result + 4, separator, separator_len) == 0
              && strcmp (result + 4 + separator_len, "857") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'d", 2147483647);
      ASSERT (result != NULL);
      ASSERT (length == 10 + 3 * separator_len);
      ASSERT (memcmp (result, "2", 1) == 0
              && memcmp (result + 1, separator, separator_len) == 0
              && memcmp (result + 1 + separator_len, "147", 3) == 0
              && memcmp (result + 4 + separator_len, separator, separator_len) == 0
              && memcmp (result + 4 + 2 * separator_len, "483", 3) == 0
              && memcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
              && strcmp (result + 7 + 3 * separator_len, "647") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'d", -2147483647);
      ASSERT (result != NULL);
      ASSERT (length == 11 + 3 * separator_len);
      ASSERT (memcmp (result, "-2", 2) == 0
              && memcmp (result + 2, separator, separator_len) == 0
              && memcmp (result + 2 + separator_len, "147", 3) == 0
              && memcmp (result + 5 + separator_len, separator, separator_len) == 0
              && memcmp (result + 5 + 2 * separator_len, "483", 3) == 0
              && memcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && strcmp (result + 8 + 3 * separator_len, "647") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.10d", 1000);
      ASSERT (result != NULL);
      if (length == 10 + separator_len)
        {
          /* Most implementations don't add the grouping character within the
             leading zeroes.  */
          ASSERT (length == 10 + separator_len);
          ASSERT (memcmp (result, "0000001", 7) == 0
                  && memcmp (result + 7, separator, separator_len) == 0
                  && strcmp (result + 7 + separator_len, "000") == 0);
        }
      else
        {
          /* But there are good arguments for adding the grouping character
             within the leading zeroes.
             <https://sourceware.org/PR23432>  */
          ASSERT (length == 10 + 3 * separator_len);
          ASSERT (memcmp (result, "0", 1) == 0
                  && memcmp (result + 1, separator, separator_len) == 0
                  && memcmp (result + 1 + separator_len, "000", 3) == 0
                  && memcmp (result + 4 + separator_len, separator, separator_len) == 0
                  && memcmp (result + 4 + 2 * separator_len, "001", 3) == 0
                  && memcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
                  && strcmp (result + 7 + 3 * separator_len, "000") == 0);
        }
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.10d", -1000);
      ASSERT (result != NULL);
      if (length == 11 + separator_len)
        {
          /* Most implementations don't add the grouping character within the
             leading zeroes.  */
          ASSERT (length == 11 + separator_len);
          ASSERT (memcmp (result, "-0000001", 8) == 0
                  && memcmp (result + 8, separator, separator_len) == 0
                  && strcmp (result + 8 + separator_len, "000") == 0);
        }
      else
        {
          /* But there are good arguments for adding the grouping character
             within the leading zeroes.
             <https://sourceware.org/PR23432>  */
          ASSERT (length == 11 + 3 * separator_len);
          ASSERT (memcmp (result, "-0", 2) == 0
                  && memcmp (result + 2, separator, separator_len) == 0
                  && memcmp (result + 2 + separator_len, "000", 3) == 0
                  && memcmp (result + 5 + separator_len, separator, separator_len) == 0
                  && memcmp (result + 5 + 2 * separator_len, "001", 3) == 0
                  && memcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
                  && strcmp (result + 8 + 3 * separator_len, "000") == 0);
        }
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'10d", 1000);
      ASSERT (result != NULL);
      if (separator_len > 0)
        {
          if (length == 10)
            {
              /* Result according to POSIX, which says that the width is
                 measured in "bytes".
                 <https://pubs.opengroup.org/onlinepubs/9799919799/functions/fprintf.html>
                 <https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap05.html#tag_05>  */
              ASSERT (length == 10);
              ASSERT (memcmp (result, "      1" + separator_len, 7 - separator_len) == 0
                      && memcmp (result + 7 - separator_len, separator, separator_len) == 0
                      && strcmp (result + 7, "000") == 0);
            }
          else
            {
              /* Result according to glibc, which measures the width in
                 multibyte characters or column positions.
                 <https://sourceware.org/PR28943>
                 <https://sourceware.org/PR30883>
                 <https://sourceware.org/PR31542>  */
              ASSERT (length == 9 + separator_len);
              ASSERT (memcmp (result, "     1", 6) == 0
                      && memcmp (result + 6, separator, separator_len) == 0
                      && strcmp (result + 6 + separator_len, "000") == 0);
            }
        }
      else
        {
          ASSERT (length == 10);
          ASSERT (strcmp (result, "      1000") == 0);
        }
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'10d", -1000);
      ASSERT (result != NULL);
      if (separator_len > 0)
        {
          if (length == 10)
            {
              /* Result according to POSIX, which says that the width is
                 measured in "bytes".
                 <https://pubs.opengroup.org/onlinepubs/9799919799/functions/fprintf.html>
                 <https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap05.html#tag_05>  */
              ASSERT (length == 10);
              ASSERT (memcmp (result, "     -1" + separator_len, 7 - separator_len) == 0
                      && memcmp (result + 7 - separator_len, separator, separator_len) == 0
                      && strcmp (result + 7, "000") == 0);
            }
          else
            {
              /* Result according to glibc, which measures the width in
                 multibyte characters or column positions.
                 <https://sourceware.org/PR28943>
                 <https://sourceware.org/PR30883>
                 <https://sourceware.org/PR31542>  */
              ASSERT (length == 9 + separator_len);
              ASSERT (memcmp (result, "    -1", 6) == 0
                      && memcmp (result + 6, separator, separator_len) == 0
                      && strcmp (result + 6 + separator_len, "000") == 0);
            }
        }
      else
        {
          ASSERT (length == 10);
          ASSERT (strcmp (result, "     -1000") == 0);
        }
      free (result);
    }

    /* Tests of %f directive with grouping separator.  */
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.0f", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (memcmp (result, "1", 1) == 0
              && memcmp (result + 1, separator, separator_len) == 0
              && strcmp (result + 1 + separator_len, "000") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.5f", 3141592653.5897932386);
      ASSERT (result != NULL);
      ASSERT (length == 16 + 3 * separator_len);
      ASSERT (memcmp (result, "3", 1) == 0
              && memcmp (result + 1, separator, separator_len) == 0
              && memcmp (result + 1 + separator_len, "141", 3) == 0
              && memcmp (result + 4 + separator_len, separator, separator_len) == 0
              && memcmp (result + 4 + 2 * separator_len, "592", 3) == 0
              && memcmp (result + 7 + 2 * separator_len, separator, separator_len) == 0
              && strcmp (result + 7 + 3 * separator_len, "653,58979") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.5f", -3141592653.5897932386);
      ASSERT (result != NULL);
      ASSERT (length == 17 + 3 * separator_len);
      ASSERT (memcmp (result, "-3", 2) == 0
              && memcmp (result + 2, separator, separator_len) == 0
              && memcmp (result + 2 + separator_len, "141", 3) == 0
              && memcmp (result + 5 + separator_len, separator, separator_len) == 0
              && memcmp (result + 5 + 2 * separator_len, "592", 3) == 0
              && memcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && strcmp (result + 8 + 3 * separator_len, "653,58979") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'010.0f", 1000.0);
      ASSERT (result != NULL);
      if (separator_len > 0)
        {
          if (length == 10)
            {
              /* Result according to POSIX, which says that the width is
                 measured in "bytes".
                 <https://pubs.opengroup.org/onlinepubs/9799919799/functions/fprintf.html>
                 <https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap05.html#tag_05>  */
              ASSERT (length == 10);
              ASSERT (memcmp (result, "0000001" + separator_len, 7 - separator_len) == 0
                      && memcmp (result + 7 - separator_len, separator, separator_len) == 0
                      && strcmp (result + 7, "000") == 0);
            }
          else
            {
              /* Result according to glibc, which measures the width in
                 multibyte characters or column positions.
                 <https://sourceware.org/PR28943>
                 <https://sourceware.org/PR30883>
                 <https://sourceware.org/PR31542>  */
              ASSERT (length == 9 + separator_len);
              ASSERT (memcmp (result, "000001", 6) == 0
                      && memcmp (result + 6, separator, separator_len) == 0
                      && strcmp (result + 6 + separator_len, "000") == 0);
            }
        }
      else
        {
          ASSERT (length == 10);
          ASSERT (strcmp (result, "0000001000") == 0);
        }
      free (result);
    }

    /* Tests of %g directive with grouping separator.  */
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'g", 1000.0);
      ASSERT (result != NULL);
      ASSERT (length == 4 + separator_len);
      ASSERT (memcmp (result, "1", 1) == 0
              && memcmp (result + 1, separator, separator_len) == 0
              && strcmp (result + 1 + separator_len, "000") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'g", -142857.0);
      ASSERT (result != NULL);
      ASSERT (length == 7 + separator_len);
      ASSERT (memcmp (result, "-142", 4) == 0
              && memcmp (result + 4, separator, separator_len) == 0
              && strcmp (result + 4 + separator_len, "857") == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.5g", 3141592653.5897932386);
      ASSERT (result != NULL);
      #if defined _WIN32 && !defined __CYGWIN__
        /* Native Windows uses 3 digits for the exponent.  */
        ASSERT (length == 11);
        ASSERT (strcmp (result, "3,1416e+009") == 0);
      #else
        ASSERT (length == 10);
        ASSERT (strcmp (result, "3,1416e+09") == 0);
      #endif
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.5g", -3141592653.5897932386);
      ASSERT (result != NULL);
      #if defined _WIN32 && !defined __CYGWIN__
        /* Native Windows uses 3 digits for the exponent.  */
        ASSERT (length == 12);
        ASSERT (strcmp (result, "-3,1416e+009") == 0);
      #else
        ASSERT (length == 11);
        ASSERT (strcmp (result, "-3,1416e+09") == 0);
      #endif
      free (result);
    }
    {
      size_t length;
      char *result =
        asnprintf (NULL, &length, "%'.50g",
                   42351647362715016953416125033982098102569580078125.0);
      ASSERT (result != NULL);
      ASSERT (length == 50 + 16 * separator_len);
      ASSERT (memcmp (result, "42", 2) == 0
              && memcmp (result + 2, separator, separator_len) == 0
              && memcmp (result + 2 + separator_len, "351", 3) == 0
              && memcmp (result + 5 + separator_len, separator, separator_len) == 0
              && memcmp (result + 5 + 2 * separator_len, "647", 3) == 0
              && memcmp (result + 8 + 2 * separator_len, separator, separator_len) == 0
              && memcmp (result + 8 + 3 * separator_len, "362", 3) == 0
              && memcmp (result + 11 + 3 * separator_len, separator, separator_len) == 0
              && memcmp (result + 11 + 4 * separator_len, "715", 3) == 0
              && memcmp (result + 14 + 4 * separator_len, separator, separator_len) == 0);
      free (result);
    }
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'010g", 1000.0);
      ASSERT (result != NULL);
      if (separator_len > 0)
        {
          if (length == 10)
            {
              /* Result according to POSIX, which says that the width is
                 measured in "bytes".
                 <https://pubs.opengroup.org/onlinepubs/9799919799/functions/fprintf.html>
                 <https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap05.html#tag_05>  */
              ASSERT (length == 10);
              ASSERT (memcmp (result, "0000001" + separator_len, 7 - separator_len) == 0
                      && memcmp (result + 7 - separator_len, separator, separator_len) == 0
                      && strcmp (result + 7, "000") == 0);
            }
          else
            {
              /* Result according to glibc, which measures the width in
                 multibyte characters or column positions.
                 <https://sourceware.org/PR28943>
                 <https://sourceware.org/PR30883>
                 <https://sourceware.org/PR31542>  */
              ASSERT (length == 9 + separator_len);
              ASSERT (memcmp (result, "000001", 6) == 0
                      && memcmp (result + 6, separator, separator_len) == 0
                      && strcmp (result + 6 + separator_len, "000") == 0);
            }
        }
      else
        {
          ASSERT (length == 10);
          ASSERT (strcmp (result, "0000001000") == 0);
        }
      free (result);
    }
    #if LDBL_MAX_10_EXP > 500
    /* This test used to crash in the Solaris libc, for all Solaris versions.
       <https://www.illumos.org/issues/17383>  */
    {
      size_t length;
      char *result = asnprintf (NULL, &length, "%'.500Lg\n", 42351647362715016953416125033982098102569580078125000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0L);
      free (result);
    }
    #endif
  }

  return test_exit_status;
}
