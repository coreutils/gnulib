/* Locale dependent transformation for comparison of Unicode strings.
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
FUNC (const UNIT *s, size_t n, uninorm_t nf,
      char *resultbuf, size_t *lengthp)
{
  UNIT normsbuf[2048 / sizeof (UNIT)];
  UNIT *norms;
  size_t norms_length;
  char convsbuf[2048];
  char *convs;
  size_t convs_length;
  char *result;

  /* Normalize the Unicode string.  */
  norms_length = sizeof (normsbuf) / sizeof (UNIT);
  norms = U_NORMALIZE (nf, s, n, normsbuf, &norms_length);
  if (norms == NULL)
    /* errno is set here.  */
    return NULL;

  /* Convert it to locale encoding.  */
  convs_length = sizeof (convsbuf) - 1;
  convs = U_CONV_TO_ENCODING (locale_charset (),
                              iconveh_error,
                              norms, norms_length,
                              NULL,
                              convsbuf, &convs_length);
  if (convs == NULL)
    {
      if (norms != normsbuf)
        {
          int saved_errno = errno;
          free (norms);
          errno = saved_errno;
        }
      return NULL;
    }

  if (norms != normsbuf)
    free (norms);

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
