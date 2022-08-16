/* Test of creating a temporary file.
   Copyright (C) 2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2022.  */

#include <config.h>

#include "tempname.h"

#include <string.h>
#include <unistd.h>

#include "macros.h"

int
main ()
{
  /* Verify that two consecutive calls to gen_tempname return two different
     file names, with a high probability.  */
  const char *templ18 = "gl-temp-XXXXXX.xyz";
  char filename1[18 + 1];
  char filename2[18 + 1];

  /* Case 1: The first file still exists while gen_tempname is called a second
     time.  */
  {
    strcpy (filename1, templ18);
    int fd1 = gen_tempname (filename1, strlen (".xyz"), 0, GT_FILE);
    ASSERT (fd1 >= 0);

    strcpy (filename2, templ18);
    int fd2 = gen_tempname (filename2, strlen (".xyz"), 0, GT_FILE);
    ASSERT (fd2 >= 0);

    /* gen_tempname arranges (via O_EXCL) to not return the name of an existing
       file.  */
    ASSERT (strcmp (filename1, filename2) != 0);

    /* Clean up.  */
    close (fd1);
    close (fd2);
    unlink (filename1);
    unlink (filename2);
  }

  /* Case 2: The first file is deleted before gen_tempname is called a second
     time.  */
  {
    strcpy (filename1, templ18);
    int fd1 = gen_tempname (filename1, strlen (".xyz"), 0, GT_FILE);
    ASSERT (fd1 >= 0);

    /* Clean up.  */
    close (fd1);
    unlink (filename1);

    strcpy (filename2, templ18);
    int fd2 = gen_tempname (filename2, strlen (".xyz"), 0, GT_FILE);
    ASSERT (fd2 >= 0);

    /* With 6 'X' and a good pseudo-random number generator behind the scenes,
       the probability of getting the same file name twice in a row should be
       1/62^6 < 1/10^10.
       But on 64-bit native Windows, this probability is ca. 0.1% to 0.3%.  */
    ASSERT (strcmp (filename1, filename2) != 0);

    /* Clean up.  */
    close (fd2);
    unlink (filename2);
  }

  return 0;
}
