/* Return a pointer to a zero-size object in memory.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* ISO C 99 does not allow memcmp(), memchr() etc. to be invoked with a NULL
   argument.  Therefore this file produces a non-NULL pointer which cannot
   be dereferenced, if possible.  */

#include <stdlib.h>

#if HAVE_MPROTECT
# include <fcntl.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/mman.h>
/* Define MAP_FILE when it isn't otherwise.  */
# ifndef MAP_FILE
#  define MAP_FILE 0
# endif
#endif

/* Return a pointer to a zero-size object in memory, if possible.
   Return NULL otherwise.  */

static void *
zerosize_ptr (void)
{
/* Use mmap and mprotect when they exist.  Don't test HAVE_MMAP, because it is
   not defined on HP-UX 11 (since it does not support MAP_FIXED).  */
#if HAVE_MPROTECT
# if HAVE_MAP_ANONYMOUS
  const int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  const int fd = -1;
# else /* !HAVE_MAP_ANONYMOUS */
  const int flags = MAP_FILE | MAP_PRIVATE;
  int fd = open ("/dev/zero", O_RDONLY, 0666);
  if (fd >= 0)
# endif
    {
      int pagesize = getpagesize ();
      char *two_pages =
	(char *) mmap (NULL, 2 * pagesize, PROT_READ | PROT_WRITE,
		       flags, fd, 0);
      if (two_pages != (char *)(-1)
	  && mprotect (two_pages + pagesize, pagesize, PROT_NONE) == 0)
	return two_pages + pagesize;
    }
#endif
  return NULL;
}
