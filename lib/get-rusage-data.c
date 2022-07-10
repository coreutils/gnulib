/* Getter for RLIMIT_DATA.
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

/* Specification.  */
#include "resource-ext.h"

/* The "data segment size" is defined as the virtual memory area of the
   current process that contains malloc()ed memory.

   There are two ways of retrieving the current data segment size:
     a) by trying setrlimit with various values and observing whether the
        kernel allows additional sbrk() calls,
     b) by using system dependent APIs that allow to iterate over the list
        of virtual memory areas.
   We define two functions
     get_rusage_data_via_setrlimit(),
     get_rusage_data_via_iterator().

   The variant
     a') by trying setrlimit with various values and observing whether
        additional malloc() calls succeed
   is not as good as a), because a malloc() call can be served by already
   allocated memory or through mmap(), and because a malloc() of 1 page may
   require 2 pages.

   Discussion per platform:

   Linux:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.
     get_rusage_data_via_setrlimit() returns the sum of the length of the
     executable's data segment plus the heap VMA (an anonymous memory area),
     whereas get_rusage_data_via_iterator() returns only the latter.
     Note that malloc() falls back on mmap() for large allocations and also
     for small allocations if there is not enough room in the data segment.

   Mac OS X:
     a) setrlimit with RLIMIT_DATA succeeds but does not really work: The OS
        ignores RLIMIT_DATA. Therefore get_rusage_data_via_setrlimit() is
        always 0.
     b) The Mach based API works.
     Note that malloc() falls back on mmap() for large allocations.

   FreeBSD:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.

   NetBSD:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.
     Both methods agree.
     Note that malloc() uses mmap() for large allocations.

   OpenBSD:
     a) setrlimit with RLIMIT_DATA works.
     b) mquery() can be used to find out about the virtual memory areas.
     get_rusage_data_via_setrlimit() works much better than
     get_rusage_data_via_iterator().
     Note that malloc() appears to use mmap() for both large and small
     allocations.

   AIX:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/$pid/map file contains a list of the virtual memory areas.

   HP-UX:
     a) setrlimit with RLIMIT_DATA works, except on HP-UX 11.00, where it
        cannot restore the previous limits, and except on HP-UX 11.11, where
        it sometimes has no effect.
     b) pstat_getprocvm() can be used to find out about the virtual memory
        areas.
     Both methods agree, except that the value of get_rusage_data_via_iterator()
     is sometimes 4 KB larger than get_rusage_data_via_setrlimit().

   IRIX:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP.
     get_rusage_data_via_setrlimit() works slightly better than
     get_rusage_data_via_iterator() before the first malloc() call.

   OSF/1:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP.
     Both methods agree.

   Solaris:
     a) setrlimit with RLIMIT_DATA works.
     b) The /proc/$pid file supports ioctls PIOCNMAP and PIOCMAP, and the
        /proc/self/maps file contains a list of the virtual memory areas.
     get_rusage_data_via_setrlimit() ignores the data segment of the executable,
     whereas get_rusage_data_via_iterator() includes it.

   Cygwin:
     a) setrlimit with RLIMIT_DATA always fails.
        get_rusage_data_via_setrlimit() therefore produces a wrong value.
     b) The /proc/$pid/maps file lists only the memory areas belonging to
        the executable and shared libraries, not the anonymous memory.
        But the native Windows API works.
     Note that malloc() apparently falls back on mmap() for large allocations.

   mingw:
     a) There is no setrlimit function.
     b) There is no sbrk() function.
     Note that malloc() falls back on VirtualAlloc() for large allocations.

   BeOS, Haiku:
     a) On BeOS, there is no setrlimit function.
        On Haiku, setrlimit exists. RLIMIT_DATA is defined but setrlimit fails.
     b) There is a specific BeOS API: get_next_area_info().
 */


#include <errno.h> /* errno */
#include <stdlib.h> /* size_t, abort, malloc, free, sbrk */
#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* getpagesize, read, close */


/* System support for get_rusage_data_via_setrlimit().  */

#if HAVE_SETRLIMIT
# include <sys/time.h>
# include <sys/resource.h> /* getrlimit, setrlimit */
# include <sys/utsname.h>
# include <string.h> /* strlen, strcmp */
#endif


