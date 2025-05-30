/* Tests of getgroups.
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

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#include <config.h>

#include <unistd.h>

#include "signature.h"
SIGNATURE_CHECK (getgroups, int, (int, gid_t[]));

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "macros.h"

/* Tell GCC not to warn about the specific edge cases tested here.
   GCC >= 10 with glibc >= 2.32 would otherwise trigger warnings, even without
   any -W options, because getgroups() is declared with
     __attribute__ ((__access__ (__write_only__, 2, 1)))
 */
#if _GL_GNUC_PREREQ (7, 0)
# pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

int
main (int argc, _GL_UNUSED char **argv)
{
  int result;
  gid_t *groups;

  errno = 0;
  result = getgroups (0, NULL);
  if (result == -1 && errno == ENOSYS)
    {
      fputs ("skipping test: no support for groups\n", stderr);
      return 77;
    }
  ASSERT (0 <= result);
  ASSERT (result + 1 < SIZE_MAX / sizeof *groups);
  groups = malloc ((result + 1) * sizeof *groups);
  ASSERT (groups);
  groups[result] = -1;
  /* Check for EINVAL handling.  Not all processes have supplemental
     groups, and getgroups does not have to return the effective gid,
     so a result of 0 is reasonable.  Also, we can't test for EINVAL
     if result is 1, because of how getgroups treats 0.  */
  if (1 < result)
    {
      errno = 0;
      ASSERT (getgroups (result - 1, groups) == -1);
      ASSERT (errno == EINVAL);
    }
  ASSERT (getgroups (result, groups) == result);
  ASSERT (getgroups (result + 1, groups) == result);
  ASSERT (groups[result] == -1);
  errno = 0;
  ASSERT (getgroups (-1, NULL) == -1);
  ASSERT (errno == EINVAL);

  /* The automated unit test, with no arguments, ends here.  However,
     for debugging purposes, you can pass a command-line argument to
     list the returned groups.  */
  if (1 < argc)
    {
      int i;
      for (i = 0; i < result; i++)
        printf ("%d\n", (int) groups[i]);
    }
  free (groups);
  return test_exit_status;
}
