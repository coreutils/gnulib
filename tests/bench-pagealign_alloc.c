/* Benchmarks for the pagealign_alloc module.
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

/* This benchmark focuses on the amount of allocated memory, in order
   to minimize wasted memory.  */

#include <config.h>

/* Specification.  */
#include "pagealign_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "resource-ext.h"

static size_t pagesize;

/* Does a few pagealign_alloc invocations.  */
static int
allocate_some_pages (void)
{
  int i;

  for (i = 1; i <= 9; i++)
    if (pagealign_alloc (pagesize) == NULL)
      return -1;
  for (i = 1; i <= 2; i++)
    if (pagealign_alloc (2 * pagesize) == NULL)
      return -1;
  for (i = 1; i <= 1; i++)
    if (pagealign_alloc (3 * pagesize) == NULL)
      return -1;

  return 0;
}

/* Returns the total size requested by allocate_some_pages ().  */
static size_t
size_of_some_pages (void)
{
  return 16 * pagesize;
}

static void
show_stats (intptr_t initial_as, intptr_t initial_data, int count)
{
  if (count == 0)
    printf ("Allocation failed.\n");
  else
    {
      intptr_t requested = count * size_of_some_pages ();
      intptr_t as_increase = get_rusage_as () - initial_as;
      intptr_t data_increase = get_rusage_data () - initial_data;
      printf ("count = %5d: requested: %6ld pages, "
              "allocated AS: %6ld pages (%.2f%%), "
              "allocated DATA: %6ld pages (%.2f%%)\n",
              count,
              (long int) (requested / pagesize),
              (long int) (as_increase / pagesize),
              100 * (double) as_increase / (double) requested,
              (long int) (data_increase / pagesize),
              100 * (double) data_increase / (double) requested);
    }
}

static void
do_test (void)
{
  intptr_t initial_as = get_rusage_as ();
  intptr_t initial_data = get_rusage_data ();
  int count;
  for (count = 0; count <= 10000; count++)
    {
      if (count == 10 || count == 30 || count == 90 || count == 270
          || count == 810 || count == 2430 || count == 7290)
        show_stats (initial_as, initial_data, count);

      if (allocate_some_pages () < 0)
        break;
    }
  show_stats (initial_as, initial_data, count);
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s TESTS\n", argv[0]);
      fprintf (stderr, "Example: %s {a|b|c|d|e|f}\n", argv[0]);
      exit (1);
    }

  const char *tests = argv[1];

  pagesize = getpagesize ();

  /* Execute each test.  */
  size_t i;
  for (i = 0; i < strlen (tests); i++)
    {
      char test = tests[i];

      switch (test)
        {
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          pagealign_impl = test - 'a';
          do_test ();
          break;
        default:
          /* Ignore.  */
          ;
        }
    }

  return 0;
}

/* Results:

                   b         c         d         e         f

glibc            176%      101%      175%       ---       ---
musl libc        198%      101%      197%       ---       ---
macOS            190%      100%      195%       ---       ---
FreeBSD          190%      100%      380%       ---       ---
NetBSD           185%      101%      379%       ---       ---
OpenBSD          177%      101%      101%       ---       ---
AIX              177%      101%      176%       ---       ---
Solaris 11.4     176%      101%      175%       ---       ---
Cygwin           181%      100%      181%       ---       ---
native Windows   184%       ---       ---      180%      100%
Android          182%      101%      181%       ---       ---
*/
