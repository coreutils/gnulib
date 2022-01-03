/* Invoke popen, but avoid some glitches.

   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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

/* Written by Eric Blake.  */

#include <config.h>

#include "stdio-safer.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/* Like popen, but do not return stdin, stdout, or stderr.  */

FILE *
popen_safer (char const *cmd, char const *mode)
{
  /* Unfortunately, we cannot use the fopen_safer approach of using
     fdopen (dup_safer (fileno (popen (cmd, mode)))), because stdio
     libraries maintain hidden state tying the original fd to the pid
     to wait on when using pclose (this hidden state is also used to
     avoid fd leaks in subsequent popen calls).  So, we instead
     guarantee that all standard streams are open prior to the popen
     call (even though this puts more pressure on open fds), so that
     the original fd created by popen is safe.  */
  FILE *fp;
  int fd = open ("/dev/null", O_RDONLY | O_CLOEXEC);
  if (0 <= fd && fd <= STDERR_FILENO)
    {
      /* Maximum recursion depth is 3.  */
      int saved_errno;
      fp = popen_safer (cmd, mode);
      saved_errno = errno;
      close (fd);
      errno = saved_errno;
    }
  else
    {
      /* Either all fd's are tied up, or fd is safe and the real popen
         will reuse it.  */
      close (fd);
      fp = popen (cmd, mode);
    }
  return fp;
}
