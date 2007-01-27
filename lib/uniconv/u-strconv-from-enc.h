/* Conversion to UTF-8/UTF-16/UTF-32 from legacy encodings.
   Copyright (C) 2002, 2006-2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

UNIT *
FUNC (const char *string,
      const char *fromcode,
      enum iconv_ilseq_handler handler)
{
  UNIT *result = NULL;
  size_t length = 0;

  if (U_CONV_FROM_ENCODING (fromcode, handler,
			    string, strlen (string) + 1, NULL,
			    &result, &length) < 0)
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
