/* Test of ftello() function on large files.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include "macros.h"

#define TESTFILE "test-ftello.data"
#define TESTFILE_ZEROES 3000000000LL
#define TESTFILE_DATA "GNU"
#define TESTFILE_DATA_LEN 3

int
main (void)
{
  remove (TESTFILE);

  if (sizeof (off_t) <= 4)
    {
      fprintf (stderr, "off_t is only 32 bits.\n");
      return 77;
    }
  /* off_t is larger than 32-bit.  */

  /* Create a file that is larger than 2 GiB.
     On file systems such as ext2, the file will have "holes" and thus
     not consume much disk space.  */
  {
    int fd = open (TESTFILE, O_CREAT | O_TRUNC | O_RDWR, 0600);
    ASSERT (fd >= 0);
    if (ftruncate (fd, TESTFILE_ZEROES) < 0
        || lseek (fd, TESTFILE_ZEROES, SEEK_SET) < 0
        || write (fd, TESTFILE_DATA, TESTFILE_DATA_LEN) < 0
        || fsync (fd) < 0)
      {
        close (fd);
        remove (TESTFILE);
        fprintf (stderr, "Could not create 3 GB large file.\n");
        return 77;
      }
    close (fd);
  }

  /* Check the value of ftello().  */
  {
    FILE *fp = fopen (TESTFILE, "r");
    ASSERT (fp != NULL);

    ASSERT (fseeko (fp, 0, SEEK_END) == 0);

    off_t length = ftello (fp);
    ASSERT (length > 0);
    ASSERT (length == TESTFILE_ZEROES + TESTFILE_DATA_LEN);

    int ret = fclose (fp);
    ASSERT (ret == 0);
  }

  remove (TESTFILE);

  return test_exit_status;
}
