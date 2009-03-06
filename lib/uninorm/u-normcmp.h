/* Normalization insensitive comparison of UTF-8 strings.
   Copyright (C) 2009 Free Software Foundation, Inc.
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
      uninorm_t nf, int *result)
{
  UNIT *norms1;
  size_t norms1_length;
  UNIT *norms2;
  size_t norms2_length;
  int cmp;

  /* Normalize S1.  */
  norms1 = U_NORMALIZE (nf, s1, n1, NULL, &norms1_length);
  if (norms1 == NULL)
    return errno;

  /* Normalize S2.  */
  norms2 = U_NORMALIZE (nf, s2, n2, NULL, &norms2_length);
  if (norms2 == NULL)
    {
      int saved_errno = errno;
      free (norms1);
      return saved_errno;
    }

  /* Compare the normalized strings.  */
  cmp = U_CMP (norms1, norms2, MIN (norms1_length, norms2_length));
  if (cmp == 0)
    {
      if (norms1_length < norms2_length)
	cmp = -1;
      else if (norms1_length > norms2_length)
	cmp = 1;
    }
  else if (cmp > 0)
    cmp = 1;
  else if (cmp < 0)
    cmp = -1;

  free (norms2);
  free (norms1);
  *result = cmp;
  return 0;
}
