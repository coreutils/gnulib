/* Test whether case matters for a Unicode string.
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
FUNC (const UNIT *s, size_t n, const char *iso639_language,
      bool *resultp)
{
  UNIT normsbuf[2048 / sizeof (UNIT)];
  UNIT *norms;
  size_t norms_length;
  UNIT mappedbuf[2048 / sizeof (UNIT)];
  UNIT *mapped_toupper;
  UNIT *mapped_tolower;
  UNIT *mapped_totitle;
  size_t mapped_length;

  /* Apply canonical decomposition to S.  */
  norms_length = sizeof (normsbuf) / sizeof (UNIT);
  norms = U_NORMALIZE (UNINORM_NFD, s, n, normsbuf, &norms_length);
  if (norms == NULL)
    /* errno is set here.  */
    return -1;

  mapped_length = sizeof (mappedbuf) / sizeof (UNIT);

  /* Apply toupper mapping.  */
  mapped_toupper = U_TOUPPER (norms, norms_length, iso639_language, NULL,
                              mappedbuf, &mapped_length);
  if (mapped_toupper == NULL)
    goto fail;

  /* Compare.  */
  if (!(mapped_length == norms_length
        && U_CMP (mapped_toupper, norms, norms_length) == 0))
    {
      if (mapped_toupper != mappedbuf)
        free (mapped_toupper);
      goto yes;
    }

  /* Apply tolower mapping.  */
  mapped_tolower = U_TOLOWER (norms, norms_length, iso639_language, NULL,
                              mapped_toupper, &mapped_length);
  if (mapped_tolower == NULL)
    {
      if (mapped_toupper != mappedbuf)
        {
          int saved_errno = errno;
          free (mapped_toupper);
          errno = saved_errno;
        }
      goto fail;
    }

  if (mapped_toupper != mapped_tolower && mapped_toupper != mappedbuf)
    free (mapped_toupper);

  /* Compare.  */
  if (!(mapped_length == norms_length
        && U_CMP (mapped_tolower, norms, norms_length) == 0))
    {
      if (mapped_tolower != mappedbuf)
        free (mapped_tolower);
      goto yes;
    }

  /* Apply totitle mapping.  */
  mapped_totitle = U_TOTITLE (norms, norms_length, iso639_language, NULL,
                              mapped_tolower, &mapped_length);
  if (mapped_totitle == NULL)
    {
      if (mapped_tolower != mappedbuf)
        {
          int saved_errno = errno;
          free (mapped_tolower);
          errno = saved_errno;
        }
      goto fail;
    }

  if (mapped_tolower != mapped_totitle && mapped_tolower != mappedbuf)
    free (mapped_tolower);

  /* Compare.  */
  if (!(mapped_length == norms_length
        && U_CMP (mapped_totitle, norms, norms_length) == 0))
    {
      if (mapped_totitle != mappedbuf)
        free (mapped_totitle);
      goto yes;
    }

  if (mapped_totitle != mappedbuf)
    free (mapped_totitle);
  if (norms != normsbuf)
    free (norms);
  *resultp = false;
  return 0;

 yes:
  if (norms != normsbuf)
    free (norms);
  *resultp = true;
  return 0;

 fail:
  if (norms != normsbuf)
    {
      int saved_errno = errno;
      free (norms);
      errno = saved_errno;
    }
  return -1;
}
