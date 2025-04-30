/* hashcode-string1.h -- declaration for a simple hash function
   Copyright (C) 1998-2004, 2006-2007, 2009-2025 Free Software Foundation, Inc.

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

/* This file uses _GL_ATTRIBUTE_PURE.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Compute a hash code for a NUL-terminated string S,
   and return the hash code modulo TABLESIZE.
   The result is platform dependent: it depends on the size of the 'size_t'
   type.  */
extern size_t hash_string (char const *s, size_t tablesize) _GL_ATTRIBUTE_PURE;


#ifdef __cplusplus
}
#endif
