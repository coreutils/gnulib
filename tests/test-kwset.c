/* Test of searching for any of a given set of fixed strings in text.
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

/* Specification.  */
#include "kwset.h"

#include "macros.h"

int
main ()
{
  /* Trivial case.  */
  {
    kwset_t set = kwsalloc (NULL);
    kwsprep (set);

    ASSERT (kwswords (set) == 0);

    static const char input[] = "foo";
    struct kwsmatch match;
    ptrdiff_t ret = kwsexec (set, input, 3, &match, false);
    ASSERT (ret == -1);

    kwsfree (set);
  }

  /* Simple case: one fixed string.  */
  {
    kwset_t set = kwsalloc (NULL);
    kwsincr (set, "acab", 4);
    kwsprep (set);

    ASSERT (kwswords (set) == 1);

    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracab", 14, &match, false);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }
    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabra", 11, &match, false);
      ASSERT (ret == -1);
    }

    kwsfree (set);
  }

  /* Any match vs. longest left-most match.  */
  {
    kwset_t set = kwsalloc (NULL);
    kwsincr (set, "acab", 4);
    kwsincr (set, "acacia", 6);
    kwsprep (set);

    ASSERT (kwswords (set) == 2);

    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabatacaciaeinacabemia", 33, &match, false);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }
    /* longest = true makes no difference, because it's only about the left-most match.  */
    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabatacaciaeinacabemia", 33, &match, true);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }

    kwsfree (set);
  }

  /* Any match vs. longest left-most match.  */
  {
    kwset_t set = kwsalloc (NULL);
    kwsincr (set, "acab", 4);
    kwsincr (set, "acabe", 5);
    kwsprep (set);

    ASSERT (kwswords (set) == 2);

    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabatacaciaeinacabemia", 33, &match, false);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }
    /* longest = true makes no difference, because it's only about the left-most match.  */
    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabatacaciaeinacabemia", 33, &match, true);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }

    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabematacaciaeinacabia", 33, &match, false);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 0);
      ASSERT (match.size == 4);
    }
    /* Here, longest = true makes a difference.  */
    {
      struct kwsmatch match;
      ptrdiff_t ret = kwsexec (set, "abracadabracabematacaciaeinacabia", 33, &match, true);
      ASSERT (ret == 10);
      ASSERT (match.offset == ret);
      ASSERT (match.index == 1);
      ASSERT (match.size == 5);
    }

    kwsfree (set);
  }

  return test_exit_status;
}
