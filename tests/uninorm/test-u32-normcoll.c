/* Test of locale dependent, normalization insensitive comparison of
   UTF-32 strings.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

#include "uninorm.h"

#include "macros.h"

#include "test-u32-normcmp.h"
int
main ()
{
  /* In the "C" locale, strcoll is equivalent to strcmp, therefore u8_normcoll
     on ASCII strings should behave like strcmp as well.  */
  test_ascii (u32_normcoll, UNINORM_NFC);

  return test_exit_status;
}
