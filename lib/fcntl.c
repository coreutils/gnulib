/* Provide file descriptor control.

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

/* Written by Eric Blake <ebb9@byu.net>.  */

#include <config.h>

/* Specification.  */
#include <fcntl.h>

#include <errno.h>
#include <stdarg.h>

#if !HAVE_FCNTL
# error not ported to mingw yet
#endif
#undef fcntl

/* Perform the specified ACTION on the file descriptor FD, possibly
   using the argument ARG further described below.  This replacement
   handles the following actions, and forwards all others on to the
   native fcntl.

   F_DUPFD_CLOEXEC - duplicate FD, with int ARG being the minimum
   target fd.  If successful, return the duplicate, which will not be
   inheritable; otherwise return -1 and set errno.  */

int
rpl_fcntl (int fd, int action, /* arg */...)
{
  va_list arg;
  int result = -1;
  va_start (arg, action);
  switch (action)
    {
    case F_DUPFD_CLOEXEC:
      {
        int target = va_arg (arg, int);

        /* Try the system call first, if the headers claim it exists
           (that is, if GNULIB_defined_F_DUPFD_CLOEXEC is 0), since we
           may be running with a glibc that has the macro but with an
           older kernel that does not support it.  Cache the
           information on whether the system call really works, but
           avoid caching failure if the corresponding F_DUPFD fails
           for any reason.  0 = unknown, 1 = yes, -1 = no.  */
        static int have_dupfd_cloexec = GNULIB_defined_F_DUPFD_CLOEXEC ? -1 : 0;
        if (0 <= have_dupfd_cloexec)
          {
            result = fcntl (fd, action, target);
            if (0 <= result || errno != EINVAL)
              have_dupfd_cloexec = 1;
            else
              {
                result = fcntl (fd, F_DUPFD, target);
                if (result < 0)
                  break;
                have_dupfd_cloexec = -1;
              }
          }
        else
          result = fcntl (fd, F_DUPFD, target);
        if (0 <= result && have_dupfd_cloexec == -1)
          {
            int flags = fcntl (result, F_GETFD);
            if (flags < 0 || fcntl (result, F_SETFD, flags | FD_CLOEXEC) == -1)
              {
                int saved_errno = errno;
                close (result);
                errno = saved_errno;
                result = -1;
              }
          }
#if REPLACE_FCHDIR
        if (0 <= result)
          result = _gl_register_dup (fd, result);
#endif
        break;
      } /* F_DUPFD_CLOEXEC */

    default:
      {
        void *p = va_arg (arg, void *);
        result = fcntl (fd, action, p);
        break;
      }
    }
  va_end (arg);
  return result;
}
