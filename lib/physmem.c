/* Calculate the size of physical memory.

   Copyright (C) 2000-2001, 2003, 2005-2006, 2009-2025 Free Software
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

/* Written by Paul Eggert.  */

#include <config.h>

#include "physmem.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#if HAVE_SYS_PSTAT_H /* HP-UX */
# include <sys/pstat.h>
#endif

#if HAVE_SYS_SYSINFO_H
/* Linux, AIX, HP-UX, Solaris, Cygwin, Android */
# include <sys/sysinfo.h>
#endif

#include <sys/types.h>

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#if HAVE_SYS_SYSCTL_H && !(defined __GLIBC__ && defined __linux__)
/* Linux/musl, macOS, *BSD, Minix */
# include <sys/sysctl.h>
#endif

#if HAVE_SYS_SYSTEMCFG_H /* AIX */
# include <sys/systemcfg.h>
#endif

#include "full-read.h"

#ifdef _WIN32

# define WIN32_LEAN_AND_MEAN
# include <windows.h>

/* Don't assume that UNICODE is not defined.  */
# undef GetModuleHandle
# define GetModuleHandle GetModuleHandleA

/* Avoid warnings from gcc -Wcast-function-type.  */
# define GetProcAddress \
   (void *) GetProcAddress

/*  MEMORYSTATUSEX is missing from older windows headers, so define
    a local replacement.  */
typedef struct
{
  DWORD dwLength;
  DWORD dwMemoryLoad;
  DWORDLONG ullTotalPhys;
  DWORDLONG ullAvailPhys;
  DWORDLONG ullTotalPageFile;
  DWORDLONG ullAvailPageFile;
  DWORDLONG ullTotalVirtual;
  DWORDLONG ullAvailVirtual;
  DWORDLONG ullAvailExtendedVirtual;
} lMEMORYSTATUSEX;
typedef BOOL (WINAPI *PFN_MS_EX) (lMEMORYSTATUSEX*);

#endif

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))

/* Return the total amount of physical memory.  */
double
physmem_total (void)
{
#if defined _SC_PHYS_PAGES && defined _SC_PAGESIZE
  { /* This works on linux-gnu, kfreebsd-gnu, solaris2, and cygwin.  */
    double pages = sysconf (_SC_PHYS_PAGES);
    double pagesize = sysconf (_SC_PAGESIZE);
    if (0 <= pages && 0 <= pagesize)
      return pages * pagesize;
  }
#endif

#if HAVE_SYSINFO && HAVE_STRUCT_SYSINFO_MEM_UNIT
  { /* This works on linux.  */
    struct sysinfo si;
    if (sysinfo(&si) == 0)
      return (double) si.totalram * si.mem_unit;
  }
#endif

#if HAVE_PSTAT_GETSTATIC
  { /* This works on hpux11.  */
    struct pst_static pss;
    if (0 <= pstat_getstatic (&pss, sizeof pss, 1, 0))
      {
        double pages = pss.physical_memory;
        double pagesize = pss.page_size;
        if (0 <= pages && 0 <= pagesize)
          return pages * pagesize;
      }
  }
#endif

#if HAVE_SYSCTL && !(defined __GLIBC__ && defined __linux__) && defined HW_PHYSMEM
  { /* This works on *bsd, kfreebsd-gnu, and darwin.  */
    unsigned int physmem;
    size_t len = sizeof physmem;
    static int mib[2] = { CTL_HW, HW_PHYSMEM };

    if (sysctl (mib, ARRAY_SIZE (mib), &physmem, &len, NULL, 0) == 0
        && len == sizeof (physmem))
      return (double) physmem;
  }
#endif

#if HAVE__SYSTEM_CONFIGURATION
  /* This works on AIX.  */
  return _system_configuration.physmem;
#endif

#if defined _WIN32
  { /* this works on windows */
    PFN_MS_EX pfnex;
    HMODULE h = GetModuleHandle ("kernel32.dll");

    if (!h)
      return 0.0;

    /*  Use GlobalMemoryStatusEx if available.  */
    if ((pfnex = (PFN_MS_EX) GetProcAddress (h, "GlobalMemoryStatusEx")))
      {
        lMEMORYSTATUSEX lms_ex;
        lms_ex.dwLength = sizeof lms_ex;
        if (!pfnex (&lms_ex))
          return 0.0;
        return (double) lms_ex.ullTotalPhys;
      }

    /*  Fall back to GlobalMemoryStatus which is always available.
        but returns wrong results for physical memory > 4GB.  */
    else
      {
        MEMORYSTATUS ms;
        GlobalMemoryStatus (&ms);
        return (double) ms.dwTotalPhys;
      }
  }
#endif

  /* Guess 64 MB.  It's probably an older host, so guess small.  */
  return 64 * 1024 * 1024;
}

#if defined __linux__

/* Get the amount of free memory and of inactive file cache memory, and
   return 0.  Upon failure, return -1.  */
