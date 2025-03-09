/* Provide a working getlogin_r for systems which lack it.

   Copyright (C) 2005-2007, 2010-2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert, Derek Price, and Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include "malloca.h"

#if defined _WIN32 && ! defined __CYGWIN__
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
/* Don't assume that UNICODE is not defined.  */
# undef GetUserName
# define GetUserName GetUserNameA
#else
# if !HAVE_DECL_GETLOGIN
extern char *getlogin (void);
# endif
#endif

#if defined __linux__ || defined __ANDROID__
# include <fcntl.h>
# include <pwd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
#endif

/* See unistd.in.h for documentation.  */
int
getlogin_r (char *name, size_t size)
{
#undef getlogin_r
#if defined _WIN32 && ! defined __CYGWIN__
  /* Native Windows platform.  */
  DWORD sz;

  /* When size > 0x7fff, the doc says that GetUserName will fail.
     Actually, on Windows XP SP3, it succeeds.  But let's be safe,
     for the sake of older Windows versions.  */
  if (size > 0x7fff)
    size = 0x7fff;
  sz = size;
  if (!GetUserName (name, &sz))
    {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
        /* In this case, the doc says that sz contains the required size, but
           actually, on Windows XP SP3, it contains 2 * the required size.  */
        return ERANGE;
      else
        return ENOENT;
    }
  return 0;
#elif defined __linux__ || defined __ANDROID__
  /* Linux.  */
  {
    /* Read the login uid from the /proc file system.  */
    int fd = open ("/proc/self/loginuid", O_RDONLY);
    if (fd >= 0)
      {
        char buf[20 + 1];
        int n = read (fd, buf, sizeof (buf) - 1);
        if (n > 0)
          {
            buf[n] = '\0';
            char *endptr;
            unsigned long uid = strtoul (buf, &endptr, 10);
            if (endptr == buf + n && uid != (uid_t) -1)
              {
                /* Convert the uid to a user name.  */
                struct passwd pbuf1;
                char pbuf2[1024];
                struct passwd *p;
                if (getpwuid_r (uid, &pbuf1, pbuf2, sizeof (pbuf2), &p) == 0)
                  {
                    if (strlen (p->pw_name) < size)
                      {
                        strcpy (name, p->pw_name);
                        close (fd);
                        return 0;
                      }
                    return ERANGE;
                  }
              }
          }
        close (fd);
      }
  }
  {
    /* Find the tty connected to the current process.  */
    char tty[1024];
    if (ttyname_r (STDIN_FILENO, tty, sizeof (tty)) == 0)
      {
        /* We cannot use read_utmp here, since it is not multithread-safe.  */
        /* Fallback for systems which don't maintain an utmp database
           or for ttys that are not recorded in that the utmp database:
           Look at the owner of that tty.  */
        struct stat statbuf;
        if (stat (tty, &statbuf) >= 0)
          {
            uid_t uid = statbuf.st_uid;
            /* Convert the uid to a user name.  */
            struct passwd pbuf1;
            char pbuf2[1024];
            struct passwd *p;
            if (getpwuid_r (uid, &pbuf1, pbuf2, sizeof (pbuf2), &p) == 0)
              {
                if (strlen (p->pw_name) < size)
                  {
                    strcpy (name, p->pw_name);
                    return 0;
                  }
                return ERANGE;
              }
          }
      }
  }
  /* ENOENT is a reasonable errno value if getlogin returns NULL.  */
  return ENOENT;
#elif HAVE_GETLOGIN_R
  /* Platform with a getlogin_r() function.  */
  int ret = getlogin_r (name, size);

  if (ret == 0)
    {
      const char *nul = memchr (name, '\0', size);
      if (nul == NULL)
        /* name contains a truncated result.  */
        return ERANGE;
      if (size > 0 && nul == name + size - 1)
        {
          /* strlen(name) == size-1.  Determine whether the untruncated result
             would have had length size-1 or size.  */
          char *room = (char *) malloca (size + 1);
          if (room == NULL)
            return ENOMEM;
          ret = getlogin_r (room, size + 1);
          /* The untruncated result should be the same as in the first call.  */
          if (ret == 0 && memcmp (name, room, size) != 0)
            /* The untruncated result would have been different.  */
            ret = ERANGE;
          freea (room);
        }
    }
  return ret;
#else
  /* Platform with a getlogin() function.  */
  char *n;
  size_t nlen;

  errno = 0;
  n = getlogin ();
  if (!n)
    /* ENOENT is a reasonable errno value if getlogin returns NULL.  */
    return (errno != 0 ? errno : ENOENT);

  nlen = strlen (n);
  if (size <= nlen)
    return ERANGE;
  memcpy (name, n, nlen + 1);
  return 0;
#endif
}
