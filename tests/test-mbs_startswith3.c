/* Test of mbs_startswith() function in a GB18030 locale.
   Copyright (C) 2025-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

#include <string.h>

#include <locale.h>

#include "macros.h"

int
main ()
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  ASSERT (mbs_startswith ("", ""));
  ASSERT (mbs_startswith ("abc", ""));

  ASSERT (!mbs_startswith ("", "a"));
  ASSERT (!mbs_startswith ("x", "a"));
  ASSERT (mbs_startswith ("a", "a"));
  ASSERT (mbs_startswith ("abc", "a"));

  ASSERT (!mbs_startswith ("", "xyz"));
  ASSERT (!mbs_startswith ("x", "xyz"));
  ASSERT (!mbs_startswith ("a", "xyz"));
  ASSERT (!mbs_startswith ("abc", "xyz"));
  ASSERT (mbs_startswith ("xyz", "xyz"));
  ASSERT (mbs_startswith ("xyzzy", "xyz"));

  ASSERT (mbs_startswith ("", ""));
  ASSERT (mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "")); /* "äƀç" */

  ASSERT (!mbs_startswith ("", "\201\060\212\061")); /* "ä" */
  ASSERT (!mbs_startswith ("\201\065\374\063", "\201\060\212\061")); /* "ẋ" "ä" */
  ASSERT (mbs_startswith ("\201\060\212\061", "\201\060\212\061")); /* "ä" "ä" */
  ASSERT (mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\060\212\061")); /* "äƀç" "ä" */
  /* Test a prefix that ends in an incomplete character.  */
  ASSERT (!mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\060\212")); /* "äƀç" */
  ASSERT (!mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\060")); /* "äƀç" */
  ASSERT (!mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201")); /* "äƀç" */

  ASSERT (!mbs_startswith ("", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋÿẑ" */
  ASSERT (!mbs_startswith ("\201\065\374\063", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋ" "ẋÿẑ" */
  ASSERT (!mbs_startswith ("\201\060\212\061", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ä" "ẋÿẑ" */
  ASSERT (!mbs_startswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "äƀç" "ẋÿẑ" */
  ASSERT (mbs_startswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋÿẑ" "ẋÿẑ" */
  ASSERT (mbs_startswith ("\201\065\374\063\201\060\213\067\201\065\374\071\201\065\374\071\201\060\213\067", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋÿẑẑÿ" "ẋÿẑ" */
  /* Test a prefix that ends in an incomplete character.  */
  ASSERT (!mbs_startswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "\201\065\374")); /* "ẋÿẑ" */
  ASSERT (!mbs_startswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "\201\065")); /* "ẋÿẑ" */
  ASSERT (!mbs_startswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "\201")); /* "ẋÿẑ" */

  return test_exit_status;
}
