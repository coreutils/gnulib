/* Test of largefile module.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

/* This test may fail if AC_SYS_LARGEFILE could not arrange for a 64-bit off_t.
   This should be rare, though: only very old systems don't have support for
   files larger than 2 GiB.  */

#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include "intprops.h"

/* Check the range of off_t.
   With MSVC, this test succeeds only thanks to the 'sys_types' module.  */
static_assert (TYPE_MAXIMUM (off_t) >> 31 >> 31 != 0);

/* Check the size of the 'struct stat' field 'st_size'.
   ,With MSVC, this test succeeds only thanks to the 'sys_stat' module.  */
static struct stat st;
static_assert (sizeof st.st_size == sizeof (off_t));

int
main (void)
{
  return 0;
}
