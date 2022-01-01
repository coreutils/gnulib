/* POSIX thread-specific storage.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

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
# include "windows-tls.h"
#else
# include <stdlib.h>
#endif

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
/* Use Windows threads.  */

int
pthread_key_create (pthread_key_t *keyp, void (*destructor) (void *))
{
  return glwthread_tls_key_create (keyp, destructor);
}

int
pthread_setspecific (pthread_key_t key, const void *value)
{
  return glwthread_tls_set (key, (void *) value);
}

void *
pthread_getspecific (pthread_key_t key)
{
  return glwthread_tls_get (key);
}

int
pthread_key_delete (pthread_key_t key)
{
  return glwthread_tls_key_delete (key);
}

#elif HAVE_PTHREAD_H
/* Provide workarounds for POSIX threads.  */

#else
/* Provide a dummy implementation for single-threaded applications.  */

int
pthread_key_create (pthread_key_t *keyp, void (*destructor) (void *))
{
  pthread_key_t key = (void **) malloc (sizeof (void *));
  if (key == NULL)
    return ENOMEM;
  *key = NULL;
  *keyp = key;
  return 0;
}

int
pthread_setspecific (pthread_key_t key, const void *value)
{
  *key = (void *) value;
  return 0;
}

void *
pthread_getspecific (pthread_key_t key)
{
  return *key;
}

int
pthread_key_delete (pthread_key_t key)
{
  free (key);
  return 0;
}

#endif
