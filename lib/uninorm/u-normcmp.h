/* Normalization insensitive comparison of Unicode strings.
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
      uninorm_t nf, int *resultp)
{
  UNIT buf1[2048 / sizeof (UNIT)];
  UNIT buf2[2048 / sizeof (UNIT)];
  UNIT *norms1;
  size_t norms1_length;
  UNIT *norms2;
  size_t norms2_length;
  int cmp;

  /* Normalize S1.  */
  norms1_length = sizeof (buf1) / sizeof (UNIT);
  norms1 = U_NORMALIZE (nf, s1, n1, buf1, &norms1_length);
  if (norms1 == NULL)
    /* errno is set here.  */
    return -1;

  /* Normalize S2.  */
  norms2_length = sizeof (buf2) / sizeof (UNIT);
  norms2 = U_NORMALIZE (nf, s2, n2, buf2, &norms2_length);
  if (norms2 == NULL)
    {
      if (norms1 != buf1)
        {
          int saved_errno = errno;
          free (norms1);
          errno = saved_errno;
        }
      return -1;
    }

  /* Compare the normalized strings.  */
  cmp = U_CMP2 (norms1, norms1_length, norms2, norms2_length);
  if (cmp > 0)
    cmp = 1;
  else if (cmp < 0)
    cmp = -1;

  if (norms2 != buf2)
    free (norms2);
  if (norms1 != buf1)
    free (norms1);
  *resultp = cmp;
  return 0;
}
