/* Test whether a Unicode string is invariant under a given case mapping.
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
FUNC (const UNIT *s, size_t n,
      UNIT * (*mapping) (const UNIT *s, size_t n, const char *iso639_language,
                         uninorm_t nf,
                         UNIT *resultbuf, size_t *lengthp),
      const char *iso639_language,
      bool *resultp)
{
  UNIT normsbuf[2048 / sizeof (UNIT)];
  UNIT *norms;
  size_t norms_length;
  UNIT mappedbuf[2048 / sizeof (UNIT)];
  UNIT *mapped;
  size_t mapped_length;

  /* Apply canonical decomposition to S.  */
  norms_length = sizeof (normsbuf) / sizeof (UNIT);
  norms = U_NORMALIZE (UNINORM_NFD, s, n, normsbuf, &norms_length);
  if (norms == NULL)
    /* errno is set here.  */
    return -1;

  /* Apply mapping.  */
  mapped_length = sizeof (mappedbuf) / sizeof (UNIT);
  mapped = mapping (norms, norms_length, iso639_language, NULL,
                    mappedbuf, &mapped_length);
  if (mapped == NULL)
    {
      if (norms != normsbuf)
        {
          int saved_errno = errno;
          free (norms);
          errno = saved_errno;
        }
      return -1;
    }

  /* Compare.  */
  *resultp = (mapped_length == norms_length
              && U_CMP (mapped, norms, norms_length) == 0);

  if (mapped != mappedbuf)
    free (mapped);
  if (norms != normsbuf)
    free (norms);
  return 0;
}
