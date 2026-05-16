/* Optimization of multithreaded code.

   Copyright (C) 2020-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#ifndef _THREAD_OPTIM_H
#define _THREAD_OPTIM_H

/* This file defines a way to optimize multithreaded code for the
   single-threaded case, based on the variable '__libc_single_threaded',
   defined in glibc >= 2.32.  */

/* Typical use: In a block or function, use

     bool mt = gl_multithreaded ();
     ...
     if (mt)
       if (pthread_mutex_lock (&lock)) abort ();
     ...
     if (mt)
       if (pthread_mutex_unlock (&lock)) abort ();

   gl_multithreaded () returns true if the process might be multithreaded,
   and false if the process is definitely single-threaded.

   if (mt) STATEMENT executes STATEMENT in the possibly-multithreaded case,
   and skips it in the single-threaded case.

   The code between the gl_multithreaded () invocation and any use of the
   variable 'mt' must not create threads or invoke functions that may
   indirectly create threads (e.g. 'dlopen' may, indirectly through C++
   initializers of global variables in the shared library being opened,
   create threads).

   The lock here is meant to synchronize threads in the same process.  The
   same optimization cannot be applied to locks that synchronize different
   processes (e.g. through shared memory mappings).  */

/* This file uses HAVE_SYS_SINGLE_THREADED_H.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdbool.h>

#if HAVE_SYS_SINGLE_THREADED_H /* glibc >= 2.32 */

# include <sys/single_threaded.h>
# define gl_multithreaded()  (!__libc_single_threaded)

#elif defined __APPLE__ && defined __MACH__ /* macOS */

/* One of the most basic libraries on macOS, CoreFoundation,
   creates new threads without being asked for.  Therefore,
   assume that the process might be multithreaded.  */
# define gl_multithreaded()  1
# define _GL_MULTITHREADED_ALWAYS_TRUE

#elif defined __ELF__ /* Linux, Hurd, FreeBSD, NetBSD, OpenBSD, Solaris, ... */\
      && HAVE_LINK_H \
      && HAVE_DL_ITERATE_PHDR /* not Solaris 10, not Haiku */

/* We can detect whether pthread_create() can be invoked, by looking
   at the contents of the PLT of the executable and of each shared object.  */
# ifdef __cplusplus
extern "C" {
# endif
extern bool gl_multithreaded (void);
# define _GL_MULTITHREADED_VIA_ELF
# ifdef __cplusplus
}
# endif

#else

/* On other platforms, such as Windows, threads are created for relatively
   simple tasks, such as events, alarms, or interruptible sleep.  Therefore,
   assume that the process might be multithreaded.  */
# define gl_multithreaded()  1
# define _GL_MULTITHREADED_ALWAYS_TRUE

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Allows programs to override the result of gl_multithreaded().  */
extern void gl_set_multithreaded (bool mt);

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_OPTIM_H */
