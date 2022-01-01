/* sethostname emulation for glibc compliance.

   Copyright (C) 2011-2022 Free Software Foundation, Inc.

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

/* Ben Walton <bwalton@artsci.utoronto.ca> */

#include <config.h>

#if !(defined _WIN32 || defined __CYGWIN__)
/* Unix API.  */

/* Specification.  */
# include <unistd.h>

# include <errno.h>
# include <stdio.h>
# include <limits.h>

/* Set up to LEN chars of NAME as system hostname.
   Return 0 if ok, set errno and return -1 on error. */

int
sethostname (const char *name, size_t len)
{
  /* Ensure the string isn't too long.  glibc does allow setting an
     empty hostname so no point in enforcing a lower bound. */
  if (len > HOST_NAME_MAX)
    {
      errno = EINVAL;
      return -1;
    }

# ifdef __minix /* Minix */
  {
    FILE *hostf;
    int r = 0;

    /* glibc returns EFAULT, EINVAL, and EPERM on error.  None of
       these are appropriate for us to set, even if they may match the
       situation, during failed open/write/close operations, so we
       leave errno alone and rely on what the system sets up. */
    hostf = fopen ("/etc/hostname.file", "we");
    if (hostf == NULL)
      r = -1;
    else
      {
        fprintf (hostf, "%.*s\n", (int) len, name);
        if (ferror (hostf))
          {
            /* Close hostf, preserving the errno from the fprintf call.  */
            int saved_errno = errno;
            fclose (hostf);
            errno = saved_errno;
            r = -1;
          }
        else
          {
            if (fclose (hostf))
              /* fclose sets errno on failure.  */
              r = -1;
          }
      }

    return r;
  }
# else
  /* For platforms that we don't have a better option for, simply bail
     out.  */
  errno = ENOSYS;
  return -1;
# endif
}

#else
/* Native Windows API.  Also used on Cygwin.  */

/* Ensure that <windows.h> declares SetComputerNameEx.  */
# if !defined _WIN32_WINNT || (_WIN32_WINNT < _WIN32_WINNT_WIN2K)
#  undef _WIN32_WINNT
#  define _WIN32_WINNT _WIN32_WINNT_WIN2K
# endif

# define WIN32_LEAN_AND_MEAN

/* Specification.  */
# include <unistd.h>

# include <errno.h>
# include <limits.h>
# include <string.h>

# include <windows.h>

/* Don't assume that UNICODE is not defined.  */
# undef GetComputerNameEx
# define GetComputerNameEx GetComputerNameExA
# undef SetComputerNameEx
# define SetComputerNameEx SetComputerNameExA

/* Set up to LEN chars of NAME as system hostname.
   Return 0 if ok, set errno and return -1 on error. */

int
sethostname (const char *name, size_t len)
{
  char name_asciz[HOST_NAME_MAX + 1];
  char old_name[HOST_NAME_MAX + 1];
  DWORD old_name_len;

  /* Ensure the string isn't too long.  glibc does allow setting an
     empty hostname so no point in enforcing a lower bound. */
  if (len > HOST_NAME_MAX)
    {
      errno = EINVAL;
      return -1;
    }

  /* Prepare a NUL-terminated copy of name.  */
  memcpy (name_asciz, name, len);
  name_asciz[len] = '\0';

  /* Save the old NetBIOS name.  */
  old_name_len = sizeof (old_name) - 1;
  if (! GetComputerNameEx (ComputerNamePhysicalNetBIOS,
                           old_name, &old_name_len))
    old_name_len = 0;

  /* Set both the NetBIOS and the first part of the IP / DNS name.  */
  if (! SetComputerNameEx (ComputerNamePhysicalNetBIOS, name_asciz))
    {
      errno = (GetLastError () == ERROR_ACCESS_DENIED ? EPERM : EINVAL);
      return -1;
    }
  if (! SetComputerNameEx (ComputerNamePhysicalDnsHostname, name_asciz))
    {
      errno = (GetLastError () == ERROR_ACCESS_DENIED ? EPERM : EINVAL);
      /* Restore the old NetBIOS name.  */
      if (old_name_len > 0)
        {
          old_name[old_name_len] = '\0';
          SetComputerNameEx (ComputerNamePhysicalNetBIOS, old_name);
        }
      return -1;
    }

  /* Note that the new host name becomes effective only after a reboot!  */
  return 0;
}

#endif
