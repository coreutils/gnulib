/* Conversion from UTF-16/UTF-32 to legacy encodings.
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

char *
FUNC (const UNIT *string,
      const char *tocode,
      enum iconv_ilseq_handler handler)
{
#if HAVE_UTF_NAME
  char *result = NULL;
  size_t length = 0;

  if (mem_iconveha ((const char *) string, (U_STRLEN (string) + 1) * sizeof (UNIT),
                    UTF_NAME, tocode,
                    handler == iconveh_question_mark, handler,
                    NULL, &result, &length) < 0)
    return NULL;
  /* Verify the result has exactly one NUL byte, at the end.  */
  if (!(length > 0 && result[length-1] == '\0' && strlen (result) == length-1))
    {
      free (result);
      errno = EILSEQ;
      return NULL;
    }
  return result;
#else
  uint8_t tmpbuf[4096];
  size_t tmpbufsize = SIZEOF (tmpbuf);
  uint8_t *utf8_string;
  char *result;

  utf8_string = U_TO_U8 (string, U_STRLEN (string) + 1, tmpbuf, &tmpbufsize);
  if (utf8_string == NULL)
    return NULL;
  result = u8_strconv_to_encoding (utf8_string, tocode, handler);
  if (result == NULL)
    {
      if (utf8_string != tmpbuf)
        {
          int saved_errno = errno;
          free (utf8_string);
          errno = saved_errno;
        }
      return NULL;
    }
  if (utf8_string != tmpbuf)
    free (utf8_string);
  return result;
#endif
}
