/* hasmntopt() function: test for the presence of a mount option.

   Copyright (C) 1995-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <mntent.h>

#include <string.h>

char *
hasmntopt (const struct mntent *mnt, const char *opt)
{
  const size_t optlen = strlen (opt);
  char *rest = mnt->mnt_opts, *p;

  while ((p = strstr (rest, opt)) != NULL)
    {
      if ((p == rest || p[-1] == ',')
          && (p[optlen] == '\0' || p[optlen] == '=' || p[optlen] == ','))
        return p;

      rest = strchr (p, ',');
      if (rest == NULL)
        break;
      ++rest;
    }

  return NULL;
}
