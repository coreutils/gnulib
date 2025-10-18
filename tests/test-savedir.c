/* Test the savedir module.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Collin Funk <collin.funk1@gmail.com>, 2025.  */

#include <config.h>

/* Specification.  */
#include "savedir.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ignore-value.h"
#include "macros.h"

/* Base directory.  */
#define BASE "test-savedir.t"

static void
test_savedir_sort_none (void)
{
  bool seen[('z' - 'a') + 1];
  char *name_space = savedir (BASE, SAVEDIR_SORT_NONE);
  ASSERT (name_space != NULL);

  /* Set the table of file names we have seen to false.  */
  memset (seen, 0, sizeof seen);

  /* Scan through the file names.  */
  for (char *namep = name_space; *namep != '\0'; namep += strlen (namep) + 1)
    {
      int index = *namep - 'a';
      ASSERT (strlen (namep) == 1);
      ASSERT (index < sizeof seen);
      seen[index] = true;
    }

  /* Make sure all of them have been seen.  */
  for (int i = 0; i < sizeof seen; ++i)
    ASSERT (seen[i]);

  free (name_space);
}

static void
test_savedir_sort_name (void)
{
  int i = 0;
  char *name_space = savedir (BASE, SAVEDIR_SORT_NAME);
  ASSERT (name_space != NULL);

  /* Check that files "a" to "z" appear in order.  */
  for (char *namep = name_space; *namep != '\0';
       namep += strlen (namep) + 1, i += 1)
    {
      ASSERT (strlen (namep) == 1);
      ASSERT (*namep - 'a' == i);
    }

  /* Make sure no extra files were seen.  */
  ASSERT (i == ('z' - 'a') + 1);

  free (name_space);
}

int
main (void)
{
  char buffer[sizeof BASE + 2];

  /* Clean up any trash from prior testsuite runs.  */
  ignore_value (system ("rm -rf " BASE "*"));

  /* Setup a directory with files "a" to "z".  */
  strcpy (buffer, BASE);
  ASSERT (mkdir (buffer, 0700) == 0);
  buffer[sizeof buffer - 1] = '\0';
  for (int i = 'a'; i <= 'z'; ++i)
    {
      buffer[sizeof buffer - 3] = '/';
      buffer[sizeof buffer - 2] = i;
      int fd = creat (buffer, 0600);
      ASSERT (0 <= fd);
      ASSERT (close (fd) == 0);
    }

  test_savedir_sort_none ();
  test_savedir_sort_name ();

  /* Cleanup files created during the test.  */
  ignore_value (system ("rm -rf " BASE "*"));

  return test_exit_status;
}
