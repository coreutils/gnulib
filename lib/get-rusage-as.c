/* Getter for RLIMIT_AS.
   Copyright (C) 2011 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

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

   MacOS X:
     a) setrlimit with RLIMIT_AS succeeds but does not really work: The OS
        ignores RLIMIT_AS. mmap() of a page always succeeds, therefore
        get_rusage_as_via_setrlimit() is always 0.
     b) The Mach based API works.

   FreeBSD:
     a) setrlimit with RLIMIT_AS works.
     b) The /proc/self/maps file contains a list of the virtual memory areas.

   NetBSD:
     a) setrlimit with RLIMIT_AS works.
     b) No VMA iteration API exists.

   OpenBSD:
     a) setrlimit exists, but RLIMIT_AS is not defined.
     b) No VMA iteration API exists.

   AIX:
     a) setrlimit with RLIMIT_AS succeeds but does not really work: The OS
        apparently ignores RLIMIT_AS. mmap() of a page always succeeds,
        therefore get_rusage_as_via_setrlimit() is always 0.
     b) No VMA iteration API exists.

   HP-UX:
     a) setrlimit with RLIMIT_AS works.
     b) No VMA iteration API exists.

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
     b) No VMA iteration API exists.

   Cygwin:
     a) setrlimit with RLIMIT_AS always fails when the limit is < 0x80000000.
        get_rusage_as_via_setrlimit() therefore produces a wrong value.
     b) The /proc/$pid/maps file lists only the memory areas belonging to
        the executable and shared libraries, not the anonymous memory.
        But the native Win32 API works.

   mingw:
     a) There is no setrlimit function.
     b) The native Win32 API works.

   BeOS, Haiku:
     a) On BeOS, there is no setrlimit function.
        On Haiku, setrlimit exists. RLIMIT_AS is defined but unsupported.
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

#if defined __sgi || defined __osf__ /* IRIX, OSF/1 */
# include <string.h> /* memcpy */
# include <sys/types.h>
# include <sys/procfs.h> /* PIOC*, prmap_t */
#endif

#if defined __APPLE__ && defined __MACH__ /* MacOS X */
# include <mach/mach.h>
#endif

#if (defined _WIN32 || defined __WIN32__) || defined __CYGWIN__ /* Windows */
# include <windows.h>
#endif

#if defined __BEOS__ /* BeOS */
# include <OS.h>
#endif


#if HAVE_SETRLIMIT && defined RLIMIT_AS && HAVE_SYS_MMAN_H && HAVE_MPROTECT

