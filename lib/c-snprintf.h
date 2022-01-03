/* snprintf in C locale.
   Copyright (C) 2012-2022 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

#ifndef _C_SNPRINTF_H
#define _C_SNPRINTF_H

/* Get size_t.  */
#include <stddef.h>

/* Get _GL_ATTRIBUTE_SPEC_PRINTF_STANDARD.  */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int c_snprintf (char *restrict str, size_t size,
                       const char *format, ...)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 3, 4));

#ifdef __cplusplus
}
#endif

#endif /* _C_SNPRINTF_H */
