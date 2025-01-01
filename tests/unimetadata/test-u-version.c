/* Test of supported Unicode version.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

#include "unimetadata.h"

#include "macros.h"

#if !(((defined _WIN32 || defined __CYGWIN__) && (HAVE_LIBUNISTRING || WOE32DLL)) || defined __ANDROID__)
/* Check that _libunistring_unicode_version is defined and links.  */
int const *vp = &_libunistring_unicode_version;
#endif

int
main ()
{
  /* Check that _libunistring_unicode_version is defined and links.  */
  volatile int v = _libunistring_unicode_version;
  ASSERT (v >= (15 << 8));
  ASSERT (v < (100 << 8));

  return test_exit_status;
}
