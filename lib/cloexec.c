/* closexec.c - set or clear the close-on-exec descriptor flag

   Copyright (C) 1991, 2004, 2005, 2006, 2009 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   The code is taken from glibc/manual/llio.texi  */

#include <config.h>

#include "cloexec.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif


/* Set the `FD_CLOEXEC' flag of DESC if VALUE is true,
   or clear the flag if VALUE is false.
   Return 0 on success, or -1 on error with `errno' set.

   Note that on MingW, this function does NOT protect DESC from being
   inherited into spawned children.  Instead, either use dup_cloexec
   followed by closing the original DESC, or use interfaces such as
   open or pipe2 that accept flags like O_CLOEXEC to create DESC
   non-inheritable in the first place.  */

int
set_cloexec_flag (int desc, bool value)
{
#if defined F_GETFD && defined F_SETFD

  int flags = fcntl (desc, F_GETFD, 0);

  if (0 <= flags)
    {
      int newflags = (value ? flags | FD_CLOEXEC : flags & ~FD_CLOEXEC);

      if (flags == newflags
          || fcntl (desc, F_SETFD, newflags) != -1)
        return 0;
    }

  return -1;

#else

  /* Use dup2 to reject invalid file descriptors; the cloexec flag
     will be unaffected.  */
  if (desc < 0)
    {
      errno = EBADF;
      return -1;
    }
  if (dup2 (desc, desc) < 0)
    /* errno is EBADF here.  */
    return -1;

  /* There is nothing we can do on this kind of platform.  Punt.  */
  return 0;
#endif
}


/* Duplicates a file handle FD, while marking the copy to be closed
   prior to exec or spawn.  Returns -1 and sets errno if FD could not
   be duplicated.  */

int
dup_cloexec (int fd)
{
  int nfd;

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

  /* Native Woe32 API.  */
  HANDLE curr_process = GetCurrentProcess ();
  HANDLE old_handle = (HANDLE) _get_osfhandle (fd);
  HANDLE new_handle;
  int mode;

  if (old_handle == INVALID_HANDLE_VALUE
      || (mode = setmode (fd, O_BINARY)) == -1)
    {
      /* fd is closed, or is open to no handle at all.
         We cannot duplicate fd in this case, because _open_osfhandle
         fails for an INVALID_HANDLE_VALUE argument.  */
      errno = EBADF;
      return -1;
    }
  setmode (fd, mode);

  if (!DuplicateHandle (curr_process,               /* SourceProcessHandle */
                        old_handle,                 /* SourceHandle */
                        curr_process,               /* TargetProcessHandle */
                        (PHANDLE) &new_handle,      /* TargetHandle */
                        (DWORD) 0,                  /* DesiredAccess */
                        FALSE,                      /* InheritHandle */
                        DUPLICATE_SAME_ACCESS))     /* Options */
    {
      /* TODO: Translate GetLastError () into errno.  */
      errno = EMFILE;
      return -1;
    }

  nfd = _open_osfhandle ((long) new_handle, mode | O_NOINHERIT);
  if (nfd < 0)
    {
      CloseHandle (new_handle);
      errno = EMFILE;
      return -1;
    }

#  if REPLACE_FCHDIR
  if (0 <= nfd)
    nfd = _gl_register_dup (fd, nfd);
#  endif
  return nfd;

#else /* !_WIN32 */

  /* Unix API.  */

# ifdef F_DUPFD_CLOEXEC
  nfd = fcntl (fd, F_DUPFD_CLOEXEC, 0);
#  if REPLACE_FCHDIR
  if (0 <= nfd)
    nfd = _gl_register_dup (fd, nfd);
#  endif

# else /* !F_DUPFD_CLOEXEC */
  nfd = dup (fd);
  if (0 <= nfd && set_cloexec_flag (nfd, true) < 0)
    {
      int saved_errno = errno;
      close (nfd);
      nfd = -1;
      errno = saved_errno;
    }
# endif /* !F_DUPFD_CLOEXEC */

  return nfd;

#endif /* !_WIN32 */
}
