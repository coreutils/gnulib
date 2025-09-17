/* Provide a working getlogin for systems which lack it.

   Copyright (C) 2010-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible, 2010, 2025.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if defined _WIN32 && ! defined __CYGWIN__
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
/* Don't assume that UNICODE is not defined.  */
# undef GetUserName
# define GetUserName GetUserNameA
#endif

#if defined __linux__ || defined __ANDROID__
# include <fcntl.h>
# include <pwd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include "readutmp.h"
#endif

char *
getlogin (void)
{
#if defined _WIN32 && ! defined __CYGWIN__
  /* Native Windows platform.  */
  static char login_name[1024];
  DWORD sz = sizeof (login_name);

  if (GetUserName (login_name, &sz))
    return login_name;
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
                struct passwd *p = getpwuid (uid);
                if (p != NULL && strlen (p->pw_name) < 64)
                  {
                    static char resultbuf[64];
                    strcpy (resultbuf, p->pw_name);
                    close (fd);
                    return resultbuf;
                  }
              }
          }
        close (fd);
      }
  }
  {
    /* Find the tty connected to the current process.  */
    char *tty = ttyname (STDIN_FILENO);
    if (tty != NULL)
      {
        /* Try to see to which user it is allocated, via utmp.  */
        if (strncmp (tty, "/dev/tty", 8) == 0)
          {
            idx_t n;
            STRUCT_UTMP *entries;
            if (read_utmp (UTMP_FILE, &n, &entries, READ_UTMP_USER_PROCESS)
                == 0)
              {
                idx_t i;
                for (i = 0; i < n; i++)
                  if (streq (entries[i].ut_line, tty + 5))
                    {
                      if (strlen (entries[i].ut_user) < 64)
                        {
                          static char resultbuf[64];
                          strcpy (resultbuf, entries[i].ut_user);
                          free (entries);
                          return resultbuf;
                        }
                      break;
                    }
                free (entries);
              }
          }
        /* Fallback for systems which don't maintain an utmp database
           or for ttys that are not recorded in that the utmp database:
           Look at the owner of that tty.  */
        struct stat statbuf;
        if (stat (tty, &statbuf) >= 0)
          {
            uid_t uid = statbuf.st_uid;
            /* Convert the uid to a user name.  */
            struct passwd *p = getpwuid (uid);
            if (p != NULL && strlen (p->pw_name) < 64)
              {
                static char resultbuf[64];
                strcpy (resultbuf, p->pw_name);
                return resultbuf;
              }
          }
      }
  }
#endif
  return NULL;
}
