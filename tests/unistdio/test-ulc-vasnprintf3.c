/* Test of ulc_vasnprintf() function in an UTF-8 locale.
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

#include "unistdio.h"

#include <locale.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

static void
test_function (char * (*my_asnprintf) (char *, size_t *, const char *, ...))
{
  /* Test the support of the 'U' conversion specifier for Unicode strings.  */

  {
    static const uint8_t unicode_string[] = "Rafa\305\202 Maszkowski"; /* Rafał Maszkowski */
    {
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%U %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* Width.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%20U %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_LEFT.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%-20U %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski     33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_ZERO: no effect.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%020U %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
  }
  { /* Width with a non-BMP argument.  */
    static const uint8_t unicode_string[] = "\360\237\220\203"; /* 🐃 */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%10U %d", unicode_string, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "         \360\237\220\203 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  {
    static const uint16_t unicode_string[] = /* Rafał Maszkowski */
      {
        'R', 'a', 'f', 'a', 0x0142, ' ', 'M', 'a', 's', 'z', 'k', 'o', 'w',
        's', 'k', 'i', 0
      };
    {
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%lU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* Width.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%20lU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_LEFT.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%-20lU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski     33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_ZERO: no effect.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%020lU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
  }
  { /* Width with a non-BMP argument.  */
    static const uint16_t unicode_string[] = { 0xd83d, 0xdc03, 0 }; /* 🐃 */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%10lU %d", unicode_string, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "         \360\237\220\203 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  {
    static const uint32_t unicode_string[] = /* Rafał Maszkowski */
      {
        'R', 'a', 'f', 'a', 0x0142, ' ', 'M', 'a', 's', 'z', 'k', 'o', 'w',
        's', 'k', 'i', 0
      };
    {
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%llU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* Width.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%20llU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_LEFT.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%-20llU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "Rafa\305\202 Maszkowski     33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_ZERO: no effect.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%020llU %d", unicode_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "    Rafa\305\202 Maszkowski 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
  }
  { /* Width with a non-BMP argument.  */
    static const uint32_t unicode_string[] = { 0x1f403, 0 }; /* 🐃 */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%10llU %d", unicode_string, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "         \360\237\220\203 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  /* Test the support of the 's' conversion specifier for strings.  */

  {
    const char *locale_string = "\303\204rger"; /* Ärger */
    {
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%s %d", locale_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "\303\204rger 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* Width.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%10s %d", locale_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "     \303\204rger 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_LEFT.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%-10s %d", locale_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "\303\204rger      33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_ZERO: no effect.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%010s %d", locale_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "     \303\204rger 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
  }

  /* Test the support of the 'ls' conversion specifier for wide strings.  */

  {
    const char *locale_string = "h\303\251t\303\251rog\303\251n\303\251it\303\251"; /* hétérogénéité */
    wchar_t wide_string[20];
    ASSERT (mbstowcs (wide_string, locale_string, SIZEOF (wide_string)) == 13);
    {
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%ls %d", wide_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "h\303\251t\303\251rog\303\251n\303\251it\303\251 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* Width.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%20ls %d", wide_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "       h\303\251t\303\251rog\303\251n\303\251it\303\251 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_LEFT.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%-20ls %d", wide_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "h\303\251t\303\251rog\303\251n\303\251it\303\251        33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
    { /* FLAG_ZERO: no effect.  */
      size_t length;
      char *result =
        my_asnprintf (NULL, &length, "%020ls %d", wide_string, 33, 44, 55);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "       h\303\251t\303\251rog\303\251n\303\251it\303\251 33") == 0);
      ASSERT (length == strlen (result));
      free (result);
    }
  }
}

static char *
my_asnprintf (char *resultbuf, size_t *lengthp, const char *format, ...)
{
  va_list args;
  char *ret;

  va_start (args, format);
  ret = ulc_vasnprintf (resultbuf, lengthp, format, args);
  va_end (args);
  return ret;
}

static void
test_vasnprintf ()
{
  test_function (my_asnprintf);
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  test_vasnprintf ();
  return test_exit_status;
}
