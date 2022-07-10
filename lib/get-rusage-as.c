/* Getter for RLIMIT_AS.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

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

#include <config.h>

/* On Android, when targeting Android 4.4 or older with a GCC toolchain,
   prevent a compilation error
     "error: call to 'mmap' declared with attribute error: mmap is not
      available with _FILE_OFFSET_BITS=64 when using GCC until android-21.
      Either raise your minSdkVersion, disable _FILE_OFFSET_BITS=64, or
      switch to Clang."
   The files that we access in this compilation unit are less than 2 GB
   large.  */
#if defined __ANDROID__
# undef _FILE_OFFSET_BITS
#endif

/* Specification.  */
#include "resource-ext.h"

/* The "address space size" is defined as the total size of the virtual memory
   areas of the current process.  This includes
     - areas belonging to the executable and shared libraries,
     - areas allocated by malloc() or mmap(),
     - the stack and environment areas,
     - gaps and guard pages (mappings with PROT_NONE),
     - other system dependent areas, such as vsyscall or vdso on Linux.

   There are two ways of retrieving the current address space size:
     a) by trying setrlimit with various values and observing whether the
        kernel allows additional mmap calls,
     b) by using system dependent APIs that allow to iterate over the list
        of virtual memory areas.
   We don't use the mincore() based approach here, because it would be very
   slow when applied to an entire address space, especially on 64-bit
   platforms.
   We define two functions
     get_rusage_as_via_setrlimit(),
     get_rusage_as_via_iterator().

   Discussion per platform:

   Linux:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.
     Both methods agree, except that on x86_64 systems, the value of
     get_rusage_as_via_iterator() is 4 KB higher than
     get_rusage_as_via_setrlimit().

   Mac OS X:
     a) setrlimit with RLIMIT_AS succeeds but does not really work: The OS
        ignores RLIMIT_AS. mmap() of a page always succeeds, therefore
        get_rusage_as_via_setrlimit() is always 0.
     b) The Mach based API works.

   FreeBSD:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.

   NetBSD:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.
     Both methods agree,

   OpenBSD:
     a) setrlimit exists, but RLIMIT_AS is not defined.
     b) mquery() can be used to find out about the virtual memory areas.

   AIX:
     a) setrlimit with RLIMIT_AS succeeds but does not really work: The OS
        apparently ignores RLIMIT_AS. mmap() of a page always succeeds,
        therefore get_rusage_as_via_setrlimit() is always 0.
     b) The /proc/$pid/map file contains a list of the virtual memory areas.

   HP-UX:
     a) setrlimit with RLIMIT_AS works.
     b) pstat_getprocvm() can be used to find out about the virtual memory
        areas.
     Both methods agree, except that the value of get_rusage_as_via_iterator()
     is slightly larger higher than get_rusage_as_via_setrlimit(), by 4 KB in
     32-bit mode and by 40 KB in 64-bit mode.

   IRIX:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP.
     Both methods agree,

   OSF/1:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP.
     The value returned by get_rusage_as_via_setrlimit() is 64 KB higher than
     get_rusage_as_via_iterator().  It's not clear why.

   Solaris:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP, and the
        /proc/self/maps file contains a list of the virtual memory areas.
     Both methods agree,

   Cygwin:
     a) setrlimit with RLIMIT_AS always fails when the limit is < 0x80000000.
        get_rusage_as_via_setrlimit() therefore produces a wrong value.
     b) The /proc/$pid/maps file lists only the memory areas belonging to
        the executable and shared libraries, not the anonymous memory.
        But the native Windows API works.

   mingw:
     a) There is no setrlimit function.
     b) The native Windows API works.

   BeOS, Haiku:
     a) On BeOS, there is no setrlimit function.
        On Haiku, setrlimit exists. RLIMIT_AS is defined but setrlimit fails.
     b) There is a specific BeOS API: get_next_area_info().
 */


#include <errno.h> /* errno */
#include <stdlib.h> /* size_t, abort */
#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* getpagesize, read, close */


/* System support for get_rusage_as_via_setrlimit().  */

