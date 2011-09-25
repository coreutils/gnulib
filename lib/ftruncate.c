/* ftruncate emulations for native Windows.
   This file is in the public domain.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if HAVE_CHSIZE

# include <errno.h>
# include <io.h>

# if HAVE_MSVC_INVALID_PARAMETER_HANDLER
#  include "msvc-inval.h"
static inline int
chsize_nothrow (int fd, long length)
{
  int result;

  TRY_MSVC_INVAL
    {
      result = chsize (fd, length);
    }
  CATCH_MSVC_INVAL
    {
      result = -1;
      errno = EBADF;
    }
  DONE_MSVC_INVAL;

  return result;
}
#  define chsize chsize_nothrow
# endif

int
ftruncate (int fd, off_t length)
{
  return chsize (fd, length);
}

#endif
