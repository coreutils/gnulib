/* realloc() function that is glibc compatible.

   Copyright (C) 1997, 2003-2004, 2006-2007, 2009-2024 Free Software
   Foundation, Inc.

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

/* written by Jim Meyering and Bruno Haible */

#include <config.h>

#include <stdlib.h>

#include <errno.h>

#include "xalloc-oversized.h"

/* Call the system's realloc below.  This file does not define
   _GL_USE_STDLIB_ALLOC because it needs Gnulib's malloc if present.  */
#undef realloc

/* Change the size of an allocated block of memory P to N bytes,
   with error checking.  If P is NULL, use malloc.  Otherwise if N is zero,
   free P and return NULL.  */

void *
rpl_realloc (void *p, size_t n)
{
  if (p == NULL)
    return malloc (n);

  if (n == 0)
    {
#if NEED_SANITIZED_REALLOC
      /* ISO C 23 § 7.24.3.7.(3) says that this case is undefined behaviour.
         Let the programmer know that it occurred.
         When the undefined-behaviour sanitizers report this case, i.e. when
         <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=117233> and
         <https://github.com/llvm/llvm-project/issues/113065>
         have been closed and new releases of GCC and clang have been made,
         we can remove this code here.  */
      abort ();
#else
      free (p);
      return NULL;
#endif
    }

  if (xalloc_oversized (n, 1))
    {
      errno = ENOMEM;
      return NULL;
    }

  void *result = realloc (p, n);

#if !HAVE_MALLOC_POSIX
  if (result == NULL)
    errno = ENOMEM;
#endif

  return result;
}
