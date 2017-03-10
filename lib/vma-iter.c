/* Iteration over virtual memory areas.
   Copyright (C) 2011-2017 Free Software Foundation, Inc.
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
#include "vma-iter.h"

#include <errno.h> /* errno */
#include <stdlib.h> /* size_t */
#include <fcntl.h> /* open, O_RDONLY */
#include <unistd.h> /* getpagesize, read, close */

#if defined __sgi || defined __osf__ /* IRIX, OSF/1 */
# include <string.h> /* memcpy */
# include <sys/types.h>
# include <sys/mman.h> /* mmap, munmap */
# include <sys/procfs.h> /* PIOC*, prmap_t */
#endif

#if defined __APPLE__ && defined __MACH__ /* Mac OS X */
# include <mach/mach.h>
#endif

#if (defined _WIN32 || defined __WIN32__) || defined __CYGWIN__ /* Windows */
# include <windows.h>
#endif

#if defined __BEOS__ || defined __HAIKU__ /* BeOS, Haiku */
# include <OS.h>
#endif

#if HAVE_MQUERY /* OpenBSD */
# include <sys/types.h>
# include <sys/mman.h> /* mquery */
#endif


/* Support for reading text files in the /proc file system.  */

#if defined __linux__ || defined __FreeBSD__ || defined __NetBSD__ /* || defined __CYGWIN__ */

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


