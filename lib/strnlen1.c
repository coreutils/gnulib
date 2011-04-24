/* Find the length of STRING + 1, but scan at most MAXLEN bytes.
   Copyright (C) 2005-2006, 2009-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "strnlen1.h"

#include <string.h>

/* The attribute __pure__ was added in gcc 2.96.  */
#undef _GL_ATTRIBUTE_PURE
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

/* Find the length of STRING + 1, but scan at most MAXLEN bytes.
   If no '\0' terminator is found in that many characters, return MAXLEN.  */
/* This is the same as strnlen (string, maxlen - 1) + 1.  */
size_t _GL_ATTRIBUTE_PURE
strnlen1 (const char *string, size_t maxlen)
{
  const char *end = (const char *) memchr (string, '\0', maxlen);
  if (end != NULL)
    return end - string + 1;
  else
    return maxlen;
}