#if HAVE_SETRLIMIT
# include <sys/time.h>
# include <sys/resource.h> /* getrlimit, setrlimit */
#endif

/* Test whether mmap() and mprotect() are available.
   We don't use HAVE_MMAP, because AC_FUNC_MMAP would not define it on HP-UX.
   HAVE_MPROTECT is not enough, because mingw does not have mmap() but has an
   mprotect() function in libgcc.a.  */
#if HAVE_SYS_MMAN_H && HAVE_MPROTECT
# include <fcntl.h>
# include <sys/types.h>
# include <sys/mman.h> /* mmap, munmap */
/* Define MAP_FILE when it isn't otherwise.  */
# ifndef MAP_FILE
#  define MAP_FILE 0
# endif
#endif


/* System support for get_rusage_as_via_iterator().  */

#include "vma-iter.h"


#if HAVE_SETRLIMIT && defined RLIMIT_AS && HAVE_SYS_MMAN_H && HAVE_MPROTECT && !defined __HAIKU__

static uintptr_t
get_rusage_as_via_setrlimit (void)
{
  uintptr_t result;

  struct rlimit orig_limit;

# if HAVE_MAP_ANONYMOUS
  const int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  const int fd = -1;
# else /* !HAVE_MAP_ANONYMOUS */
  const int flags = MAP_FILE | MAP_PRIVATE;
  int fd = open ("/dev/zero", O_RDONLY | O_CLOEXEC, 0666);
  if (fd < 0)
    return 0;
# endif

  /* Record the original limit.  */
  if (getrlimit (RLIMIT_AS, &orig_limit) < 0)
    {
      result = 0;
      goto done2;
    }

  if (orig_limit.rlim_max != RLIM_INFINITY
      && (orig_limit.rlim_cur == RLIM_INFINITY
          || orig_limit.rlim_cur > orig_limit.rlim_max))
    /* We may not be able to restore the current rlim_cur value.
       So bail out.  */
    {
      result = 0;
      goto done2;
    }

  {
    /* The granularity is a single page.  */
    const size_t pagesize = getpagesize ();

    uintptr_t low_bound = 0;
    uintptr_t high_bound;

    for (;;)
      {
        /* Here we know that the address space size is >= low_bound.  */
        struct rlimit try_limit;
        uintptr_t try_next = 2 * low_bound + pagesize;

        if (try_next < low_bound)
          /* Overflow.  */
          try_next = ((uintptr_t) (~ 0) / pagesize) * pagesize;

        /* There's no point in trying a value > orig_limit.rlim_max, as
           setrlimit would fail anyway.  */
        if (orig_limit.rlim_max != RLIM_INFINITY
            && orig_limit.rlim_max < try_next)
          try_next = orig_limit.rlim_max;

        /* Avoid endless loop.  */
        if (try_next == low_bound)
          {
            /* try_next could not be increased.  */
            result = low_bound;
            goto done1;
          }

        try_limit.rlim_max = orig_limit.rlim_max;
        try_limit.rlim_cur = try_next;
        if (setrlimit (RLIMIT_AS, &try_limit) == 0)
          {
            /* Allocate a page of memory, to compare the current address space
               size with try_limit.rlim_cur.  */
            void *new_page =
              mmap (NULL, pagesize, PROT_READ | PROT_WRITE, flags, fd, 0);

            if (new_page != (void *)(-1))
              {
                /* The page could be added successfully.  Free it.  */
                if (munmap (new_page, pagesize) < 0)
                  abort ();
                /* We know that the address space size is
                   < try_limit.rlim_cur.  */
                high_bound = try_next;
                break;
              }
            else
              {
                /* We know that the address space size is
                   >= try_limit.rlim_cur.  */
                low_bound = try_next;
              }
          }
        else
          {
            /* Here we expect only EINVAL, not EPERM.  */
            if (errno != EINVAL)
              abort ();
            /* We know that the address space size is
               >= try_limit.rlim_cur.  */
            low_bound = try_next;
          }
      }

    /* Here we know that the address space size is
       >= low_bound and < high_bound.  */
    while (high_bound - low_bound > pagesize)
      {
        struct rlimit try_limit;
        uintptr_t try_next =
          low_bound + (((high_bound - low_bound) / 2) / pagesize) * pagesize;

        /* Here low_bound <= try_next < high_bound.  */
        try_limit.rlim_max = orig_limit.rlim_max;
        try_limit.rlim_cur = try_next;
        if (setrlimit (RLIMIT_AS, &try_limit) == 0)
          {
            /* Allocate a page of memory, to compare the current address space
               size with try_limit.rlim_cur.  */
            void *new_page =
              mmap (NULL, pagesize, PROT_READ | PROT_WRITE, flags, fd, 0);

            if (new_page != (void *)(-1))
              {
                /* The page could be added successfully.  Free it.  */
                if (munmap (new_page, pagesize) < 0)
                  abort ();
                /* We know that the address space size is
                   < try_limit.rlim_cur.  */
                high_bound = try_next;
              }
            else
              {
                /* We know that the address space size is
                   >= try_limit.rlim_cur.  */
                low_bound = try_next;
              }
          }
        else
          {
            /* Here we expect only EINVAL, not EPERM.  */
            if (errno != EINVAL)
              abort ();
            /* We know that the address space size is
               >= try_limit.rlim_cur.  */
            low_bound = try_next;
          }
      }

    result = low_bound;
  }

 done1:
  /* Restore the original rlim_cur value.  */
  if (setrlimit (RLIMIT_AS, &orig_limit) < 0)
    abort ();

 done2:
# if !HAVE_MAP_ANONYMOUS
  close (fd);
# endif
  return result;
}

