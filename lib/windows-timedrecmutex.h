/* Timed recursive mutexes (native Windows implementation).
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
   Based on GCC's gthr-win32.h.  */

#ifndef _WINDOWS_TIMEDRECMUTEX_H
#define _WINDOWS_TIMEDRECMUTEX_H

#define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#include <windows.h>

#include <time.h>

#include "windows-initguard.h"

/* The native Windows documentation says that CRITICAL_SECTION already
   implements a recursive lock.  But we need not rely on it: It's easy to
   implement a recursive lock without this assumption.  */

typedef struct
        {
          glwthread_initguard_t guard; /* protects the initialization */
          DWORD owner;
          unsigned long depth;
          HANDLE event;
          CRITICAL_SECTION lock;
        }
        glwthread_timedrecmutex_t;

#define GLWTHREAD_TIMEDRECMUTEX_INIT { GLWTHREAD_INITGUARD_INIT, 0, 0 }

#ifdef __cplusplus
extern "C" {
#endif

extern int glwthread_timedrecmutex_init (glwthread_timedrecmutex_t *mutex);
extern int glwthread_timedrecmutex_lock (glwthread_timedrecmutex_t *mutex);
extern int glwthread_timedrecmutex_trylock (glwthread_timedrecmutex_t *mutex);
extern int glwthread_timedrecmutex_timedlock (glwthread_timedrecmutex_t *mutex,
                                              const struct timespec *abstime);
extern int glwthread_timedrecmutex_unlock (glwthread_timedrecmutex_t *mutex);
extern int glwthread_timedrecmutex_destroy (glwthread_timedrecmutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* _WINDOWS_TIMEDRECMUTEX_H */
