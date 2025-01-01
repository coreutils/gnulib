/* Test the getusershell, setusershell, endusershell functions.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

/* Written by Collin Funk <collin.funk1@gmail.com>, 2024.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include "signature.h"
SIGNATURE_CHECK (getusershell, char *, (void));
SIGNATURE_CHECK (setusershell, void, (void));
SIGNATURE_CHECK (endusershell, void, (void));

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "macros.h"

/* The shell names in the order they appear in '/etc/shells'.  */
static char **shells = NULL;
static size_t shell_count = 0;

/* Prepare the shells array.  */
static void
first_pass (void)
{
  char *ptr;
  size_t i = 0;

  /* Avoid reallocation.  */
  shell_count = 16;
  shells = malloc (shell_count * sizeof (char *));
  ASSERT (shells != NULL);

  for (; (ptr = getusershell ()); ++i)
    {
      /* Make sure comments and empty lines are ignored.  */
      ASSERT (ptr[0] != '#');
      ASSERT (ptr[0] != '\0');
      if (i >= shell_count)
        {
          shell_count *= 2;
          shells = realloc (shells, shell_count * sizeof (char *));
          ASSERT (shells != NULL);
        }
      shells[i] = strdup (ptr);
      ASSERT (shells[i] != NULL);
    }

  shell_count = i;
}

/* Assuming that '/etc/shells' does not change during the duration of this
   test, check that setusershell puts us back at the start of the file.  */
static void
second_pass (void)
{
  size_t i;
  char *ptr;

  /* Return to the start of the file.  */
  setusershell ();

  /* Make sure order is preserved.  */
  for (i = 0; i < shell_count; ++i)
    {
      ptr = getusershell ();
      ASSERT (ptr != NULL);
      ASSERT (STREQ (ptr, shells[i]));
      printf ("%s\n", ptr);
      free (shells[i]);
    }

  /* End of file.  */
  ASSERT (getusershell () == NULL);

  /* Clean up.  */
  free (shells);
  endusershell ();
}

int
main (void)
{
  first_pass ();
  second_pass ();

  return test_exit_status;
}
