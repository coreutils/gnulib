/* Conversion to UTF-8/UTF-16/UTF-32 from legacy encodings.
   Copyright (C) 2002, 2006-2007, 2009-2010 Free Software Foundation, Inc.

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

UNIT *
FUNC (const char *string,
      const char *fromcode,
      enum iconv_ilseq_handler handler)
{
  UNIT *result;
  size_t length;

  result =
    U_CONV_FROM_ENCODING (fromcode, handler,
                          string, strlen (string) + 1, NULL,
                          NULL, &length);
  if (result == NULL)
    return NULL;
  /* Verify the result has exactly one NUL unit, at the end.  */
  if (!(length > 0 && result[length-1] == 0
        && U_STRLEN (result) == length-1))
    {
      free (result);
      errno = EILSEQ;
      return NULL;
    }
  return result;
}
