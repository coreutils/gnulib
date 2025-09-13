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
#include "gl_xmap.h"
#include "gl_hash_map.h"
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

/* Map:
   For PA_IMPL_MALLOC:
     aligned_ptr -> void *.
     For each memory region, we store the original pointer returned by malloc().
   For PA_IMPL_MMAP:
     aligned_ptr -> size_t.
     For each memory region, we store its size.  */
static gl_map_t page_info_map;


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
        if (page_info_map == NULL)
          page_info_map =
            gl_map_create_empty (GL_HASH_MAP, NULL, NULL, NULL, NULL);
        gl_map_put (page_info_map, ret, unaligned_ptr);
      }
      break;

    case PA_IMPL_MMAP:
      #if HAVE_SYS_MMAN_H
      ret = mmap (NULL, size, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
      if (ret == MAP_FAILED)
        return NULL;
      if (page_info_map == NULL)
        page_info_map =
          gl_map_create_empty (GL_HASH_MAP, NULL, NULL, NULL, NULL);
      gl_map_put (page_info_map, ret, (void *) (uintptr_t) size);
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
      break;
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
      {
        const void *value;
        if (page_info_map == NULL
            || !gl_map_getremove (page_info_map, aligned_ptr, &value))
          abort ();
        void *unaligned_ptr = (void *) value;
        free (unaligned_ptr);
      }
      break;

    case PA_IMPL_MMAP:
      #if HAVE_SYS_MMAN_H
      {
        const void *value;
        if (page_info_map == NULL
            || !gl_map_getremove (page_info_map, aligned_ptr, &value))
          abort ();
        if (munmap (aligned_ptr, (size_t) (uintptr_t) value) < 0)
          error (EXIT_FAILURE, errno, "Failed to unmap memory");
      }
      break;
      #else
      abort ();
      #endif

    case PA_IMPL_POSIX_MEMALIGN:
      #if HAVE_POSIX_MEMALIGN
      free (aligned_ptr);
      break;
      #else
      abort ();
      #endif

    case PA_IMPL_ALIGNED_MALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/aligned-free>  */
      _aligned_free (aligned_ptr);
      break;
      #else
      abort ();
      #endif

    case PA_IMPL_VIRTUAL_ALLOC:
      #if defined _WIN32 && !defined __CYGWIN__
      /* Documentation:
         <https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree>  */
      if (!VirtualFree (aligned_ptr, 0, MEM_RELEASE))
        error (EXIT_FAILURE, 0, "Failed to free memory");
      break;
      #else
      abort ();
      #endif

    default:
      abort ();
    }
}
