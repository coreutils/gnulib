/* strerror-impl.h --- Implementation of POSIX compatible strerror() function.

   Copyright (C) 2007-2011 Free Software Foundation, Inc.

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

#ifdef STATIC
STATIC
#endif
char *
strerror (int n)
{
  static char buf[256];

  int ret = strerror_r (n, buf, sizeof (buf));

  if (ret == 0)
    return buf;

  if (ret == ERANGE)
    /* If this happens, increase the size of buf.  */
    abort ();

  {
    static char const fmt[] = "Unknown error (%d)";
    verify (sizeof (buf) >= sizeof (fmt) + INT_STRLEN_BOUND (n));
    sprintf (buf, fmt, n);
    errno = ret;
    return buf;
  }
}
