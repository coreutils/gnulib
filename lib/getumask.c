/* Retrieve the umask of the process (multithread-safe).
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

/* There are three ways to implement a getumask() function on systems that
   don't have it:
     (a) Through system calls on the file system.
     (b) Through a global variable that the main() function has to set,
         together with an override of the umask() function.
     (c) Through { mode_t mask = umask (0); umask (mask); }.

   Each has its drawbacks:
     (a) Causes additional system calls. May fail in some rare cases.
     (b) Causes globally visible code complexity / maintainer effort.
     (c) Is not multithread-safe: open() calls in other threads may
         create files with wrong access permissions.

   Here we implement (a), as the least evil.  */

#include <config.h>
/* The package may define ASSUME_UMASK_CONSTANT to 1, to indicate that the
   program does not call umask().  */
/* #define ASSUME_UMASK_CONSTANT 1 */

/* Specification.  */
#include <sys/stat.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "clean-temp.h"
#include "tempname.h"

mode_t
getumask (void)
{
#if 0
  /* This is not multithread-safe!  */
  mode_t mask = umask (0);
  umask (mask);
  return mask;
#else
# if ASSUME_UMASK_CONSTANT
  static int cached_umask = -1;
  if (cached_umask >= 0)
    return cached_umask;
# endif

  int mask = -1;
# if defined __linux__
  {
    /* In Linux >= 4.7, the umask can be retrieved from an "Umask:" line in the
       /proc/self/status file.  */
    char buf[4096];
    int fd = open ("/proc/self/status", O_RDONLY);
    if (fd >= 0)
      {
        ssize_t n = read (fd, buf, sizeof (buf));
        if (n > 0)
          {
            const char *p_end = buf + n;
            const char *p = buf;

            for (;;)
              {
                /* Here we're at the beginning of a line.  */
                if (p_end - p > 8 && memcmp (p, "Umask:\t0", 8) == 0)
                  {
                    unsigned int value = 0;
                    p += 8;
                    for (; p < p_end && *p >= '0' && *p <= '7'; p++)
                      value = 8 * value + (*p - '0');
                    if (p < p_end && *p == '\n')
                      mask = value;
                    break;
                  }
                /* Search the start of the next line.  */
                for (; p < p_end && *p != '\n'; p++)
                  ;
                if (p == p_end)
                  break;
                p++;
              }
          }
        close (fd);
      }
  }
# endif
  if (mask < 0)
    {
      /* Create a temporary file and inspect its access permissions.  */
      const char *tmpdir = getenv ("TMPDIR");
      if (tmpdir == NULL || *tmpdir == '\0')
        tmpdir = "/tmp";
      size_t tmpdir_length = strlen (tmpdir);
      char *temp_filename = (char *) malloc (tmpdir_length + 15 + 1);
      if (temp_filename != NULL)
        {
          memcpy (temp_filename, tmpdir, tmpdir_length);
          strcpy (temp_filename + tmpdir_length, "/gtumask.XXXXXX");
          int fd = gen_register_open_temp (temp_filename, 0, O_RDWR,
                                           S_IRWXU|S_IRWXG|S_IRWXO);
          if (fd >= 0)
            {
              struct stat statbuf;
              if (fstat (fd, &statbuf) >= 0)
                mask = (S_IRWXU|S_IRWXG|S_IRWXO) & ~statbuf.st_mode;
              close_temp (fd);
              cleanup_temporary_file (temp_filename, false);
            }
          free (temp_filename);
        }
    }
  if (mask < 0)
    {
      /* We still don't know!  Assume a paranoid user.  */
      mask = 077;
    }
# if ASSUME_UMASK_CONSTANT
  cached_umask = mask;
# endif
  return mask;
#endif
}
