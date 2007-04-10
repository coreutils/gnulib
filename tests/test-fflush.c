/* Test of POSIX compatible fflush() function.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Eric Blake, 2007.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  FILE *f;
  char buffer[10];
  int fd;

  /* Create test file.  */
  f = fopen ("test-fflush.txt", "w");
  if (!f || fwrite ("1234567890", 1, 10, f) != 10 || fclose (f) != 0)
    {
      fputs ("Failed to create sample file.\n", stderr);
      unlink ("test-fflush.txt");
      return 1;
    }

  /* Test fflush.  */
  f = fopen ("test-fflush.txt", "r");
  fd = fileno (f);
  if (!f || 0 > fd || fread (buffer, 1, 5, f) != 5)
    {
      fputs ("Failed initial read of sample file.\n", stderr);
      fclose (f);
      unlink ("test-fflush.txt");
      return 1;
    }
  /* For deterministic results, ensure f read a bigger buffer.  */
  if (lseek (fd, 0, SEEK_CUR) == 5)
    {
      fputs ("Sample file was not buffered.\n", stderr);
      fclose (f);
      unlink ("test-fflush.txt");
      return 1;
    }
  /* POSIX requires fflush-fseek to set file offset of fd.  */
  if (fflush (f) != 0 || fseek (f, 0, SEEK_CUR) != 0)
    {
      fputs ("Failed to flush sample file.\n", stderr);
      fclose (f);
      unlink ("test-fflush.txt");
      return 1;
    }
  /* Check that offset is correct.  */
  if (lseek (fd, 0, SEEK_CUR) != 5)
    {
      fputs ("File offset is wrong.\n", stderr);
      fclose (f);
      unlink ("test-fflush.txt");
      return 1;
    }
  /* Check that file reading resumes at correct location.  */
  if (fgetc (f) != '6')
    {
      fputs ("Failed to read next byte of file.\n", stderr);
      fclose (f);
      unlink ("test-fflush.txt");
      return 1;
    }
  fclose (f);
  unlink ("test-fflush.txt");
  return 0;
}