#else

static uintptr_t
get_rusage_as_via_setrlimit (void)
{
  return 0;
}

#endif


#if VMA_ITERATE_SUPPORTED

static int
vma_iterate_callback (void *data, uintptr_t start, uintptr_t end,
                      unsigned int flags)
{
  uintptr_t *totalp = (uintptr_t *) data;

  *totalp += end - start;
  return 0;
}

static uintptr_t
get_rusage_as_via_iterator (void)
{
  uintptr_t total = 0;

  vma_iterate (vma_iterate_callback, &total);

  return total;
}

#else

static uintptr_t
get_rusage_as_via_iterator (void)
{
  return 0;
}

#endif


uintptr_t
get_rusage_as (void)
{
#if (defined __APPLE__ && defined __MACH__) || defined _AIX || defined __CYGWIN__ || defined __MVS__ || defined __SANITIZE_THREAD__ /* Mac OS X, AIX, Cygwin, z/OS, gcc -fsanitize=thread */
  /* get_rusage_as_via_setrlimit() does not work.
     Prefer get_rusage_as_via_iterator().  */
  return get_rusage_as_via_iterator ();
#elif HAVE_SETRLIMIT && defined RLIMIT_AS && HAVE_SYS_MMAN_H && HAVE_MPROTECT && !defined __HAIKU__
  /* Prefer get_rusage_as_via_setrlimit() if it succeeds,
     because the caller may want to use the result with setrlimit().  */
  uintptr_t result;

  result = get_rusage_as_via_setrlimit ();
  if (result == 0)
    result = get_rusage_as_via_iterator ();
  return result;
#else
  return get_rusage_as_via_iterator ();
#endif
}


#ifdef TEST

#include <stdio.h>

int
main ()
{
  printf ("Initially:           0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_as_via_setrlimit (), get_rusage_as_via_iterator (),
          get_rusage_as ());
  malloc (0x88);
  printf ("After small malloc:  0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_as_via_setrlimit (), get_rusage_as_via_iterator (),
          get_rusage_as ());
  malloc (0x8812);
  printf ("After medium malloc: 0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_as_via_setrlimit (), get_rusage_as_via_iterator (),
          get_rusage_as ());
  malloc (0x281237);
  printf ("After large malloc:  0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_as_via_setrlimit (), get_rusage_as_via_iterator (),
          get_rusage_as ());
  return 0;
}

#endif /* TEST */
