/* Test that execinfo.h defines stub functions.
   Copyright (C) 2024 Free Software Foundation, Inc.

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
#include <execinfo.h>

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

static void
test_backtrace (void)
{
  void *buffer[10];
  char **symbols;
  int size;

  size = backtrace (buffer, SIZEOF (buffer));
  symbols = backtrace_symbols (buffer, size);

  /* Print the backtrace if possible.  */
  if (0 < size && symbols != NULL)
    {
      for (int i = 0; i < size; ++i)
        printf ("%s\n", symbols[i]);
      free (symbols);
    }
}

int
main (void)
{
  test_backtrace ();

  return 0;
}
