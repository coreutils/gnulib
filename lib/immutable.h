/* Immutable data.

   Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#ifndef _IMMUTABLE_H
#define _IMMUTABLE_H

/* This file provide a facility to allocate and free immutable data objects.

   An immutable data object is allocated in three steps:
     1. You allocate an immutable memory region.
          DATA *wp = immmalloc (sizeof (*wp));
        The pointer wp is actually a writable view to the memory region.
     2. You fill the memory region, through the pointer wp:
          wp->x = ...;
          wp->y = ...;
          ...
     3. You declare the memory region as frozen.  This means that you relinquish
        write access.
          DATA const *p = immfreeze (wp);
        You can now let wp get out-of-scope.

   Then the pointer p can be used only in read-only ways.  That is, if you cast
   away the 'const' and attempt to write to the memory region, it will crash at
   runtime (through a SIGSEGV signal).
     p->x = ...;             // rejected by the compiler
     ((DATA *) p)->x = ...;  // crashes at runtime

   Finally, you can free the immutable data object:
     immfree (p);
 */

/* If you compile this module with the C macro NO_IMMUTABLE set to 1, or on a
   platform that lacks support for read-only and writeable memory areas, the
   functions work alike, except that the "read-only" pointers are actually
   writable.  */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This macro tells whether the implementation effectively rejects writes to
   immutable data.  */
#if !NO_IMMUTABLE && ((defined _WIN32 && !defined __CYGWIN__) || HAVE_WORKING_MPROTECT)
# define IMMUTABLE_EFFECTIVE 1
#else
# define IMMUTABLE_EFFECTIVE 0
#endif

/* Allocates an immutable memory region.
   SIZE if the number of bytes; should be > 0.
   Returns a writeable pointer to the memory region.
   Upon memory allocation failure, returns NULL with errno set to ENOMEM.  */
extern void * immmalloc (size_t size);

/* Freezes an immutable memory region.
   WRITABLE_POINTER is a non-NULL return value from immmalloc().
   Returns a read-only pointer to the same memory region.  */
extern const void * immfreeze (void *writable_pointer);

/* Frees an immutable memory region.
   READONLY_POINTER is a return value from immfreeze().  */
extern void immfree (const void *readonly_pointer);

/* The following is just an application to some data types.  */

/* Allocates an immutable memory region that contains a copy of the given string.
   Returns a read-only pointer to this duplicated string.
   Upon memory allocation failure, returns NULL with errno set to ENOMEM.  */
extern const char * immstrdup (const char *string);

#ifdef __cplusplus
}
#endif

#endif /* _IMMUTABLE_H */
