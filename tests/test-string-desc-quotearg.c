/* Test of string descriptors.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include "string-desc-quotearg.h"

#include <stdlib.h>
#include <string.h>

#include "macros.h"

int
main (void)
{
  string_desc_t s1 = sd_from_c ("Hello world!");
  string_desc_t s2 = sd_new_addr (21, "The\0quick\0brown\0\0fox");

  /* Test sd_quotearg_buffer.  */
  {
    char buf[80];
    size_t n = sd_quotearg_buffer (buf, sizeof (buf), s2, NULL);
    ASSERT (n == 21);
    ASSERT (memcmp (buf, "The\0quick\0brown\0\0fox", n) == 0);
  }

  /* Test sd_quotearg_alloc.  */
  {
    size_t n;
    char *ret = sd_quotearg_alloc (s2, &n, NULL);
    ASSERT (n == 21);
    ASSERT (memcmp (ret, "The\0quick\0brown\0\0fox", n) == 0);
    free (ret);
  }

  /* Test sd_quotearg_n.  */
  {
    char *ret = sd_quotearg_n (1, s2);
    ASSERT (memcmp (ret, "Thequickbrownfox", 16 + 1) == 0);
  }

  /* Test sd_quotearg.  */
  {
    char *ret = sd_quotearg (s2);
    ASSERT (memcmp (ret, "Thequickbrownfox", 16 + 1) == 0);
  }

  /* Test sd_quotearg_n_style.  */
  {
    char *ret = sd_quotearg_n_style (1, clocale_quoting_style, s2);
    ASSERT (memcmp (ret, "\"The\\0quick\\0brown\\0\\0fox\\0\"", 28 + 1) == 0
            || /* if the locale has UTF-8 encoding */
               memcmp (ret, "\342\200\230The\\0quick\\0brown\\0\\0fox\\0\342\200\231", 32 + 1) == 0);
  }

  /* Test sd_quotearg_style.  */
  {
    char *ret = sd_quotearg_style (clocale_quoting_style, s2);
    ASSERT (memcmp (ret, "\"The\\0quick\\0brown\\0\\0fox\\0\"", 28 + 1) == 0
            || /* if the locale has UTF-8 encoding */
               memcmp (ret, "\342\200\230The\\0quick\\0brown\\0\\0fox\\0\342\200\231", 32 + 1) == 0);
  }

  /* Test sd_quotearg_char.  */
  {
    char *ret = sd_quotearg_char (s1, ' ');
    ASSERT (memcmp (ret, "Hello world!", 12 + 1) == 0); /* ' ' not quoted?! */
  }

  /* Test sd_quotearg_colon.  */
  {
    char *ret = sd_quotearg_colon (sd_from_c ("a:b"));
    ASSERT (memcmp (ret, "a:b", 3 + 1) == 0); /* ':' not quoted?! */
  }

  /* Test sd_quotearg_n_custom.  */
  {
    char *ret = sd_quotearg_n_custom (2, "<", ">", s1);
    ASSERT (memcmp (ret, "<Hello world!>", 14 + 1) == 0);
  }

  /* Test sd_quotearg_n_custom.  */
  {
    char *ret = sd_quotearg_custom ("[[", "]]", s1);
    ASSERT (memcmp (ret, "[[Hello world!]]", 16 + 1) == 0);
  }

  return test_exit_status;
}
