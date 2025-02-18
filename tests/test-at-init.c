/* Test of computed initialization.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include "at-init.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Avoid an error in the pragma on MSVC.  */
#undef read

extern int get_squares (void);
extern int get_cubes (void);

AT_INIT (initial_message);
#ifdef __SUNPRO_C
# pragma init (initial_message)
#endif

static void
initial_message (void)
{
  const char message[] = "Initializing...";
  write (STDOUT_FILENO, message, strlen (message));
}

AT_FINI (final_message);
#ifdef __SUNPRO_C
# pragma fini (final_message)
#endif

static void
final_message (void)
{
  const char message[] = "Finishing...";
  write (STDOUT_FILENO, message, strlen (message));
}

int
main ()
{
  char message[100];
  sprintf (message, "Main...%d...%d...", get_squares (), get_cubes ());
  write (STDOUT_FILENO, message, strlen (message));
}
