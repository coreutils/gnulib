/* Conversion from UTF-16/UTF-32 to legacy encodings.
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

int
FUNC (const char *tocode,
      enum iconv_ilseq_handler handler,
      const UNIT *src, size_t srclen,
      size_t *offsets,
      char **resultp, size_t *lengthp)
{
#if HAVE_UTF_NAME
  size_t *scaled_offsets;
  int retval;

  if (offsets != NULL)
    {
      scaled_offsets =
	(size_t *) malloc (srclen * sizeof (UNIT) * sizeof (size_t));
      if (scaled_offsets == NULL)
	{
	  errno = ENOMEM;
	  return -1;
	}
    }
  else
    scaled_offsets = NULL;

  retval = mem_iconveha ((const char *) src, srclen * sizeof (UNIT),
			 UTF_NAME, tocode,
			 handler == iconveh_question_mark, handler,
			 scaled_offsets, resultp, lengthp);

  if (offsets != NULL)
    {
      if (retval >= 0)
	{
	  /* Convert scaled_offsets[srclen * sizeof (UNIT)] to
	     offsets[srclen].  */
	  size_t i;

	  for (i = 0; i < srclen; i++)
	    offsets[i] = scaled_offsets[i * sizeof (UNIT)];
	}
      free (scaled_offsets);
    }
  return retval;
#else
  uint8_t tmpbuf[4096];
  size_t tmpbufsize = SIZEOF (tmpbuf);
  uint8_t *utf8_src;
  size_t utf8_srclen;
  size_t *scaled_offsets;
  int retval;

  utf8_src = U_TO_U8 (src, srclen, tmpbuf, &tmpbufsize);
  if (utf8_src == NULL)
    return NULL;
  utf8_srclen = tmpbufsize;

  if (offsets != NULL)
    {
      scaled_offsets = (size_t *) malloc (utf8_srclen * sizeof (size_t));
      if (scaled_offsets == NULL)
	{
	  if (utf8_src != tmpbuf)
	    free (utf8_src);
	  errno = ENOMEM;
	  return -1;
	}
    }
  else
    scaled_offsets = NULL;

  retval = u8_conv_to_encoding (tocode, handler, utf8_src, utf8_srclen,
				scaled_offsets, resultp, lengthp);
  if (retval < 0)
    {
      if (utf8_src != tmpbuf)
	{
	  int saved_errno = errno;
	  free (utf8_src);
	  errno = saved_errno;
	}
      return -1;
    }
  if (offsets != NULL)
    {
      size_t iunit;	/* offset into src */
      size_t i8;	/* offset into utf8_src */

      for (iunit = 0; iunit < srclen; iunit++)
	offsets[iunit] = (size_t)(-1);

      iunit = 0;
      i8 = 0;
      while (iunit < srclen)
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
      free (scaled_offsets);
    }
  if (utf8_src != tmpbuf)
    free (utf8_src);
  return retval;
#endif
}
