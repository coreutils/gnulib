/* POSIX once-only control.
   Copyright (C) 2019-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* Specification.  */
#include <pthread.h>

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
# include "windows-once.h"
#endif

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
/* Use Windows threads.  */

int
pthread_once (pthread_once_t *once_control, void (*initfunction) (void))
{
  glwthread_once (once_control, initfunction);
  return 0;
}

#elif HAVE_PTHREAD_H
/* Provide workarounds for POSIX threads.  */

#else
/* Provide a dummy implementation for single-threaded applications.  */

int
pthread_once (pthread_once_t *once_control, void (*initfunction) (void))
{
  if (*once_control == 0)
    {
      *once_control = ~ 0;
      initfunction ();
    }
  return 0;
}

#endif
