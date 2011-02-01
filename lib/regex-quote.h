/* Construct a regular expression from a literal string.
   Copyright (C) 1995, 2010-2011 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2010.

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

#include <stddef.h>

/* regex_quote converts a literal string to a regular expression that will
   look for this literal string.
   cflags can be 0 or REG_EXTENDED.
   If it is 0, the result is a Basic Regular Expression (BRE)
   <http://www.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03>.
   If it is REG_EXTENDED, the result is an Extended Regular Expression (ERE)
   <http://www.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_04>.
   The result is not anchored; if you want it to match only complete lines,
   you need to add "^" at the beginning of the result and "$" at the end of the
   result.
 */

/* Returns the number of bytes needed for the quoted string.  */
extern size_t regex_quote_length (const char *string, int cflags);

/* Copies the quoted string to p and returns the incremented p.
   There must be room for regex_quote_length (string, cflags) + 1 bytes at p.
 */
extern char * regex_quote_copy (char *p, const char *string, int cflags);

/* Returns the freshly allocated quoted string.  */
extern char * regex_quote (const char *string, int cflags);
