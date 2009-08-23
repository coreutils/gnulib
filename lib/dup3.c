/* Copy a file descriptor, applying specific flags.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#include "binary-io.h"

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */

/* Get declarations of the Win32 API functions.  */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

/* Upper bound on getdtablesize().  See lib/getdtablesize.c.  */
# define OPEN_MAX_MAX 0x10000

#else
/* Unix API.  */

# ifndef O_CLOEXEC
#  define O_CLOEXEC 0
# endif

#endif

int
dup3 (int oldfd, int newfd, int flags)
{
  if (oldfd < 0 || newfd < 0 || newfd >= getdtablesize ())
    {
      errno = EBADF;
      return -1;
    }

  if (newfd == oldfd)
    {
      errno = EINVAL;
      return -1;
    }

  /* Check the supported flags.
     Note that O_NONBLOCK is not supported, because setting it on newfd
     would implicitly also set it on oldfd.  */
  if ((flags & ~(O_CLOEXEC | O_BINARY | O_TEXT)) != 0)
    {
      errno = EINVAL;
      return -1;
    }

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */

  if (flags & O_CLOEXEC)
    {
      /* Neither dup() nor dup2() can create a file descriptor with
	 O_CLOEXEC = O_NOINHERIT set.  We need to use the low-level function
	 _open_osfhandle for this.  Iterate until all file descriptors less
	 than newfd are filled up.  */
      HANDLE curr_process = GetCurrentProcess ();
      HANDLE old_handle = (HANDLE) _get_osfhandle (oldfd);
      unsigned char fds_to_close[OPEN_MAX_MAX / CHAR_BIT];
      unsigned int fds_to_close_bound = 0;
      int result;

      if (old_handle == INVALID_HANDLE_VALUE)
	{
	  /* oldfd is not open, or is an unassigned standard file
	     descriptor.  */
	  errno = EBADF;
	  return -1;
	}

      close (newfd);

      for (;;)
	{
	  HANDLE new_handle;
	  int duplicated_fd;
	  unsigned int index;

	  if (!DuplicateHandle (curr_process,	      /* SourceProcessHandle */
				old_handle,	      /* SourceHandle */
				curr_process,	      /* TargetProcessHandle */
				(PHANDLE) &new_handle, /* TargetHandle */
				(DWORD) 0,	      /* DesiredAccess */
				FALSE,		      /* InheritHandle */
				DUPLICATE_SAME_ACCESS)) /* Options */
	    {
	      errno = EBADF; /* arbitrary */
	      result = -1;
	      break;
	    }
	  duplicated_fd = _open_osfhandle ((long) new_handle, flags);
	  if (duplicated_fd < 0)
	    {
	      CloseHandle (new_handle);
	      result = -1;
	      break;
	    }
	  if (duplicated_fd > newfd)
	    /* Shouldn't happen, since newfd is still closed.  */
	    abort ();
	  if (duplicated_fd == newfd)
	    {
	      result = newfd;
	      break;
	    }

	  /* Set the bit duplicated_fd in fds_to_close[].  */
	  index = (unsigned int) duplicated_fd / CHAR_BIT;
	  if (index >= fds_to_close_bound)
	    {
	      if (index >= sizeof (fds_to_close))
		/* Need to increase OPEN_MAX_MAX.  */
		abort ();
	      memset (fds_to_close + fds_to_close_bound, '\0',
		      index + 1 - fds_to_close_bound);
	      fds_to_close_bound = index + 1;
	    }
	  fds_to_close[index] |= 1 << ((unsigned int) duplicated_fd % CHAR_BIT);
	}

      /* Close the previous fds that turned out to be too small.  */
      {
	int saved_errno = errno;
	unsigned int duplicated_fd;

	for (duplicated_fd = 0;
	     duplicated_fd < fds_to_close_bound * CHAR_BIT;
	     duplicated_fd++)
	  if ((fds_to_close[duplicated_fd / CHAR_BIT]
	       >> (duplicated_fd % CHAR_BIT))
	      & 1)
	    close (duplicated_fd);

	errno = saved_errno;
      }

      return result;
    }

  if (dup2 (oldfd, newfd) < 0)
    return -1;

#else
/* Unix API.  */

  if (dup2 (oldfd, newfd) < 0)
    return -1;

  /* POSIX <http://www.opengroup.org/onlinepubs/9699919799/functions/dup.html>
     says that initially, the FD_CLOEXEC flag is cleared on newfd.  */

  if (flags & O_CLOEXEC)
    {
      int fcntl_flags;

      if ((fcntl_flags = fcntl (newfd, F_GETFD, 0)) < 0
	  || fcntl (newfd, F_SETFD, fcntl_flags | FD_CLOEXEC) == -1)
	{
	  int saved_errno = errno;
	  close (newfd);
	  errno = saved_errno;
	  return -1;
	}
    }

#endif

#if O_BINARY
  if (flags & O_BINARY)
    setmode (newfd, O_BINARY);
  else if (flags & O_TEXT)
    setmode (newfd, O_TEXT);
#endif

  return newfd;
}
