/* Locale dependent, normalization insensitive comparison of Unicode strings.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

int
FUNC (const UNIT *s1, size_t n1, const UNIT *s2, size_t n2,
      uninorm_t nf, int *resultp)
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
  transformed1 = U_NORMXFRM (s1, n1, nf, buf1, &transformed1_length);
  if (transformed1 == NULL)
    /* errno is set here.  */
    return -1;

  /* Normalize and transform S2.  */
  transformed2_length = sizeof (buf2);
  transformed2 = U_NORMXFRM (s2, n2, nf, buf2, &transformed2_length);
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
