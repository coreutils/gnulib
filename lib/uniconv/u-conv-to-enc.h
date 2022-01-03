/* Conversion from UTF-16/UTF-32 to legacy encodings.
   Copyright (C) 2002, 2006-2022 Free Software Foundation, Inc.

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

char *
FUNC (const char *tocode,
      enum iconv_ilseq_handler handler,
      const UNIT *src, size_t srclen,
      size_t *offsets,
      char *resultbuf, size_t *lengthp)
{
#if HAVE_UTF_NAME
  size_t *scaled_offsets;
  char *result;
  size_t length;

  if (offsets != NULL && srclen > 0)
    {
      scaled_offsets =
        (size_t *) malloc (srclen * sizeof (UNIT) * sizeof (size_t));
      if (scaled_offsets == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
    }
  else
    scaled_offsets = NULL;

  result = resultbuf;
  length = *lengthp;
  if (mem_iconveha ((const char *) src, srclen * sizeof (UNIT),
                    UTF_NAME, tocode,
                    handler == iconveh_question_mark, handler,
                    scaled_offsets, &result, &length) < 0)
    {
      int saved_errno = errno;
      free (scaled_offsets);
      errno = saved_errno;
      return NULL;
    }

  if (offsets != NULL)
    {
      /* Convert scaled_offsets[srclen * sizeof (UNIT)] to
         offsets[srclen].  */
      size_t i;

      for (i = 0; i < srclen; i++)
        offsets[i] = scaled_offsets[i * sizeof (UNIT)];
      free (scaled_offsets);
    }

  if (result == NULL) /* when (resultbuf == NULL && length == 0)  */
    {
      result = (char *) malloc (1);
      if (result == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
    }
  *lengthp = length;
  return result;
#else
  uint8_t tmpbuf[4096];
  size_t tmpbufsize = SIZEOF (tmpbuf);
  uint8_t *utf8_src;
  size_t utf8_srclen;
  size_t *scaled_offsets;
  char *result;

  utf8_src = U_TO_U8 (src, srclen, tmpbuf, &tmpbufsize);
  if (utf8_src == NULL)
    return NULL;
  utf8_srclen = tmpbufsize;

  if (offsets != NULL && utf8_srclen > 0)
    {
      scaled_offsets = (size_t *) malloc (utf8_srclen * sizeof (size_t));
      if (scaled_offsets == NULL)
        {
          if (utf8_src != tmpbuf)
            free (utf8_src);
          errno = ENOMEM;
          return NULL;
        }
    }
  else
    scaled_offsets = NULL;

  result = u8_conv_to_encoding (tocode, handler, utf8_src, utf8_srclen,
                                scaled_offsets, resultbuf, lengthp);
  if (result == NULL)
    {
      int saved_errno = errno;
      free (scaled_offsets);
      if (utf8_src != tmpbuf)
        free (utf8_src);
      errno = saved_errno;
      return NULL;
    }
  if (offsets != NULL)
    {
      size_t iunit;     /* offset into src */
      size_t i8;        /* offset into utf8_src */

      for (iunit = 0; iunit < srclen; iunit++)
        offsets[iunit] = (size_t)(-1);

      iunit = 0;
      i8 = 0;
      while (iunit < srclen && i8 < utf8_srclen)
        {
          int countunit;
          int count8;

          offsets[iunit] = scaled_offsets[i8];

          countunit = U_MBLEN (src + iunit, srclen - iunit);
          count8 = u8_mblen (utf8_src + i8, utf8_srclen - i8);
          if (countunit < 0 || count8 < 0)
            abort ();
          iunit += countunit;
          i8 += count8;
        }
      /* Check that utf8_src has been traversed entirely.  */
      if (i8 < utf8_srclen)
        abort ();
      /* Check that src has been traversed entirely, except possibly for an
         incomplete sequence of units at the end.  */
      if (iunit < srclen)
        {
          offsets[iunit] = *lengthp;
          if (!(U_MBLEN (src + iunit, srclen - iunit) < 0))
            abort ();
        }
      free (scaled_offsets);
    }
  if (utf8_src != tmpbuf)
    free (utf8_src);
  return result;
#endif
}
