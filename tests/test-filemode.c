/* Test the strmode function.
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
#include "filemode.h"

#include <sys/stat.h>

/* The strmode function is already defined on some systems in <string.h> or
   <unistd.h>.  On macOS, FreeBSD < 14.0, OpenBSD, the function takes an int
   instead of a mode_t (uint16_t) as its first argument.  Include these
   headers here to make sure our declaration doesn't conflict with system
   functions.  */
#include <string.h>
#include <unistd.h>

#include "macros.h"

struct mode_to_string
{
  mode_t mode;
  char str[12];
};

static const struct mode_to_string table[] =
  {
    { S_IFCHR, "c--------- " },
    { S_IFDIR, "d--------- " },
    { S_IFIFO, "p--------- " },
    { S_IFREG, "---------- " },
    { S_IFCHR | S_IRUSR | S_IWGRP | S_IXOTH, "cr---w---x " },
    { S_IFDIR | S_IXUSR | S_IRGRP | S_IWOTH, "d--xr---w- " },
    { S_IFIFO | S_IWUSR | S_IXGRP | S_IROTH, "p-w---xr-- " },
#if S_ISUID
    { S_IFREG | S_ISUID, "---S------ " },
    { S_IFREG | S_ISUID | S_IXUSR, "---s------ " },
#endif
#if S_ISGID
    { S_IFREG | S_ISGID, "------S--- " },
    { S_IFREG | S_ISGID | S_IXGRP, "------s--- " },
#endif
#if S_ISVTX
    { S_IFREG | S_ISVTX, "---------T " },
    { S_IFREG | S_ISVTX | S_IXOTH, "---------t " },
#endif
  };

int
main (void)
{
  size_t i;
  char buffer[12];

  for (i = 0; i < SIZEOF (table); ++i)
    {
      strmode (table[i].mode, buffer);
      ASSERT (STREQ (table[i].str, buffer));
    }

  return test_exit_status;
}
