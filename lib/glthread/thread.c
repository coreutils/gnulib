/* Creating and controlling threads.
   Copyright (C) 2005-2019 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.
   Based on GCC's gthr-posix.h, gthr-posix95.h, gthr-win32.h.  */

#include <config.h>

/* Specification.  */
# define _GLTHREAD_THREAD_INLINE _GL_EXTERN_INLINE
#include "glthread/thread.h"

#include <stdlib.h>
#include "glthread/lock.h"

/* ========================================================================= */

#if USE_POSIX_THREADS

#include <pthread.h>

#if defined PTW32_VERSION || defined __MVS__

const gl_thread_t gl_null_thread /* = { .p = NULL } */;

#endif

#endif

/* ========================================================================= */

#if USE_WINDOWS_THREADS

#endif

/* ========================================================================= */
