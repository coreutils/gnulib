/* Test of POSIX compatible fflush() function.
   Copyright (C) 2008 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdio.h>

#include <stdlib.h>

/* This test can only be made to work on specific platforms.  */
#if defined _IO_ferror_unlocked || defined __sferror /* GNU libc, BeOS; FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
# define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          abort ();							     \
        }								     \
    }									     \
  while (0)
#else
# define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          printf ("Skipping test: expected failure on this platform\n");     \
          exit (77);							     \
        }								     \
    }									     \
  while (0)
#endif

int
main (int argc, char **argv)
{
#if 0
  /* Check fflush after a backup ungetc() call.  This is case 1 in terms of
     <http://lists.gnu.org/archive/html/bug-gnulib/2008-03/msg00131.html>.
     The Austin Group has not yet decided how this should behave.  */
#endif
#if 0
  /* Check fflush after a non-backup ungetc() call.  This is case 2 in terms of
     <http://lists.gnu.org/archive/html/bug-gnulib/2008-03/msg00131.html>.
     The Austin Group has not yet decided how this should behave.  */
  /* Check that fflush after a non-backup ungetc() call discards the ungetc
     buffer.  This is mandated by POSIX
     <http://www.opengroup.org/susv3/functions/ungetc.html>:
       "The value of the file-position indicator for the stream after
        reading or discarding all pushed-back bytes shall be the same
        as it was before the bytes were pushed back."  */
  int c;

  c = fgetc (stdin);
  ASSERT (c == '#');

  c = fgetc (stdin);
  ASSERT (c == '!');

  /* Here the file-position indicator must be 2.  */

  c = ungetc ('@', stdin);
  ASSERT (c == '@');

  fflush (stdin);

  /* Here the file-position indicator must be 2 again.  */

  c = fgetc (stdin);
  ASSERT (c == '/');
#endif

  return 0;
}
