/* Test of ptsname(3).
   Copyright (C) 2010 Free Software Foundation, Inc.

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
SIGNATURE_CHECK (ptsname, char *, (int));

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"

int
main (void)
{
  {
    int fd;
    char *result;

    /* Open the controlling tty of the current process.  */
    fd = open ("/dev/tty", O_RDONLY);
    if (fd < 0)
      {
        fprintf (stderr, "Skipping test: cannot open controlling tty\n");
        return 77;
      }

    result = ptsname (fd);
    /* The result is usually NULL, because /dev/tty is a slave, not a
       master.  */
    if (result != NULL)
      {
        ASSERT (memcmp (result, "/dev/", 5) == 0);
      }

    close (fd);
  }

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
              char *result;
              char slave_name[32];

              result = ptsname (fd);
              ASSERT (result != NULL);
              sprintf (slave_name, "/dev/tty%c%c", char1, char2);
              ASSERT (strcmp (result, slave_name) == 0);

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
                char *result;
                char slave_name[32];

                result = ptsname (fd);
                ASSERT (result != NULL);
                sprintf (slave_name, "/dev/tty%c%c", char1, char2);
                ASSERT (strcmp (result, slave_name) == 0);

                close (fd);
              }
          }
  }

  return 0;
}