/* System support for get_rusage_data_via_iterator().  */

#include "vma-iter.h"


#if !(defined __APPLE__ && defined __MACH__) || defined TEST
/* Implement get_rusage_data_via_setrlimit().  */

# if HAVE_SETRLIMIT && defined RLIMIT_DATA && HAVE_SBRK && !defined __HAIKU__

#  ifdef _AIX
#   define errno_expected() (errno == EINVAL || errno == EFAULT)
#  else
#   define errno_expected() (errno == EINVAL)
#  endif

static uintptr_t
get_rusage_data_via_setrlimit (void)
{
  uintptr_t result;

  struct rlimit orig_limit;

#  ifdef __hpux
  /* On HP-UX 11.00, setrlimit() of RLIMIT_DATA does not work: It cannot
     restore the previous limits.
     On HP-UX 11.11, setrlimit() of RLIMIT_DATA does not work: It sometimes
     has no effect on the next sbrk() call.  */
  {
    struct utsname buf;

    if (uname (&buf) == 0
        && strlen (buf.release) >= 5
        && (strcmp (buf.release + strlen (buf.release) - 5, "11.00") == 0
            || strcmp (buf.release + strlen (buf.release) - 5, "11.11") == 0))
      return 0;
  }
#  endif

  /* Record the original limit.  */
  if (getrlimit (RLIMIT_DATA, &orig_limit) < 0)
    return 0;

  if (orig_limit.rlim_max != RLIM_INFINITY
      && (orig_limit.rlim_cur == RLIM_INFINITY
          || orig_limit.rlim_cur > orig_limit.rlim_max))
    /* We may not be able to restore the current rlim_cur value.
       So bail out.  */
    return 0;

  {
    /* The granularity is a single page.  */
    const intptr_t pagesize = getpagesize ();

    uintptr_t low_bound = 0;
    uintptr_t high_bound;

    for (;;)
      {
        /* Here we know that the data segment size is >= low_bound.  */
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
            goto done;
          }

        try_limit.rlim_max = orig_limit.rlim_max;
        try_limit.rlim_cur = try_next;
        if (setrlimit (RLIMIT_DATA, &try_limit) == 0)
          {
            /* Allocate a page of memory, to compare the current data segment
               size with try_limit.rlim_cur.  */
            void *new_page = sbrk (pagesize);

            if (new_page != (void *)(-1))
              {
                /* The page could be added successfully.  Free it.  */
                sbrk (- pagesize);
                /* We know that the data segment size is
                   < try_limit.rlim_cur.  */
                high_bound = try_next;
                break;
              }
            else
              {
                /* We know that the data segment size is
                   >= try_limit.rlim_cur.  */
                low_bound = try_next;
              }
          }
        else
          {
            /* Here we expect only EINVAL or (on AIX) EFAULT, not EPERM.  */
            if (! errno_expected ())
              abort ();
            /* We know that the data segment size is
               >= try_limit.rlim_cur.  */
            low_bound = try_next;
          }
      }

    /* Here we know that the data segment size is
       >= low_bound and < high_bound.  */
    while (high_bound - low_bound > pagesize)
      {
        struct rlimit try_limit;
        uintptr_t try_next =
          low_bound + (((high_bound - low_bound) / 2) / pagesize) * pagesize;

        /* Here low_bound <= try_next < high_bound.  */
        try_limit.rlim_max = orig_limit.rlim_max;
        try_limit.rlim_cur = try_next;
        if (setrlimit (RLIMIT_DATA, &try_limit) == 0)
          {
            /* Allocate a page of memory, to compare the current data segment
               size with try_limit.rlim_cur.  */
            void *new_page = sbrk (pagesize);

            if (new_page != (void *)(-1))
              {
                /* The page could be added successfully.  Free it.  */
                sbrk (- pagesize);
                /* We know that the data segment size is
                   < try_limit.rlim_cur.  */
                high_bound = try_next;
              }
            else
              {
                /* We know that the data segment size is
                   >= try_limit.rlim_cur.  */
                low_bound = try_next;
              }
          }
        else
          {
            /* Here we expect only EINVAL or (on AIX) EFAULT, not EPERM.  */
            if (! errno_expected ())
              abort ();
            /* We know that the data segment size is
               >= try_limit.rlim_cur.  */
            low_bound = try_next;
          }
      }

    result = low_bound;
  }

 done:
  /* Restore the original rlim_cur value.  */
  if (setrlimit (RLIMIT_DATA, &orig_limit) < 0)
    abort ();

  return result;
}

