/* Test whether a single-byte character is a hexadecimal digit.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <ctype.h>

int
isxdigit_l (int c, _GL_UNUSED locale_t locale)
{
  /* For consistency with isxdigit(), which is not locale dependent
     (see ISO C23 § 7.4.2.12).  */
  return ((c >= '0' && c <= '9')
#if 'A' == 0x41 && 'a' == 0x61
          /* Optimization, assuming ASCII */
          || ((c & ~0x20) >= 'A' && (c & ~0x20) <= 'F')
#else
          || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')
#endif
         );
}
