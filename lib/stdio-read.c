/* POSIX compatible FILE stream read function.
   Copyright (C) 2008-2012 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

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

#include <config.h>

/* Specification.  */
#include <stdio.h>

/* Replace these functions only if module 'nonblocking' is requested.  */
#if GNULIB_NONBLOCKING

/* On native Windows platforms, when read() is called on a non-blocking pipe
   with an empty buffer, ReadFile() fails with error GetLastError() =
   ERROR_NO_DATA, and read() in consequence fails with error EINVAL.  This
   read() function is at the basis of the function which fills the buffer of
   a FILE stream.  */

# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

#  include <errno.h>
#  include <io.h>

#  define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#  include <windows.h>

#  include "msvc-nothrow.h"

#  define CALL_WITH_ERRNO_FIX(RETTYPE, EXPRESSION, FAILED) \
  if (ferror (stream))                                                        \
    return (EXPRESSION);                                                      \
  else                                                                        \
    {                                                                         \
      RETTYPE ret;                                                            \
      SetLastError (0);                                                       \
      ret = (EXPRESSION);                                                     \
      if (FAILED)                                                             \
        {                                                                     \
          if (GetLastError () == ERROR_NO_DATA && ferror (stream))            \
            {                                                                 \
              int fd = fileno (stream);                                       \
              if (fd >= 0)                                                    \
                {                                                             \
                  HANDLE h = (HANDLE) _get_osfhandle (fd);                    \
                  if (GetFileType (h) == FILE_TYPE_PIPE)                      \
                    {                                                         \
                      /* h is a pipe or socket.  */                           \
                      DWORD state;                                            \
                      if (GetNamedPipeHandleState (h, &state, NULL, NULL,     \
                                                   NULL, NULL, 0)             \
                          && (state & PIPE_NOWAIT) != 0)                      \
                        /* h is a pipe in non-blocking mode.                  \
                           Change errno from EINVAL to EAGAIN.  */            \
                        errno = EAGAIN;                                       \
                    }                                                         \
                }                                                             \
            }                                                                 \
        }                                                                     \
      return ret;                                                             \
    }

int
scanf (const char *format, ...)
{
  int retval;
  va_list args;

  va_start (args, format);
  retval = vfscanf (stdin, format, args);
  va_end (args);

  return retval;
}

int
fscanf (FILE *stream, const char *format, ...)
{
  int retval;
  va_list args;

  va_start (args, format);
  retval = vfscanf (stream, format, args);
  va_end (args);

  return retval;
}

int
vscanf (const char *format, va_list args)
{
  return vfscanf (stdin, format, args);
}

int
vfscanf (FILE *stream, const char *format, va_list args)
#undef vfscanf
{
  CALL_WITH_ERRNO_FIX (int, vfscanf (stream, format, args), ret == EOF)
}

int
getchar (void)
{
  return fgetc (stdin);
}

int
fgetc (FILE *stream)
#undef fgetc
{
  CALL_WITH_ERRNO_FIX (int, fgetc (stream), ret == EOF)
}

char *
fgets (char *s, int n, FILE *stream)
#undef fgets
{
  CALL_WITH_ERRNO_FIX (char *, fgets (s, n, stream), ret == NULL)
}

char *
gets (char *s)
#undef gets
{
  FILE *stream = stdin;
  CALL_WITH_ERRNO_FIX (char *, gets (s), ret == NULL)
}

size_t
fread (void *ptr, size_t s, size_t n, FILE *stream)
#undef fread
{
  CALL_WITH_ERRNO_FIX (size_t, fread (ptr, s, n, stream), ret < n)
}

# endif
#endif
