/* Test of execution of program termination handlers.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include "canonicalize.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#if !HAVE_SYMLINK
# define symlink(a,b) (-1)
#endif /* !HAVE_SYMLINK */

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

#define BASE "t-can-lgpl.tmp"

int
main ()
{
#ifdef GNULIB_CANONICALIZE
  /* No need to test canonicalize-lgpl module if canonicalize is also
     in use.  */
  return 0;
#endif

  /* Setup some hierarchy to be used by this test.  Start by removing
     any leftovers from a previous partial run.  */
  {
    int fd;
    ASSERT (system ("rm -rf " BASE " ise") == 0);
    ASSERT (mkdir (BASE, 0700) == 0);
    fd = creat (BASE "/tra", 0600);
    ASSERT (0 <= fd);
    ASSERT (close (fd) == 0);
  }

  /* Check for ., .., intermediate // handling, and for error cases.  */
  {
    char *result = canonicalize_file_name (BASE "//./..//" BASE "/tra");
    ASSERT (result != NULL);
    ASSERT (strstr (result, "/" BASE "/tra")
	    == result + strlen (result) - strlen ("/" BASE "/tra"));
    free (result);
    errno = 0;
    result = canonicalize_file_name ("");
    ASSERT (result == NULL);
    ASSERT (errno == ENOENT);
    errno = 0;
    result = canonicalize_file_name (NULL);
    ASSERT (result == NULL);
    ASSERT (errno == EINVAL);
  }

  /* From here on out, tests involve symlinks.  */
  if (symlink (BASE "/ket", "ise") != 0)
    {
      ASSERT (remove (BASE "/tra") == 0);
      ASSERT (rmdir (BASE) == 0);
      fputs ("skipping test: symlinks not supported on this filesystem\n",
	     stderr);
      return 77;
    }
  ASSERT (symlink ("bef", BASE "/plo") == 0);
  ASSERT (symlink ("tra", BASE "/huk") == 0);
  ASSERT (symlink ("lum", BASE "/bef") == 0);
  ASSERT (symlink ("wum", BASE "/ouk") == 0);
  ASSERT (symlink ("../ise", BASE "/ket") == 0);
  ASSERT (mkdir (BASE "/lum", 0700) == 0);

  /* Check that the symbolic link to a file can be resolved.  */
  {
    char *result1 = canonicalize_file_name (BASE "/huk");
    char *result2 = canonicalize_file_name (BASE "/tra");
    ASSERT (result1 != NULL);
    ASSERT (result2 != NULL);
    ASSERT (strcmp (result1, result2) == 0);
    ASSERT (strcmp (result1 + strlen (result1) - strlen ("/" BASE "/tra"),
		    "/" BASE "/tra") == 0);
    free (result1);
    free (result2);
  }

  /* Check that the symbolic link to a directory can be resolved.  */
  {
    char *result1 = canonicalize_file_name (BASE "/plo");
    char *result2 = canonicalize_file_name (BASE "/bef");
    char *result3 = canonicalize_file_name (BASE "/lum");
    ASSERT (result1 != NULL);
    ASSERT (result2 != NULL);
    ASSERT (result3 != NULL);
    ASSERT (strcmp (result1, result2) == 0);
    ASSERT (strcmp (result2, result3) == 0);
    ASSERT (strcmp (result1 + strlen (result1) - strlen ("/" BASE "/lum"),
		    "/" BASE "/lum") == 0);
    free (result1);
    free (result2);
    free (result3);
  }

  /* Check that a symbolic link to a nonexistent file yields NULL.  */
  {
    char *result;
    errno = 0;
    result = canonicalize_file_name (BASE "/ouk");
    ASSERT (result == NULL);
    ASSERT (errno == ENOENT);
  }

  /* Check that a loop of symbolic links is detected.  */
  {
    char *result;
    errno = 0;
    result = canonicalize_file_name ("ise");
    ASSERT (result == NULL);
    ASSERT (errno == ELOOP);
  }

  /* Cleanup.  */
  ASSERT (remove (BASE "/plo") == 0);
  ASSERT (remove (BASE "/huk") == 0);
  ASSERT (remove (BASE "/bef") == 0);
  ASSERT (remove (BASE "/ouk") == 0);
  ASSERT (remove (BASE "/ket") == 0);
  ASSERT (remove (BASE "/lum") == 0);
  ASSERT (remove (BASE "/tra") == 0);
  ASSERT (remove (BASE) == 0);
  ASSERT (remove ("ise") == 0);

  return 0;
}
