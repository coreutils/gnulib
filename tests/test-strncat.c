/* Test of strncat() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

#include <string.h>

#include "signature.h"
SIGNATURE_CHECK (strncat, char *, (char *, const char *, size_t));

#include <stdlib.h>

#include "zerosize-ptr.h"
#include "macros.h"

/* Test the library, not the compiler+library.  */
static char *
lib_strncat (char *s1, char const *s2, size_t n)
{
  return strncat (s1, s2, n);
}
static char *(*volatile volatile_strncat) (char *, char const *, size_t)
  = lib_strncat;
#undef strncat
#define strncat volatile_strncat

#define UNIT char
#define U_STRNCAT strncat
#define MAGIC ((char) 0xBA)
#include "unistr/test-strncat.h"

int
main ()
{
  /* Simple string.  */
  { /* "Grüß Gott. Здравствуйте! x=(-b±sqrt(b²-4ac))/(2a)  日本語,中文,한글"
       in UTF-8 encoding.  */
    static const char input[] =
      { 'G', 'r', (char) 0xC3, (char) 0xBC, (char) 0xC3, (char) 0x9F, ' ',
        'G', 'o', 't', 't', '.', ' ', (char) 0xD0, (char) 0x97, (char) 0xD0,
        (char) 0xB4, (char) 0xD1, (char) 0x80, (char) 0xD0, (char) 0xB0,
        (char) 0xD0, (char) 0xB2, (char) 0xD1, (char) 0x81, (char) 0xD1,
        (char) 0x82, (char) 0xD0, (char) 0xB2, (char) 0xD1, (char) 0x83,
        (char) 0xD0, (char) 0xB9, (char) 0xD1, (char) 0x82, (char) 0xD0,
        (char) 0xB5, '!', ' ', 'x', '=', '(', '-', 'b', (char) 0xC2,
        (char) 0xB1, 's', 'q', 'r', 't', '(', 'b', (char) 0xC2, (char) 0xB2,
        '-', '4', 'a', 'c', ')', ')', '/', '(', '2', 'a', ')', ' ', ' ',
        (char) 0xE6, (char) 0x97, (char) 0xA5, (char) 0xE6, (char) 0x9C,
        (char) 0xAC, (char) 0xE8, (char) 0xAA, (char) 0x9E, ',', (char) 0xE4,
        (char) 0xB8, (char) 0xAD, (char) 0xE6, (char) 0x96, (char) 0x87, ',',
        (char) 0xED, (char) 0x95, (char) 0x9C, (char) 0xEA, (char) 0xB8,
        (char) 0x80, '\0'
      };
    check (input, SIZEOF (input));
  }

  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */

#if 0 /* I think this is invalid, per ISO C 23 § 7.26.3.2.  */
  ASSERT (strncat (NULL, "x", 0) == NULL);
#endif

  {
    char y[2] = { 'x', '\0' };
    ASSERT (strncat (y, NULL, 0) == y);
  }

  return test_exit_status;
}
