/* ISO C 11 thread-specific storage in multithreaded situations.
   Copyright (C) 2005-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2005, 2019.  */

#include <config.h>

#include <threads.h>

#include <string.h>

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

#else
/* Use POSIX threads.  */

# include <pthread.h>

#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

int
tss_create (tss_t *keyp, tss_dtor_t destructor)
{
  int err = glwthread_tls_key_create (keyp, destructor);
  if (err == 0)
    return thrd_success;
  else
    {
      memset (keyp, '\0', sizeof (tss_t));
      return thrd_error;
    }
}

int
tss_set (tss_t key, void *value)
{
  int err = glwthread_tls_set (key, value);
  return (err == 0 ? thrd_success : thrd_error);
}

void *
tss_get (tss_t key)
{
  return glwthread_tls_get (key);
}

void
tss_delete (tss_t key)
{
  glwthread_tls_key_delete (key);
}

#else
/* Use POSIX threads.  */

int
tss_create (tss_t *keyp, tss_dtor_t destructor)
{
  int err = pthread_key_create (keyp, destructor);
  if (err == 0)
    return thrd_success;
  else
    {
      memset (keyp, '\0', sizeof (tss_t));
      return thrd_error;
    }
}

int
tss_set (tss_t key, void *value)
{
  int err = pthread_setspecific (key, value);
  return (err == 0 ? thrd_success : thrd_error);
}

void *
tss_get (tss_t key)
{
  return pthread_getspecific (key);
}

void
tss_delete (tss_t key)
{
  pthread_key_delete (key);
}

#endif
