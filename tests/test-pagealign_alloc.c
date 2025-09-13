/* Test pagealign_alloc module.
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
#include "pagealign_alloc.h"

/* This test verifies that allocating and then freeing N pages is O(N),
   not O(N²).  */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "xalloc.h"

int
main (int argc, char *argv[])
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.
     On a fast x86_64 machine in 2025, with the PA_IMPL_MALLOC implementation,
     the execution times are:
        n   |    time     |   time   |
            | linear list | hash set |
     -------+-------------+----------+
      10000 |   0.15 sec  | 0.03 sec |
      25000 |   0.8 sec   | 0.07 sec |
      50000 |   3.1 sec   | 0.13 sec |
     100000 |  14 sec     | 0.26 sec |
   */
  int alarm_value = 10;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  int n;
  if (argc > 1)
    n = atoi (argv[1]);
  else
    {
#if defined __CYGWIN__
      n = 25000;
#else
      n = (sizeof (void *) <= 4 ? 25000 : 50000);
#endif
    }

  void **pages = XNMALLOC (n, void *);

  size_t pagesize = getpagesize ();

  int i;
  for (i = 0; i < n; i++)
    pages[i] = pagealign_xalloc (pagesize);

  /* Free half of the pages in allocation order and the other half of the pages
     in reverse allocation order.  */
  int half = n / 2;
  for (i = 0; i < half; i++)
    pagealign_free (pages[i]);
  for (i = n - 1; i >= half; i--)
    pagealign_free (pages[i]);

  return 0;
}
