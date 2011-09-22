/* ftruncate emulations for native Windows.
   This file is in the public domain.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if HAVE_CHSIZE

# include <io.h>

int
ftruncate (int fd, off_t length)
{
  return chsize (fd, length);
}

#endif
