/* Test of ptsname_r(3).
   Copyright (C) 2010-2012 Free Software Foundation, Inc.

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

#include <stdlib.h>

#include "signature.h"
SIGNATURE_CHECK (ptsname_r, int, (int, char *, size_t));

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "same-inode.h"

#include "macros.h"

/* Compare two slave names.
   On some systems, there are hard links in the /dev/ directory.
   For example, on OSF/1 5.1,
     /dev/ttyp0 == /dev/pts/0
     /dev/ttyp9 == /dev/pts/9
     /dev/ttypa == /dev/pts/10
     /dev/ttype == /dev/pts/14
 */
static int
same_slave (const char *slave_name1, const char *slave_name2)
{
  struct stat statbuf1;
  struct stat statbuf2;

  return (strcmp (slave_name1, slave_name2) == 0
          || (stat (slave_name1, &statbuf1) >= 0
              && stat (slave_name2, &statbuf2) >= 0
              && SAME_INODE (statbuf1, statbuf2)));
}

static char *
null_ptr (void)
{
  return NULL;
}

static void
test_errors (int fd, const char *slave)
{
  char buffer[256];
  size_t len;
  size_t buflen_max;
  size_t buflen;
  int result;

  len = strlen (slave);
  buflen_max = len + 5;
  if (buflen_max > sizeof buffer)
    buflen_max = sizeof buffer;
  for (buflen = 0; buflen <= buflen_max; buflen++)
    {
      memset (buffer, 'X', sizeof buffer);
      errno = 0;
      result = ptsname_r (fd, buffer, buflen);
      if (buflen > len)
        {
          ASSERT (result == 0);
          ASSERT (buffer[0] == '/');
        }
      else
        {
          ASSERT (result != 0);
          ASSERT (result == errno);
          ASSERT (errno == ERANGE);
          ASSERT (buffer[0] == 'X');
        }
    }

  errno = 0;
  result = ptsname_r (fd, null_ptr (), 0);
  ASSERT (result != 0);
  ASSERT (result == errno);
  ASSERT (errno == EINVAL);
}

int
main (void)
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  */
  signal (SIGALRM, SIG_DFL);
  alarm (5);
#endif

  {
    char buffer[256];
    int result;

    errno = 0;
    result = ptsname_r (-1, buffer, sizeof buffer);
    ASSERT (result != 0);
    ASSERT (result == errno);
    ASSERT (errno == EBADF || errno == ENOTTY);
  }

  {
    int fd;
    char buffer[256];
    int result;

    /* Open the controlling tty of the current process.  */
    fd = open ("/dev/tty", O_RDONLY);
    if (fd < 0)
      {
        fprintf (stderr, "Skipping test: cannot open controlling tty\n");
        return 77;
      }

    result = ptsname_r (fd, buffer, sizeof buffer);
    /* The result is usually NULL, because /dev/tty is a slave, not a
       master.  */
    if (result == 0)
      {
        ASSERT (memcmp (buffer, "/dev/", 5) == 0);
      }

    close (fd);
  }

#if defined __sun
  /* Solaris has BSD-style /dev/pty[p-r][0-9a-f] files, but the function
     ptsname() does not work on them.  */
  {
    int fd;
    char buffer[256];
    int result;

    /* Open the controlling tty of the current process.  */
    fd = open ("/dev/ptmx", O_RDWR | O_NOCTTY);
    if (fd < 0)
      {
        fprintf (stderr, "Skipping test: cannot open pseudo-terminal\n");
        return 77;
      }

    result = ptsname_r (fd, buffer, sizeof buffer);
    ASSERT (result == 0);
    ASSERT (memcmp (buffer, "/dev/pts/", 9) == 0);

    test_errors (fd, buffer);

    close (fd);
  }

#else

  /* Try various master names of MacOS X: /dev/pty[p-w][0-9a-f]  */
  {
    int char1;
    int char2;

    for (char1 = 'p'; char1 <= 'w'; char1++)
      for (char2 = '0'; char2 <= 'f'; (char2 == '9' ? char2 = 'a' : char2++))
        {
          char master_name[32];
          int fd;

          sprintf (master_name, "/dev/pty%c%c", char1, char2);
          fd = open (master_name, O_RDONLY);
          if (fd >= 0)
            {
              char buffer[256];
              int result;
              char slave_name[32];

              result = ptsname_r (fd, buffer, sizeof buffer);
              ASSERT (result == 0);
              sprintf (slave_name, "/dev/tty%c%c", char1, char2);
              ASSERT (same_slave (buffer, slave_name));

              test_errors (fd, buffer);

              close (fd);
            }
        }
  }

  /* Try various master names of *BSD: /dev/pty[p-sP-S][0-9a-v]  */
  {
    int upper;
    int char1;
    int char2;

    for (upper = 0; upper <= 1; upper++)
      for (char1 = (upper ? 'P' : 'p'); char1 <= (upper ? 'S' : 's'); char1++)
        for (char2 = '0'; char2 <= 'v'; (char2 == '9' ? char2 = 'a' : char2++))
          {
            char master_name[32];
            int fd;

            sprintf (master_name, "/dev/pty%c%c", char1, char2);
            fd = open (master_name, O_RDONLY);
            if (fd >= 0)
              {
                char buffer[256];
                int result;
                char slave_name[32];

                result = ptsname_r (fd, buffer, sizeof buffer);
                ASSERT (result == 0);
                sprintf (slave_name, "/dev/tty%c%c", char1, char2);
                ASSERT (same_slave (buffer, slave_name));

                test_errors (fd, buffer);

                close (fd);
              }
          }
  }

#endif

  return 0;
}
