/* Test of mbs_endswith() function.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

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

  ASSERT (mbs_endswith ("", ""));
  ASSERT (mbs_endswith ("abc", ""));

  ASSERT (!mbs_endswith ("", "c"));
  ASSERT (!mbs_endswith ("x", "c"));
  ASSERT (mbs_endswith ("c", "c"));
  ASSERT (mbs_endswith ("abc", "c"));

  ASSERT (!mbs_endswith ("", "xyz"));
  ASSERT (!mbs_endswith ("x", "xyz"));
  ASSERT (!mbs_endswith ("a", "xyz"));
  ASSERT (!mbs_endswith ("abc", "xyz"));
  ASSERT (mbs_endswith ("xyz", "xyz"));
  ASSERT (mbs_endswith ("yxxyz", "xyz"));

  ASSERT (mbs_endswith ("", ""));
  ASSERT (mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "")); /* "äƀç" */

  ASSERT (!mbs_endswith ("", "\201\060\212\064")); /* "ç" */
  ASSERT (!mbs_endswith ("\201\065\374\063", "\201\060\212\064")); /* "ẋ" "ç" */
  ASSERT (mbs_endswith ("\201\060\212\064", "\201\060\212\064")); /* "ç" "ç" */
  ASSERT (mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\060\212\064")); /* "äƀç" "ç" */
  ASSERT (!mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "4")); /* "äƀç" */
  ASSERT (!mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "0\212\064")); /* "äƀç" "0?" */
  ASSERT (!mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "8\201\060\212\064")); /* "äƀç" "8ç" */

  ASSERT (!mbs_endswith ("", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋÿẑ" */
  ASSERT (!mbs_endswith ("\201\065\374\063", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋ" "ẋÿẑ" */
  ASSERT (!mbs_endswith ("\201\060\212\061", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ä" "ẋÿẑ" */
  ASSERT (!mbs_endswith ("\201\060\212\061\201\060\227\070\201\060\212\064", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "äƀç" "ẋÿẑ" */
  ASSERT (mbs_endswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ẋÿẑ" "ẋÿẑ" */
  ASSERT (mbs_endswith ("\201\060\213\067\201\065\374\063\201\065\374\063\201\060\213\067\201\065\374\071", "\201\065\374\063\201\060\213\067\201\065\374\071")); /* "ÿẋẋÿẑ" "ẋÿẑ" */
  ASSERT (!mbs_endswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "9")); /* "ẋÿẑ" */
  ASSERT (!mbs_endswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "5\374\071")); /* "ẋÿẑ" "5?" */
  ASSERT (!mbs_endswith ("\201\065\374\063\201\060\213\067\201\065\374\071", "7\201\065\374\071")); /* "ẋÿẑ" "7ẑ" */

  return test_exit_status;
}
