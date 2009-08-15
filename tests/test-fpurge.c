/* Test of fpurge() function.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

#define TESTFILE "t-fpurge.tmp"

int
main ()
{
  FILE *fp;

  /* Create a file with some contents.  */
  fp = fopen (TESTFILE, "w");
  if (fp == NULL)
    goto skip;
  if (fwrite ("foobarsh", 1, 8, fp) < 8)
    goto skip;
  if (fclose (fp))
    goto skip;

  /* Open it in read-write mode.  */
  fp = fopen (TESTFILE, "r+");
  if (fp == NULL)
    goto skip;
  if (fseek (fp, 3, SEEK_CUR))
    goto skip;
  if (fwrite ("g", 1, 1, fp) < 1)
    goto skip;
  if (fflush (fp))
    goto skip;
  if (fwrite ("bz", 1, 2, fp) < 2)
    goto skip;
  /* Discard pending write.  */
  ASSERT (fpurge (fp) == 0);
  ASSERT (fclose (fp) == 0);

  /* Open it in read-only mode.  */
  fp = fopen (TESTFILE, "r");
  if (fp == NULL)
    goto skip;
  {
    char buf[8];
    if (fread (buf, 1, 7, fp) < 7)
      goto skip;
    ASSERT (memcmp (buf, "foogars", 7) == 0);
  }
  /* Discard the buffered 'h', leaving position at EOF.  */
  ASSERT (fpurge (fp) == 0);
  ASSERT (getc (fp) == EOF);
  ASSERT (fclose (fp) == 0);

  /* Ensure that purging a read does not corrupt subsequent writes.  */
  fp = fopen (TESTFILE, "r+");
  ASSERT (fp);
  ASSERT (fseek (fp, -1, SEEK_END) == 0);
  ASSERT (getc (fp) == 'h');
  ASSERT (getc (fp) == EOF);
  ASSERT (fpurge (fp) == 0);
  ASSERT (putc ('!', fp) == '!');
  ASSERT (fclose (fp) == 0);
  fp = fopen (TESTFILE, "r");
  ASSERT (fp);
  {
    char buf[9];
    ASSERT (fread (buf, 1, 9, fp) == 9);
    ASSERT (memcmp (buf, "foogarsh!", 9) == 0);
  }

  remove (TESTFILE);
  return 0;

 skip:
  fprintf (stderr, "Skipping test: prerequisite file operations failed.\n");
  remove (TESTFILE);
  return 77;
}
