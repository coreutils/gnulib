/* Optimization of multithreaded code.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#ifndef _THREAD_OPTIM_H
#define _THREAD_OPTIM_H

/* This file defines a way to optimize multithreaded code for the single-thread
   case, based on the variable '__libc_single_threaded', defined in
   glibc >= 2.32.  */

/* Typical use: In a block or function, use

     IF_MT_DECL;
     ...
     IF_MT
       if (pthread_mutex_lock (&lock)) abort ();
     ...
     IF_MT
       if (pthread_mutex_unlock (&lock)) abort ();

   The macro IF_MT_DECL establishes local variables for use by IF_MT.

   IF_MT STATEMENT executes STATEMENT in the multithreaded cases, and skips it
   in the single-threaded case.

   The code between IF_MT_DECL and IF_MT must not create threads or invoke
   functions that may indirectly create threads (e.g. 'dlopen' may, indirectly
   through C++ initializers of global variables in the shared library being
   opened, create threads).

   The lock here is meant to synchronize threads in the same process.  The
   same optimization cannot be applied to locks that synchronize different
   processes (e.g. through shared memory mappings).  */

#if HAVE_SYS_SINGLE_THREADED_H /* glibc >= 2.32 */
# include <sys/single_threaded.h>
# define IF_MT_DECL  char optimize_for_single_thread = __libc_single_threaded
# define IF_MT       if (!optimize_for_single_thread)
#else
# define IF_MT_DECL  (void)0
# define IF_MT
#endif

#endif /* _THREAD_OPTIM_H */
