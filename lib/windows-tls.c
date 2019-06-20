/* Thread-local storage (native Windows implementation).
   Copyright (C) 2005-2019 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.  */

#include <config.h>

/* Specification.  */
#include "windows-tls.h"

int
glwthread_tls_key_create (glwthread_tls_key_t *keyp, void (*destructor) (void *))
{
  /* TODO: The destructor is unsupported.  */
  (void) destructor;

  if ((*keyp = TlsAlloc ()) == (DWORD)-1)
    return EAGAIN;
  return 0;
}

void *
glwthread_tls_get (glwthread_tls_key_t key)
{
  return TlsGetValue (key);
}

int
glwthread_tls_set (glwthread_tls_key_t key, void *value)
{
  if (!TlsSetValue (key, value))
    return EINVAL;
  return 0;
}

int
glwthread_tls_key_delete (glwthread_tls_key_t key)
{
  if (!TlsFree (key))
    return EINVAL;
  return 0;
}
