/* check-version.h --- Check version string compatibility.
   Copyright (C) 1998-2006, 2008-2022 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson.  This interface is influenced by
   gcry_check_version from Werner Koch's Libgcrypt.  Paul Eggert
   suggested the use of strverscmp to simplify implementation. */

#include <config.h>

#include <stddef.h>
#include <string.h>

/* Get specification. */
#include "check-version.h"

/* Check that the version of the library (i.e., the CPP symbol VERSION)
 * is at minimum the requested one in REQ_VERSION (typically found in
 * a header file) and return the version string.  Return NULL if the
 * condition is not satisfied.  If a NULL is passed to this function,
 * no check is done, but the version string is simply returned.
 */
const char *
check_version (const char *req_version)
{
  if (!req_version || strverscmp (req_version, VERSION) <= 0)
    return VERSION;

  return NULL;
}
