/* Locale dependent transformation for case insensitive comparison of Unicode
   strings.
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

char *
FUNC (const UNIT *s, size_t n, const char *iso639_language, uninorm_t nf,
      char *resultbuf, size_t *lengthp)
{
  UNIT foldedsbuf[2048 / sizeof (UNIT)];
  UNIT *foldeds;
  size_t foldeds_length;
  char convsbuf[2048];
  char *convs;
  size_t convs_length;
  char *result;

  /* Casefold and normalize the Unicode string.  */
  foldeds_length = sizeof (foldedsbuf) / sizeof (UNIT);
  foldeds = U_CASEFOLD (s, n, iso639_language, nf, foldedsbuf, &foldeds_length);
  if (foldeds == NULL)
    /* errno is set here.  */
    return NULL;

  /* Convert it to locale encoding.  */
  convs_length = sizeof (convsbuf) - 1;
  convs = U_CONV_TO_ENCODING (locale_charset (),
                              iconveh_error,
                              foldeds, foldeds_length,
                              NULL,
                              convsbuf, &convs_length);
  if (convs == NULL)
    {
      if (foldeds != foldedsbuf)
        {
          int saved_errno = errno;
          free (foldeds);
          errno = saved_errno;
        }
      return NULL;
    }

  if (foldeds != foldedsbuf)
    free (foldeds);

  /* Ensure one more byte is available.  */
  if (convs != convsbuf)
    {
      char *memory = (char *) realloc (convs, convs_length + 1);
      if (memory == NULL)
        {
          free (convs);
          errno = ENOMEM;
          return NULL;
        }
      convs = memory;
    }

  /* Apply locale dependent transformations for comparison.  */
  result = memxfrm (convs, convs_length, resultbuf, lengthp);
  if (result == NULL)
    {
      if (convs != convsbuf)
        {
          int saved_errno = errno;
          free (convs);
          errno = saved_errno;
        }
      return NULL;
    }

  if (convs != convsbuf)
    free (convs);
  return result;
}