static inline uintptr_t
get_rusage_as_via_setrlimit (void)
{
  uintptr_t result;

  struct rlimit orig_limit;

# if HAVE_MAP_ANONYMOUS
  const int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  const int fd = -1;
# else /* !HAVE_MAP_ANONYMOUS */
  const int flags = MAP_FILE | MAP_PRIVATE;
  int fd = open ("/dev/zero", O_RDONLY, 0666);
  if (fd < 0)
    return 0;
# endif

  /* Record the original limit.  */
  if (getrlimit (RLIMIT_AS, &orig_limit) < 0)
    {
      result = 0;
      goto done;
    }

  if (orig_limit.rlim_max != RLIM_INFINITY
      && (orig_limit.rlim_cur == RLIM_INFINITY
          || orig_limit.rlim_cur > orig_limit.rlim_max))
    /* We may not be able to restore the current rlim_cur value.
       So bail out.  */
    {
      result = 0;
      goto done;
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
            goto done;
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

  /* Restore the original rlim_cur value.  */
  if (setrlimit (RLIMIT_AS, &orig_limit) < 0)
    abort ();

 done:
# if !HAVE_MAP_ANONYMOUS
  close (fd);
# endif
  return result;
}

#endif


/* Support for reading text files in the /proc file system.  */

#if defined __linux__ || defined __FreeBSD__ /* || defined __CYGWIN__ */

/* Buffered read-only streams.
   We cannot use <stdio.h> here, because fopen() calls malloc(), and a malloc()
   call may call mmap() and thus pre-allocate available memory.  */

struct rofile
  {
    int fd;
    size_t position;
    size_t filled;
    int eof_seen;
    char buffer[1024];
  };

/* Open a read-only file stream.  */
static int
rof_open (struct rofile *rof, const char *filename)
{
  int fd = open (filename, O_RDONLY);
  if (fd < 0)
    return -1;
  rof->fd = fd;
  rof->position = 0;
  rof->filled = 0;
  rof->eof_seen = 0;
  return 0;
}

/* Return the next byte from a read-only file stream without consuming it,
   or -1 at EOF.  */
static int
rof_peekchar (struct rofile *rof)
{
  if (rof->position == rof->filled)
    {
      if (rof->eof_seen)
        return -1;
      else
        for (;;)
          {
            int n = read (rof->fd, rof->buffer, sizeof (rof->buffer));
# ifdef EINTR
            if (n < 0 && errno == EINTR)
              continue;
# endif
            if (n <= 0)
              {
                rof->eof_seen = 1;
                return -1;
              }
            rof->filled = n;
            rof->position = 0;
            break;
          }
    }
  return (unsigned char) rof->buffer[rof->position];
}

/* Return the next byte from a read-only file stream, or -1 at EOF.  */
static int
rof_getchar (struct rofile *rof)
{
  int c = rof_peekchar (rof);
  if (c >= 0)
    rof->position++;
  return c;
}

/* Parse an unsigned hexadecimal number from a read-only file stream.  */
static int
rof_scanf_lx (struct rofile *rof, unsigned long *valuep)
{
  unsigned long value = 0;
  unsigned int numdigits = 0;
  for (;;)
    {
      int c = rof_peekchar (rof);
      if (c >= '0' && c <= '9')
        value = (value << 4) + (c - '0');
      else if (c >= 'A' && c <= 'F')
        value = (value << 4) + (c - 'A' + 10);
      else if (c >= 'a' && c <= 'f')
        value = (value << 4) + (c - 'a' + 10);
      else
        break;
      rof_getchar (rof);
      numdigits++;
    }
  if (numdigits == 0)
    return -1;
  *valuep = value;
  return 0;
}

/* Close a read-only file stream.  */
static void
rof_close (struct rofile *rof)
{
  close (rof->fd);
}

#endif


static inline uintptr_t
get_rusage_as_via_iterator (void)
{
#if defined __linux__ /* || defined __CYGWIN__ */

  struct rofile rof;
  int c;
  unsigned long total;

  /* Open the current process' maps file.  It describes one VMA per line.  */
  if (rof_open (&rof, "/proc/self/maps") < 0)
    return 0;

  total = 0;
  for (;;)
    {
      unsigned long start, end;

      if (!(rof_scanf_lx (&rof, &start) >= 0
            && rof_getchar (&rof) == '-'
            && rof_scanf_lx (&rof, &end) >= 0))
        break;
      while (c = rof_getchar (&rof), c != -1 && c != '\n')
        ;
      total += end - start;
    }
  rof_close (&rof);
  return total;

#elif defined __FreeBSD__

  struct rofile rof;
  int c;
  unsigned long total;

  /* Open the current process' maps file.  It describes one VMA per line.  */
  if (rof_open (&rof, "/proc/curproc/map") < 0)
    return 0;

  total = 0;
  for (;;)
    {
      unsigned long start, end;

      if (!(rof_getchar (&rof) == '0'
            && rof_getchar (&rof) == 'x'
            && rof_scanf_lx (&rof, &start) >= 0))
        break;
      while (c = rof_peekchar (&rof), c == ' ' || c == '\t')
        rof_getchar (&rof);
      if (!(rof_getchar (&rof) == '0'
            && rof_getchar (&rof) == 'x'
            && rof_scanf_lx (&rof, &end) >= 0))
        break;
      while (c = rof_getchar (&rof), c != -1 && c != '\n')
        continue;
      total += end - start;
    }
  rof_close (&rof);
  return total;

#elif defined __sgi || defined __osf__ /* IRIX, OSF/1 */

  size_t pagesize;
  char fnamebuf[6+10+1];
  char *fname;
  int fd;
  int nmaps;
  size_t memneed;
# if HAVE_MAP_ANONYMOUS
#  define zero_fd -1
#  define map_flags MAP_ANONYMOUS
# else
  int zero_fd;
#  define map_flags 0
# endif
  void *auxmap;
  unsigned long auxmap_start;
  unsigned long auxmap_end;
  prmap_t* maps;
  prmap_t* mp;
  unsigned long total;

  pagesize = getpagesize ();

  /* Construct fname = sprintf (fnamebuf+i, "/proc/%u", getpid ()).  */
  fname = fnamebuf + sizeof (fnamebuf) - 1;
  *fname = '\0';
  {
    unsigned int value = getpid ();
    do
      *--fname = (value % 10) + '0';
    while ((value = value / 10) > 0);
  }
  fname -= 6;
  memcpy (fname, "/proc/", 6);

  fd = open (fname, O_RDONLY);
  if (fd < 0)
    return 0;

  if (ioctl (fd, PIOCNMAP, &nmaps) < 0)
    goto fail2;

  memneed = (nmaps + 10) * sizeof (prmap_t);
  /* Allocate memneed bytes of memory.
     We cannot use alloca here, because not much stack space is guaranteed.
     We also cannot use malloc here, because a malloc() call may call mmap()
     and thus pre-allocate available memory.
     So use mmap(), and ignore the resulting VMA.  */
  memneed = ((memneed - 1) / pagesize + 1) * pagesize;
# if !HAVE_MAP_ANONYMOUS
  zero_fd = open ("/dev/zero", O_RDONLY, 0644);
  if (zero_fd < 0)
    goto fail2;
# endif
  auxmap = (void *) mmap ((void *) 0, memneed, PROT_READ | PROT_WRITE,
                          map_flags | MAP_PRIVATE, zero_fd, 0);
# if !HAVE_MAP_ANONYMOUS
  close (zero_fd);
# endif
  if (auxmap == (void *) -1)
    goto fail2;
  auxmap_start = (unsigned long) auxmap;
  auxmap_end = auxmap_start + memneed;
  maps = (prmap_t *) auxmap;

  if (ioctl (fd, PIOCMAP, maps) < 0)
    goto fail1;

  total = 0;
  for (mp = maps;;)
    {
      unsigned long start, end;

      start = (unsigned long) mp->pr_vaddr;
      end = start + mp->pr_size;
      if (start == 0 && end == 0)
        break;
      mp++;
      if (start <= auxmap_start && auxmap_end - 1 <= end - 1)
        /* Consider [start,end-1] \ [auxmap_start,auxmap_end-1]
           = [start,auxmap_start-1] u [auxmap_end,end-1].  */
        total += (end - start) - memneed;
      else
        total += end - start;
    }
  munmap (auxmap, memneed);
  close (fd);
  return total;

 fail1:
  munmap (auxmap, memneed);
 fail2:
  close (fd);
  return 0;

#elif defined __APPLE__ && defined __MACH__ /* MacOS X */

  task_t task = mach_task_self ();
  vm_address_t address;
  vm_size_t size;
  vm_address_t total = 0;

  for (address = VM_MIN_ADDRESS;; address += size)
    {
      int more;
      mach_port_t object_name;
      /* In MacOS X 10.5, the types vm_address_t, vm_offset_t, vm_size_t have
         32 bits in 32-bit processes and 64 bits in 64-bit processes. Whereas
         mach_vm_address_t and mach_vm_size_t are always 64 bits large.
         MacOS X 10.5 has three vm_region like methods:
           - vm_region. It has arguments that depend on whether the current
             process is 32-bit or 64-bit. When linking dynamically, this
             function exists only in 32-bit processes. Therefore we use it only
             in 32-bit processes.
           - vm_region_64. It has arguments that depend on whether the current
             process is 32-bit or 64-bit. It interprets a flavor
             VM_REGION_BASIC_INFO as VM_REGION_BASIC_INFO_64, which is
             dangerous since 'struct vm_region_basic_info_64' is larger than
             'struct vm_region_basic_info'; therefore let's write
             VM_REGION_BASIC_INFO_64 explicitly.
           - mach_vm_region. It has arguments that are 64-bit always. This
             function is useful when you want to access the VM of a process
             other than the current process.
         In 64-bit processes, we could use vm_region_64 or mach_vm_region.
         I choose vm_region_64 because it uses the same types as vm_region,
         resulting in less conditional code.  */
# if defined __ppc64__ || defined __x86_64__
      struct vm_region_basic_info_64 info;
      mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT_64;

      more = (vm_region_64 (task, &address, &size, VM_REGION_BASIC_INFO_64,
                            (vm_region_info_t)&info, &info_count, &object_name)
              == KERN_SUCCESS);
# else
      struct vm_region_basic_info info;
      mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT;

      more = (vm_region (task, &address, &size, VM_REGION_BASIC_INFO,
                         (vm_region_info_t)&info, &info_count, &object_name)
              == KERN_SUCCESS);
# endif
      if (object_name != MACH_PORT_NULL)
        mach_port_deallocate (mach_task_self (), object_name);
      if (!more)
        break;
      total += size;
    }
  return total;

#elif (defined _WIN32 || defined __WIN32__) || defined __CYGWIN__
  /* Windows platform.  Use the native Windows API.  */

  MEMORY_BASIC_INFORMATION info;
  unsigned long address = 0;
  unsigned long total = 0;

  while (VirtualQuery ((void*)address, &info, sizeof(info)) == sizeof(info))
    {
      if (info.State != MEM_FREE)
        total += info.RegionSize;
      address = (unsigned long)info.BaseAddress + info.RegionSize;
    }
  return total;

#elif defined __BEOS__
  /* Use the BeOS specific API.  */

  area_info info;
  int32 cookie;
  unsigned long total = 0;

  cookie = 0;
  while (get_next_area_info (0, &cookie, &info) == B_OK)
    {
      unsigned long start, end;

      start = (unsigned long) info.address;
      end = start + info.size;

      total += end - start;
    }
  return total;

#else

  return 0;

#endif
}


uintptr_t
get_rusage_as (void)
{
#if (defined __APPLE__ && defined __MACH__) || defined _AIX || defined __CYGWIN__ /* MacOS X, AIX, Cygwin */
  /* get_rusage_as_via_setrlimit() does not work.
     Prefer get_rusage_as_via_iterator().  */
  return get_rusage_as_via_iterator ();
#elif HAVE_SETRLIMIT && defined RLIMIT_AS && HAVE_SYS_MMAN_H && HAVE_MPROTECT
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
