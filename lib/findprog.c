/* Locating a program in PATH.
   Copyright (C) 2001-2004, 2006-2026 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

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


#include <config.h>

/* Specification.  */
#include "findprog.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if !(defined _WIN32 || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__)
# include <sys/stat.h>
#endif
#ifdef __ANDROID__
# include <paths.h>
#endif

/* Avoid collision between findprog.c and findprog-lgpl.c.  */
#if IN_FINDPROG_LGPL || ! GNULIB_FINDPROG_LGPL

#if !IN_FINDPROG_LGPL
# include "xalloc.h"
#endif
#include "concat-filename.h"


/* A buffer of size 256 should be sufficient, because the default PATH has
   length < 100 on all systems seen so far:
   - Linux, Hurd, macOS, Cygwin:
     "/bin:/usr/bin"
   - FreeBSD:
     "/usr/bin:/bin:/usr/sbin:/sbin"
   - NetBSD:
     "/usr/bin:/bin:/usr/sbin:/sbin:/usr/pkg/bin:/usr/pkg/sbin:/usr/local/bin:/usr/local/sbin"
   - OpenBSD:
     "/usr/bin:/bin:/usr/sbin:/sbin:/usr/X11R6/bin:/usr/local/bin"
   - Solaris 11.4:
     "/usr/xpg6/bin:/usr/xpg4/bin:/usr/bin:/opt/developerstudio12.5/bin:/opt/solarisstudio12.4/bin"
   - Haiku:
     "/boot/system/non-packaged/bin:/bin:/boot/system/apps:/boot/system/preferences"
 */
#define MAX_DEFAULT_PATH_SIZE 256

const char *
find_in_path (const char *progname)
{
#if defined _WIN32 || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
  /* Native Windows, Cygwin, OS/2, DOS */
  /* The searching rules with .COM, .EXE, .BAT, .CMD etc. suffixes are
     too complicated.  Leave it to the OS.  */
  return progname;
#else
  /* Unix */
  if (strchr (progname, '/') != NULL)
    /* If progname contains a slash, it is either absolute or relative to
       the current directory.  PATH is not used.  */
    return progname;

  const char *path = getenv ("PATH");
  char default_path[MAX_DEFAULT_PATH_SIZE];
  if (path == NULL)
    {
      /* If PATH is not set, the default search path is implementation
         dependent.  On POSIX systems, it is confstr (_CS_PATH).  */
      #if HAVE_CONFSTR
      int ret = confstr (_CS_PATH, default_path, sizeof (default_path));
      if (ret == 0)
        /* The platform has confstr(), but it does not support _CS_PATH.  */
        abort ();
      if (ret > sizeof (default_path))
        /* MAX_DEFAULT_PATH_SIZE is too small.  Increase it.  */
        abort ();
      path = default_path;
      #elif defined __ANDROID__
      path = _PATH_DEFPATH;
      #else /* native Windows, Minix */
      path = "";
      #endif
    }
  if (*path == '\0')
    return progname;

  /* Make a copy, to prepare for destructive modifications.  */
  char *path_copy;
# if !IN_FINDPROG_LGPL
  path_copy = xstrdup (path);
# else
  path_copy = strdup (path);
  if (path_copy == NULL)
    /* Out of memory.  */
    return progname;
# endif
  for (char *path_rest = path_copy; ; )
    {
      /* Extract next directory in PATH.  */
      const char *dir = path_rest;
      char *cp;
      for (cp = path_rest; *cp != '\0' && *cp != ':'; cp++)
        ;
      bool last = (*cp == '\0');
      *cp = '\0';

      /* Empty PATH components designate the current directory.  */
      if (dir == cp)
        dir = ".";

      /* Concatenate dir and progname.  */
      char *progpathname;
# if !IN_FINDPROG_LGPL
      progpathname = xconcatenated_filename (dir, progname, NULL);
# else
      progpathname = concatenated_filename (dir, progname, NULL);
      if (progpathname == NULL)
        {
          /* Out of memory.  */
          free (path_copy);
          return progname;
        }
# endif

      /* On systems which have the eaccess() system call, let's use it.
         On other systems, let's hope that this program is not installed
         setuid or setgid, so that it is ok to call access() despite its
         design flaw.  */
      if (eaccess (progpathname, X_OK) == 0)
        {
          /* Check that the progpathname does not point to a directory.  */
          struct stat statbuf;

          if (stat (progpathname, &statbuf) >= 0
              && ! S_ISDIR (statbuf.st_mode))
            {
              /* Found!  */
              if (streq (progpathname, progname))
                {
                  free (progpathname);

                  /* Add the "./" prefix for real, that xconcatenated_filename()
                     optimized away.  This avoids a second PATH search when the
                     caller uses execlp/execvp.  */
# if !IN_FINDPROG_LGPL
                  progpathname = XNMALLOC (2 + strlen (progname) + 1, char);
# else
                  progpathname = (char *) malloc (2 + strlen (progname) + 1);
                  if (progpathname == NULL)
                    {
                      /* Out of memory.  */
                      free (path_copy);
                      return progname;
                    }
# endif
                  progpathname[0] = '.';
                  progpathname[1] = '/';
                  memcpy (progpathname + 2, progname, strlen (progname) + 1);
                }

              free (path_copy);
              return progpathname;
            }
        }

      free (progpathname);

      if (last)
        break;

      path_rest = cp + 1;
    }

  /* Not found in PATH.  An error will be signalled at the first call.  */
  free (path_copy);
  return progname;
#endif
}

#endif
