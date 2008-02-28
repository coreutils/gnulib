/* Test of freadptr() function.
   Copyright (C) 2007-2008 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#include "freadptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "freadahead.h"

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          abort ();							     \
        }								     \
    }									     \
  while (0)

int
main (int argc, char **argv)
{
  int nbytes = atoi (argv[1]);
  void *buf = malloc (nbytes);
  ASSERT (fread (buf, 1, nbytes, stdin) == nbytes);

  if (lseek (0, 0, SEEK_CUR) == nbytes)
    /* An unbuffered stdio, such as BeOS or on uClibc compiled without
       __STDIO_BUFFERS.  */
    ASSERT (freadahead (stdin) == 0);
  else
    {
      /* Normal buffered stdio.  */
      const char stdin_contents[] =
	"#!/bin/sh\n\n./test-freadptr${EXEEXT} 5 < \"$srcdir/test-freadptr.sh\" || exit 1\nexit 0\n";
      const char *expected = stdin_contents + nbytes;
      size_t available = freadahead (stdin);
      ASSERT (available != 0);
      ASSERT (available <= strlen (expected));
      ASSERT (memcmp (freadptr (stdin), expected, available) == 0);
    }

  return 0;
}
