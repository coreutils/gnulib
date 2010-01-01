/* Conversion to UTF-16/UTF-32 from legacy encodings.
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
FUNC (const char *fromcode,
      enum iconv_ilseq_handler handler,
      const char *src, size_t srclen,
      size_t *offsets,
      UNIT *resultbuf, size_t *lengthp)
{
#if HAVE_UTF_NAME
  char *result = (char *) resultbuf;
  size_t length = *lengthp * sizeof (UNIT);

  if (mem_iconveha (src, srclen, fromcode, UTF_NAME, true, handler,
                    offsets, &result, &length) < 0)
    return NULL;
  if (offsets != NULL)
    {
      /* Convert 'char *' offsets to 'UNIT *' offsets.  */
      size_t *offsets_end = offsets + srclen;
      size_t *o;

      for (o = offsets; o < offsets_end; o++)
        if (*o != (size_t)(-1))
          *o = *o / sizeof (UNIT);
    }
  if ((length % sizeof (UNIT)) != 0)
    abort ();
  *lengthp = length / sizeof (UNIT);
  return (UNIT *) result;
#else
  uint8_t *utf8_string;
  size_t utf8_length;
  UNIT *result;

  utf8_string =
    u8_conv_from_encoding (fromcode, handler, src, srclen, offsets,
                           NULL, &utf8_length);
  if (utf8_string == NULL)
    return NULL;
  result = U8_TO_U (utf8_string, utf8_length, resultbuf, lengthp);
  if (result == NULL)
    {
      int saved_errno = errno;
      free (utf8_string);
      errno = saved_errno;
      return NULL;
    }
  if (offsets != NULL)
    {
      size_t length = *lengthp;
      size_t *offsets_end = offsets + srclen;
      size_t *o;
      size_t off8 = 0;  /* offset into utf8_string */
      size_t offunit = 0;       /* offset into result */

      for (o = offsets; o < offsets_end; o++)
        if (*o != (size_t)(-1))
          {
            while (off8 < *o)
              {
                int count8 = u8_mblen (utf8_string + off8, utf8_length - off8);
                int countunit = U_MBLEN (result + offunit, length - offunit);
                if (count8 < 0 || countunit < 0)
                  abort ();
                off8 += count8;
                offunit += countunit;
              }
            if (*o != off8)
              abort ();
            *o = offunit;
          }
    }
  free (utf8_string);
  return result;
#endif
}
