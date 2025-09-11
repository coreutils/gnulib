/* Memory allocation aligned to system page boundaries.

   Copyright (C) 2005-2007, 2009-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Derek R. Price <derek@ximbiot.com>.  */

#include <config.h>

/* Specification.  */
#include "pagealign_alloc.h"

#include <errno.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#if HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif
#if defined _WIN32 && !defined __CYGWIN__
# include <malloc.h>
# define WIN32_LEAN_AND_MEAN /* avoid including junk */
# include <windows.h>
#endif

#include <error.h>
#include "xalloc.h"
#include "gettext.h"

#define _(msgid) dgettext (GNULIB_TEXT_DOMAIN, msgid)

#if HAVE_SYS_MMAN_H
/* Define MAP_FAILED for old systems which neglect to.  */
# ifndef MAP_FAILED
#  define MAP_FAILED ((void *)-1)
# endif
#endif


/* Implementation of pagealign_alloc.  */
pagealign_impl_t pagealign_impl;

typedef union
{
  /* For PA_IMPL_MALLOC:
     For each memory region, we store the original pointer returned by
     malloc().  */
  void *pointer;
  /* For PA_IMPL_MMAP, PA_IMPL_VIRTUAL_ALLOC:
     For each memory region, we store its size.  */
  size_t size;
} info_t;


/* For PA_IMPL_MALLOC, PA_IMPL_MMAP.  */

/* A simple linked list of allocated memory regions.  It is probably not the
   most efficient way to store these, but anyway...  */
typedef struct memnode_s memnode_t;
struct memnode_s
{
  void *aligned_ptr;
  info_t info;
  memnode_t *next;
};

/* The list of currently allocated memory regions.  */
static memnode_t *memnode_table = NULL;

static void
new_memnode (void *aligned_ptr, info_t info)
{
  memnode_t *new_node = XMALLOC (memnode_t);
  new_node->aligned_ptr = aligned_ptr;
  new_node->info = info;
  new_node->next = memnode_table;
  memnode_table = new_node;
}

/* Dispose of the memnode containing a map for the ALIGNED_PTR in question
   and return the content of the node's INFO field.  */
static info_t
get_memnode (void *aligned_ptr)
{
  info_t ret;
  memnode_t *c;
  memnode_t **p_next = &memnode_table;

  for (c = *p_next; c != NULL; p_next = &c->next, c = c->next)
    if (c->aligned_ptr == aligned_ptr)
      break;

  if (c == NULL)
    /* An attempt to free untracked memory.  A wrong pointer was passed
       to pagealign_free().  */
    abort ();

  /* Remove this entry from the list, save the return value, and free it.  */
  *p_next = c->next;
  ret = c->info;
  free (c);

  return ret;
}


/* Returns the default implementation.  */
static pagealign_impl_t
get_default_impl (void)
{
#if HAVE_POSIX_MEMALIGN
  return PA_IMPL_POSIX_MEMALIGN;
#elif HAVE_SYS_MMAN_H
  return PA_IMPL_MMAP;
#else
  return PA_IMPL_MALLOC;
#endif
}


void *
pagealign_alloc (size_t size)
{
  void *ret;
  pagealign_impl_t impl;

  impl = pagealign_impl;
  if (impl == PA_IMPL_DEFAULT)
    impl = get_default_impl ();

  switch (impl)
    {
    case PA_IMPL_MALLOC:
      {
        size_t pagesize = getpagesize ();
        void *unaligned_ptr = malloc (size + pagesize - 1);
        if (unaligned_ptr == NULL)
          {
            /* Set errno.  We don't know whether malloc already set errno: some
               implementations of malloc do, some don't.  */
            errno = ENOMEM;
            return NULL;
          }
        ret = (char *) unaligned_ptr
              + ((- (uintptr_t) unaligned_ptr) & (pagesize - 1));
        info_t info;
        info.pointer = unaligned_ptr;
        new_memnode (ret, info);
      }
      break;

    case PA_IMPL_MMAP:
      #if HAVE_SYS_MMAN_H
      ret = mmap (NULL, size, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
      if (ret == MAP_FAILED)
        return NULL;
      info_t info;
      info.size = size;
      new_memnode (ret, info);
      break;
      #else
      errno = ENOSYS;
      return NULL;
      #endif

    case PA_IMPL_POSIX_MEMALIGN:
      #if HAVE_POSIX_MEMALIGN
      {
        int status = posix_memalign (&ret, getpagesize (), size);
        if (status)
          {
            errno = status;
            return NULL;
          }
      }
      break;
      #else
      errno = ENOSYS;
      return NULL;
      #endif

    case PA_IMPL_ALIGNED_MALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-malloc>  */
      return _aligned_malloc (size, getpagesize ());
      #else
      errno = ENOSYS;
      return NULL;
      #endif

    case PA_IMPL_VIRTUAL_ALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc>  */
      ret = VirtualAlloc (NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
      if (ret == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
      info_t info;
      info.size = size;
      new_memnode (ret, info);
      #else
      errno = ENOSYS;
      return NULL;
      #endif

    default:
      abort ();
    }

  return ret;
}

void *
pagealign_xalloc (size_t size)
{
  void *ret;

  ret = pagealign_alloc (size);
  if (ret == NULL)
    xalloc_die ();
  return ret;
}


void
pagealign_free (void *aligned_ptr)
{
  pagealign_impl_t impl;

  impl = pagealign_impl;
  if (impl == PA_IMPL_DEFAULT)
    impl = get_default_impl ();

  switch (impl)
    {
    case PA_IMPL_MALLOC:
      free (get_memnode (aligned_ptr).pointer);
      break;

    case PA_IMPL_MMAP:
      #if HAVE_SYS_MMAN_H
      if (munmap (aligned_ptr, get_memnode (aligned_ptr).size) < 0)
        error (EXIT_FAILURE, errno, "Failed to unmap memory");
      #else
      abort ();
      #endif

    case PA_IMPL_POSIX_MEMALIGN:
      #if HAVE_POSIX_MEMALIGN
      free (aligned_ptr);
      #else
      abort ();
      #endif

    case PA_IMPL_ALIGNED_MALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-free>  */
      _aligned_free (aligned_ptr);
      #else
      abort ();
      #endif

    case PA_IMPL_VIRTUAL_ALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree>  */
      if (!VirtualFree (aligned_ptr, get_memnode (aligned_ptr).size, MEM_RELEASE))
        error (EXIT_FAILURE, 0, "Failed to free memory");
      #else
      abort ();
      #endif

    default:
      abort ();
    }
}