int
vma_iterate (vma_iterate_callback_fn callback, void *data)
{
#if defined __linux__ /* || defined __CYGWIN__ */

  struct rofile rof;
  int c;

  /* Open the current process' maps file.  It describes one VMA per line.  */
  if (rof_open (&rof, "/proc/self/maps") < 0)
    return -1;

  for (;;)
    {
      unsigned long start, end;
      unsigned int flags;

      /* Parse one line.  First start and end.  */
      if (!(rof_scanf_lx (&rof, &start) >= 0
            && rof_getchar (&rof) == '-'
            && rof_scanf_lx (&rof, &end) >= 0))
        break;
      /* Then the flags.  */
      do
        c = rof_getchar (&rof);
      while (c == ' ');
      flags = 0;
      if (c == 'r')
        flags |= VMA_PROT_READ;
      c = rof_getchar (&rof);
      if (c == 'w')
        flags |= VMA_PROT_WRITE;
      c = rof_getchar (&rof);
      if (c == 'x')
        flags |= VMA_PROT_EXECUTE;
      while (c = rof_getchar (&rof), c != -1 && c != '\n')
        ;

      if (callback (data, start, end, flags))
        break;
    }
  rof_close (&rof);
  return 0;

#elif defined __FreeBSD__ || defined __NetBSD__

  struct rofile rof;
  int c;

  /* Open the current process' maps file.  It describes one VMA per line.  */
  if (rof_open (&rof, "/proc/curproc/map") < 0)
    return -1;

  for (;;)
    {
      unsigned long start, end;
      unsigned int flags;

      /* Parse one line.  First start.  */
      if (!(rof_getchar (&rof) == '0'
            && rof_getchar (&rof) == 'x'
            && rof_scanf_lx (&rof, &start) >= 0))
        break;
      while (c = rof_peekchar (&rof), c == ' ' || c == '\t')
        rof_getchar (&rof);
      /* Then end.  */
      if (!(rof_getchar (&rof) == '0'
            && rof_getchar (&rof) == 'x'
            && rof_scanf_lx (&rof, &end) >= 0))
        break;
      /* Then the flags.  */
      do
        c = rof_getchar (&rof);
      while (c == ' ');
      flags = 0;
      if (c == 'r')
        flags |= VMA_PROT_READ;
      c = rof_getchar (&rof);
      if (c == 'w')
        flags |= VMA_PROT_WRITE;
      c = rof_getchar (&rof);
      if (c == 'x')
        flags |= VMA_PROT_EXECUTE;
      while (c = rof_getchar (&rof), c != -1 && c != '\n')
        ;

      if (callback (data, start, end, flags))
        break;
    }
  rof_close (&rof);
  return 0;

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
    return -1;

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

  for (mp = maps;;)
    {
      unsigned long start, end;
      unsigned int flags;

      start = (unsigned long) mp->pr_vaddr;
      end = start + mp->pr_size;
      if (start == 0 && end == 0)
        break;
      flags = 0;
      if (mp->pr_mflags & MA_READ)
        flags |= VMA_PROT_READ;
      if (mp->pr_mflags & MA_WRITE)
        flags |= VMA_PROT_WRITE;
      if (mp->pr_mflags & MA_EXEC)
        flags |= VMA_PROT_EXECUTE;
      mp++;
      if (start <= auxmap_start && auxmap_end - 1 <= end - 1)
        {
          /* Consider [start,end-1] \ [auxmap_start,auxmap_end-1]
             = [start,auxmap_start-1] u [auxmap_end,end-1].  */
          if (start < auxmap_start)
            if (callback (data, start, auxmap_start, flags))
              break;
          if (auxmap_end - 1 < end - 1)
            if (callback (data, auxmap_end, end, flags))
              break;
        }
      else
        {
          if (callback (data, start, end, flags))
            break;
        }
    }
  munmap (auxmap, memneed);
  close (fd);
  return 0;

 fail1:
  munmap (auxmap, memneed);
 fail2:
  close (fd);
  return -1;

#elif defined __APPLE__ && defined __MACH__ /* Mac OS X */

  task_t task = mach_task_self ();
  vm_address_t address;
  vm_size_t size;

  for (address = VM_MIN_ADDRESS;; address += size)
    {
      int more;
      mach_port_t object_name;
      unsigned int flags;
      /* In Mac OS X 10.5, the types vm_address_t, vm_offset_t, vm_size_t have
         32 bits in 32-bit processes and 64 bits in 64-bit processes. Whereas
         mach_vm_address_t and mach_vm_size_t are always 64 bits large.
         Mac OS X 10.5 has three vm_region like methods:
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
      flags = 0;
      if (info.protection & VM_PROT_READ)
        flags |= VMA_PROT_READ;
      if (info.protection & VM_PROT_WRITE)
        flags |= VMA_PROT_WRITE;
      if (info.protection & VM_PROT_EXECUTE)
        flags |= VMA_PROT_EXECUTE;
      if (callback (data, address, address + size, flags))
        break;
    }
  return 0;

#elif (defined _WIN32 || defined __WIN32__) || defined __CYGWIN__
  /* Windows platform.  Use the native Windows API.  */

  MEMORY_BASIC_INFORMATION info;
  uintptr_t address = 0;

  while (VirtualQuery ((void*)address, &info, sizeof(info)) == sizeof(info))
    {
      if (info.State != MEM_FREE)
        /* Ignore areas where info.State has the value MEM_RESERVE or,
           equivalently, info.Protect has the undocumented value 0.
           This is needed, so that on Cygwin, areas used by malloc() are
           distinguished from areas reserved for future malloc().  */
        if (info.State != MEM_RESERVE)
          {
            uintptr_t start, end;
            unsigned int flags;

            start = (uintptr_t)info.BaseAddress;
            end = start + info.RegionSize;
            switch (info.Protect & ~(PAGE_GUARD|PAGE_NOCACHE))
              {
              case PAGE_READONLY:
                flags = VMA_PROT_READ;
                break;
              case PAGE_READWRITE:
              case PAGE_WRITECOPY:
                flags = VMA_PROT_READ | VMA_PROT_WRITE;
                break;
              case PAGE_EXECUTE:
                flags = VMA_PROT_EXECUTE;
                break;
              case PAGE_EXECUTE_READ:
                flags = VMA_PROT_READ | VMA_PROT_EXECUTE;
                break;
              case PAGE_EXECUTE_READWRITE:
              case PAGE_EXECUTE_WRITECOPY:
                flags = VMA_PROT_READ | VMA_PROT_WRITE | VMA_PROT_EXECUTE;
                break;
              case PAGE_NOACCESS:
              default:
                flags = 0;
                break;
              }

            if (callback (data, start, end, flags))
              break;
          }
      address = (uintptr_t)info.BaseAddress + info.RegionSize;
    }
  return 0;

#elif defined __BEOS__ || defined __HAIKU__
  /* Use the BeOS specific API.  */

  area_info info;
  int32 cookie;

  cookie = 0;
  while (get_next_area_info (0, &cookie, &info) == B_OK)
    {
      unsigned long start, end;
      unsigned int flags;

      start = (unsigned long) info.address;
      end = start + info.size;
      flags = 0;
      if (info.protection & B_READ_AREA)
        flags |= VMA_PROT_READ | VMA_PROT_EXECUTE;
      if (info.protection & B_WRITE_AREA)
        flags |= VMA_PROT_WRITE;

      if (callback (data, start, end, flags))
        break;
    }
  return 0;

#elif HAVE_MQUERY /* OpenBSD */

  uintptr_t pagesize;
  uintptr_t address;
  int /*bool*/ address_known_mapped;

  pagesize = getpagesize ();
  /* Avoid calling mquery with a NULL first argument, because this argument
     value has a specific meaning.  We know the NULL page is unmapped.  */
  address = pagesize;
  address_known_mapped = 0;
  for (;;)
    {
      /* Test whether the page at address is mapped.  */
      if (address_known_mapped
          || mquery ((void *) address, pagesize, 0, MAP_FIXED, -1, 0)
             == (void *) -1)
        {
          /* The page at address is mapped.
             This is the start of an interval.  */
          uintptr_t start = address;
          uintptr_t end;

          /* Find the end of the interval.  */
          end = (uintptr_t) mquery ((void *) address, pagesize, 0, 0, -1, 0);
          if (end == (uintptr_t) (void *) -1)
            end = 0; /* wrap around */
          address = end;

          /* It's too complicated to find out about the flags.  Just pass 0.  */
          if (callback (data, start, end, 0))
            break;

          if (address < pagesize) /* wrap around? */
            break;
        }
      /* Here we know that the page at address is unmapped.  */
      {
        uintptr_t query_size = pagesize;

        address += pagesize;

        /* Query larger and larger blocks, to get through the unmapped address
           range with few mquery() calls.  */
        for (;;)
          {
            if (2 * query_size > query_size)
              query_size = 2 * query_size;
            if (address + query_size - 1 < query_size) /* wrap around? */
              {
                address_known_mapped = 0;
                break;
              }
            if (mquery ((void *) address, query_size, 0, MAP_FIXED, -1, 0)
                == (void *) -1)
              {
                /* Not all the interval [address .. address + query_size - 1]
                   is unmapped.  */
                address_known_mapped = (query_size == pagesize);
                break;
              }
            /* The interval [address .. address + query_size - 1] is
               unmapped.  */
            address += query_size;
          }
        /* Reduce the query size again, to determine the precise size of the
           unmapped interval that starts at address.  */
        while (query_size > pagesize)
          {
            query_size = query_size / 2;
            if (address + query_size - 1 >= query_size)
              {
                if (mquery ((void *) address, query_size, 0, MAP_FIXED, -1, 0)
                    != (void *) -1)
                  {
                    /* The interval [address .. address + query_size - 1] is
                       unmapped.  */
                    address += query_size;
                    address_known_mapped = 0;
                  }
                else
                  address_known_mapped = (query_size == pagesize);
              }
          }
        /* Here again query_size = pagesize, and
           either address + pagesize - 1 < pagesize, or
           mquery ((void *) address, pagesize, 0, MAP_FIXED, -1, 0) fails.
           So, the unmapped area ends at address.  */
      }
      if (address + pagesize - 1 < pagesize) /* wrap around? */
        break;
    }
  return 0;

#else

  /* Not implemented.  */
  return -1;

#endif
}


#ifdef TEST

#include <stdio.h>

/* Output the VMAs of the current process in a format similar to the Linux
   /proc/$pid/maps file.  */

static int
vma_iterate_callback (void *data, uintptr_t start, uintptr_t end,
                      unsigned int flags)
{
  printf ("%08lx-%08lx %c%c%c\n",
          (unsigned long) start, (unsigned long) end,
          flags & VMA_PROT_READ ? 'r' : '-',
          flags & VMA_PROT_WRITE ? 'w' : '-',
          flags & VMA_PROT_EXECUTE ? 'x' : '-');
  return 0;
}

int
main ()
{
  vma_iterate (vma_iterate_callback, NULL);

  /* Let the user interactively look at the /proc file system.  */
  sleep (10);

  return 0;
}

#endif /* TEST */
