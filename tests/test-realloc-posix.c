/* Test of realloc function.
   Copyright (C) 2010-2024 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <errno.h>
#include <stdint.h>

#include "macros.h"

int
main (int argc, _GL_UNUSED char **argv)
{
  void *volatile p;

  /* Check that realloc (NULL, 0) is not a NULL pointer.  */
  p = realloc (NULL, 0);
  ASSERT (p != NULL);
#ifdef NOT_YET
  free (p);

  /* Check that realloc (p, 0) for non-NULL p is not a NULL pointer.  */
  p = malloc (200);
  p = realloc (p, 0);
  ASSERT (p != NULL);
  free (p);

  /* Check that realloc (p, 0) for non-NULL p is not a NULL pointer.  */
  p = malloc (2 * 1024 * 1024);
  p = realloc (p, 0);
  ASSERT (p != NULL);
#endif

  /* Check that realloc (p, n) fails when p is non-null and n exceeds
     PTRDIFF_MAX.  */
  if (PTRDIFF_MAX < SIZE_MAX)
    {
      size_t one = argc != 12345;
      void *volatile r = realloc (p, PTRDIFF_MAX + one);
      ASSERT (r == NULL);
      /* Avoid a test failure due to glibc bug
         <https://sourceware.org/bugzilla/show_bug.cgi?id=27870>.  */
      if (!getenv ("MALLOC_CHECK_"))
        ASSERT (errno == ENOMEM);
    }

  free (p);
  return test_exit_status;
}
