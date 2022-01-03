/* Locale dependent, case and normalization insensitive comparison of Unicode
   strings.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

int
FUNC (const UNIT *s1, size_t n1, const UNIT *s2, size_t n2,
      const char *iso639_language, uninorm_t nf, int *resultp)
{
  char buf1[2048];
  char buf2[2048];
  char *transformed1;
  size_t transformed1_length;
  char *transformed2;
  size_t transformed2_length;
  int cmp;

  /* Normalize and transform S1.  */
  transformed1_length = sizeof (buf1);
  transformed1 =
    U_CASEXFRM (s1, n1, iso639_language, nf, buf1, &transformed1_length);
  if (transformed1 == NULL)
    /* errno is set here.  */
    return -1;

  /* Normalize and transform S2.  */
  transformed2_length = sizeof (buf2);
  transformed2 =
    U_CASEXFRM (s2, n2, iso639_language, nf, buf2, &transformed2_length);
  if (transformed2 == NULL)
    {
      if (transformed1 != buf1)
        {
          int saved_errno = errno;
          free (transformed1);
          errno = saved_errno;
        }
      return -1;
    }

  /* Compare the transformed strings.  */
  cmp = memcmp2 (transformed1, transformed1_length,
                 transformed2, transformed2_length);
  if (cmp < 0)
    cmp = -1;
  else if (cmp > 0)
    cmp = 1;

  if (transformed2 != buf2)
    free (transformed2);
  if (transformed1 != buf1)
    free (transformed1);
  *resultp = cmp;
  return 0;
}
