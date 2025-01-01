/* ISO C 11 once-only initialization.
   Copyright (C) 2005-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005, 2019.
   Based on GCC's gthr-posix.h, gthr-posix95.h, gthr-win32.h.  */

#include <config.h>

#include <threads.h>

#include <errno.h>

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# include <stdlib.h>

#else
/* Use POSIX threads.  */

# include <pthread.h>

#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

void
call_once (once_flag *flagp, void (*func) (void))
{
  glwthread_once (flagp, func);
}

#else
/* Use POSIX threads.  */

void
call_once (once_flag *flagp, void (*func) (void))
{
# if defined __CYGWIN__
  /* Verify that once_flag and pthread_once_t are of the same size.  */
  struct _ { int v [sizeof (once_flag) == sizeof (pthread_once_t) ? 1 : -1]; };
  pthread_once ((pthread_once_t *) flagp, func);
# else
  pthread_once (flagp, func);
# endif
}

#endif
