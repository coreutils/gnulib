/* Formatted output to strings.
   Copyright (C) 1999, 2002, 2006-2010 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif

int
VSPRINTF (DCHAR_T *buf, const FCHAR_T *format, va_list args)
{
  /* Pass an infinite length.  But note that *vasnprintf may fail if the buffer
     argument is larger than INT_MAX (if that fits into a 'size_t' at all).
     Also note that glibc's iconv fails with E2BIG when we pass a length that
     is so large that buf + length wraps around, i.e.
     (uintptr_t) (buf + length) < (uintptr_t) buf.  */
  size_t length;
  DCHAR_T *result;

  /* Set length = min (SIZE_MAX, INT_MAX, - (uintptr_t) buf - 1).  */
  length = (SIZE_MAX < INT_MAX ? SIZE_MAX : INT_MAX);
  if (length > (~ (uintptr_t) buf) / sizeof (DCHAR_T))
    length = (~ (uintptr_t) buf) / sizeof (DCHAR_T);

  result = VASNPRINTF (buf, &length, format, args);
  if (result == NULL)
    return -1;

  /* The infinite buffer size guarantees that the result is not malloc()ed.  */
  if (result != buf)
    {
      /* length is near SIZE_MAX.  */
      free (result);
      errno = EOVERFLOW;
      return -1;
    }

  if (length > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }

  /* Return the number of resulting units, excluding the trailing NUL.  */
  return length;
}