static int
get_meminfo (unsigned long long *mem_free_p,
             unsigned long long *mem_inactive_file_p)
{
  /* While the sysinfo() system call returns mem_total, mem_free, and a few
     other numbers, the only way to get mem_inactive_file is by reading
     /proc/meminfo.  */
  int fd = open ("/proc/meminfo", O_RDONLY);
  if (fd >= 0)
    {
      char buf[4096];
      size_t buf_size = full_read (fd, buf, sizeof (buf));
      close (fd);
      if (buf_size > 0)
        {
          char *buf_end = buf + buf_size;
          unsigned long long mem_free = 0;
          unsigned long long mem_inactive_file = 0;

          /* Iterate through the lines.  */
          char *line = buf;
          for (;;)
            {
              char *p;
              for (p = line; p < buf_end; p++)
                if (*p == '\n')
                  break;
              if (p == buf_end)
                break;
              *p = '\0';
              if (sscanf (line, "MemFree: %llu kB", &mem_free) == 1)
                {
                  mem_free *= 1024;
                }
              if (sscanf (line, "Inactive(file): %llu kB", &mem_inactive_file) == 1)
                {
                  mem_inactive_file *= 1024;
                }
              line = p + 1;
            }
          if (mem_free > 0 && mem_inactive_file > 0)
            {
              *mem_free_p = mem_free;
              *mem_inactive_file_p = mem_inactive_file;
              return 0;
            }
        }
    }
  return -1;
}

#endif

/* Return the amount of physical memory that can be claimed, with a given
   aggressivity.  */
double
physmem_claimable (double aggressivity)
{
#if defined _SC_AVPHYS_PAGES && defined _SC_PAGESIZE
# if defined __linux__
  /* On Linux, sysconf (_SC_AVPHYS_PAGES) returns the amount of "free" memory.
     The Linux memory management system attempts to keep only a small amount
     of memory (something like 5% to 10%) as free, because memory is better
     used in the file cache.
     We compute the "claimable" memory as
       (free memory) + aggressivity * (inactive memory in the file cache).  */
  if (aggressivity > 0.0)
    {
      unsigned long long mem_free;
      unsigned long long mem_inactive_file;
      if (get_meminfo (&mem_free, &mem_inactive_file) == 0)
        return (double) mem_free + aggressivity * (double) mem_inactive_file;
    }
# endif
  { /* This works on linux-gnu, kfreebsd-gnu, solaris2, and cygwin.  */
    double pages = sysconf (_SC_AVPHYS_PAGES);
    double pagesize = sysconf (_SC_PAGESIZE);
    if (0 <= pages && 0 <= pagesize)
      return pages * pagesize;
  }
#endif

#if HAVE_SYSINFO && HAVE_STRUCT_SYSINFO_MEM_UNIT
  { /* This works on linux.  */
    struct sysinfo si;
    if (sysinfo(&si) == 0)
      return ((double) si.freeram + si.bufferram) * si.mem_unit;
  }
#endif

#if HAVE_PSTAT_GETSTATIC && HAVE_PSTAT_GETDYNAMIC
  { /* This works on hpux11.  */
    struct pst_static pss;
    struct pst_dynamic psd;
    if (0 <= pstat_getstatic (&pss, sizeof pss, 1, 0)
        && 0 <= pstat_getdynamic (&psd, sizeof psd, 1, 0))
      {
        double pages = psd.psd_free;
        double pagesize = pss.page_size;
        if (0 <= pages && 0 <= pagesize)
          return pages * pagesize;
      }
  }
#endif

#if HAVE_SYSCTL && !(defined __GLIBC__ && defined __linux__) && defined HW_USERMEM
  { /* This works on *bsd, kfreebsd-gnu, and darwin.  */
    unsigned int usermem;
    size_t len = sizeof usermem;
    static int mib[2] = { CTL_HW, HW_USERMEM };

    if (sysctl (mib, ARRAY_SIZE (mib), &usermem, &len, NULL, 0) == 0
        && len == sizeof (usermem))
      return (double) usermem;
  }
#endif

#if defined _WIN32
  { /* this works on windows */
    PFN_MS_EX pfnex;
    HMODULE h = GetModuleHandle ("kernel32.dll");

    if (!h)
      return 0.0;

    /*  Use GlobalMemoryStatusEx if available.  */
    if ((pfnex = (PFN_MS_EX) GetProcAddress (h, "GlobalMemoryStatusEx")))
      {
        lMEMORYSTATUSEX lms_ex;
        lms_ex.dwLength = sizeof lms_ex;
        if (!pfnex (&lms_ex))
          return 0.0;
        return (double) lms_ex.ullAvailPhys;
      }

    /*  Fall back to GlobalMemoryStatus which is always available.
        but returns wrong results for physical memory > 4GB  */
    else
      {
        MEMORYSTATUS ms;
        GlobalMemoryStatus (&ms);
        return (double) ms.dwAvailPhys;
      }
  }
#endif

  /* Guess 25% of physical memory.  */
  return physmem_total () / 4;
}

/* Return the amount of physical memory available.  */
double
physmem_available (void)
{
  return physmem_claimable (0.0);
}

#if DEBUG

# include <stdio.h>
# include <stdlib.h>

int
main (void)
{
  printf ("%12.f %12.f\n", physmem_total (), physmem_available ());
  exit (0);
}

#endif /* DEBUG */

/*
Local Variables:
compile-command: "gcc -DDEBUG -g -O -Wall -W physmem.c"
End:
*/