# else

static uintptr_t
get_rusage_data_via_setrlimit (void)
{
  return 0;
}

# endif

#endif


#if !(defined __APPLE__ && defined __MACH__) || defined TEST
/* Implement get_rusage_data_via_iterator().  */

# if VMA_ITERATE_SUPPORTED

struct locals
{
  uintptr_t brk_value;
  uintptr_t data_segment_size;
};

static int
vma_iterate_callback (void *data, uintptr_t start, uintptr_t end,
                      unsigned int flags)
{
  struct locals *lp = (struct locals *) data;

  if (start <= lp->brk_value && lp->brk_value - 1 <= end - 1)
    {
      lp->data_segment_size = end - start;
      return 1;
    }
  return 0;
}

static uintptr_t
get_rusage_data_via_iterator (void)
{
#  if (defined _WIN32 && !defined __CYGWIN__) || defined __BEOS__ || defined __HAIKU__
  /* On native Windows, there is no sbrk() function.
     On Haiku, sbrk(0) always returns 0.  */
  static void *brk_value;

  if (brk_value == NULL)
    {
      brk_value = malloc (1);
      if (brk_value == NULL)
        return 0;
    }
#  else
  void *brk_value;

  brk_value = sbrk (0);
  if (brk_value == (void *)-1)
    return 0;
#  endif

  {
    struct locals l;

    l.brk_value = (uintptr_t) brk_value;
    l.data_segment_size = 0;
    vma_iterate (vma_iterate_callback, &l);

    return l.data_segment_size;
  }
}

# else

static uintptr_t
get_rusage_data_via_iterator (void)
{
  return 0;
}

# endif

#endif


uintptr_t
get_rusage_data (void)
{
#if (defined __APPLE__ && defined __MACH__) /* Mac OS X */
  /* get_rusage_data_via_setrlimit() does not work: it always returns 0.
     get_rusage_data_via_iterator() does not work: it always returns 0x400000.
     And sbrk() is deprecated.  */
  return 0;
#elif defined __minix /* Minix */
  /* get_rusage_data_via_setrlimit() does not work: it always returns 0.
     get_rusage_data_via_iterator() does not work: it shrinks upon malloc. */
  return 0;
#elif defined __CYGWIN__ /* Cygwin */
  /* get_rusage_data_via_setrlimit() does not work.
     Prefer get_rusage_data_via_iterator().  */
  return get_rusage_data_via_iterator ();
#elif HAVE_SETRLIMIT && defined RLIMIT_DATA && !defined __HAIKU__
# if defined __linux__ || defined __ANDROID__ || defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined _AIX || defined __hpux || defined __sgi || defined __osf__ || defined __sun /* Linux, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris */
  /* get_rusage_data_via_setrlimit() works.  */
  return get_rusage_data_via_setrlimit ();
# else
  /* Prefer get_rusage_data_via_setrlimit() if it succeeds,
     because the caller may want to use the result with setrlimit().  */
  uintptr_t result;

  result = get_rusage_data_via_setrlimit ();
  if (result == 0)
    result = get_rusage_data_via_iterator ();
  return result;
# endif
#else
  return get_rusage_data_via_iterator ();
#endif
}


#ifdef TEST

#include <stdio.h>

int
main ()
{
  printf ("Initially:           0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_data_via_setrlimit (), get_rusage_data_via_iterator (),
          get_rusage_data ());
  malloc (0x88);
  printf ("After small malloc:  0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_data_via_setrlimit (), get_rusage_data_via_iterator (),
          get_rusage_data ());
  malloc (0x8812);
  printf ("After medium malloc: 0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_data_via_setrlimit (), get_rusage_data_via_iterator (),
          get_rusage_data ());
  malloc (0x281237);
  printf ("After large malloc:  0x%08lX 0x%08lX 0x%08lX\n",
          get_rusage_data_via_setrlimit (), get_rusage_data_via_iterator (),
          get_rusage_data ());
  return 0;
}

#endif /* TEST */
