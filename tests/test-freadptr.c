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
       __STDIO_BUFFERS.  Or stdin is a pipe.  */
    ASSERT (freadahead (stdin) == 0);
  else
    {
      /* Normal buffered stdio.  */
      const char stdin_contents[] =
	"#!/bin/sh\n\n./test-freadptr${EXEEXT} 5 < \"$srcdir/test-freadptr.sh\" || exit 1\ncat \"$srcdir/test-freadptr.sh\" | ./test-freadptr${EXEEXT} 5 || exit 1\nexit 0\n";
      const char *expected = stdin_contents + nbytes;
      size_t available;
      size_t available2;
      size_t available3;

      /* Test normal behaviour.  */
      available = freadahead (stdin);
      ASSERT (available != 0);
      ASSERT (available <= strlen (expected));
      {
	const char *ptr = freadptr (stdin);

	ASSERT (ptr != NULL);
	ASSERT (memcmp (ptr, expected, available) == 0);
      }

      /* Test behaviour after normal ungetc.  */
      ungetc (fgetc (stdin), stdin);
      available2 = freadahead (stdin);
      ASSERT (/* available2 == available - 1 || */ available2 == available);
#if 0
      if (available2 == available - 1)
	{
	  ASSERT (freadptr (stdin) == NULL);
	}
      else
#endif
	{
	  const char *ptr = freadptr (stdin);

	  ASSERT (ptr != NULL);
	  ASSERT (memcmp (ptr, expected, available) == 0);
	}

      /* Test behaviour after arbitrary ungetc.  */
      fgetc (stdin);
      ungetc ('@', stdin);
      available3 = freadahead (stdin);
      ASSERT (available3 == 0 || available3 == 1 || /* available3 == available - 1 || */ available3 == available);
      if (available3 == 0)
	;
      else if (available3 == 1)
	{
	  const char *ptr = freadptr (stdin);

	  if (ptr != NULL)
	    {
	      ASSERT (ptr[0] == '@');
	    }
	}
#if 0
      else if (available3 == available - 1)
	{
	  ASSERT (freadptr (stdin) == NULL);
	}
#endif
      else
	{
	  const char *ptr = freadptr (stdin);

	  if (ptr != NULL)
	    {
	      ASSERT (ptr[0] == '@');
	      ASSERT (memcmp (ptr + 1, expected + 1, available - 1) == 0);
	    }
	}
    }

  return 0;
}
