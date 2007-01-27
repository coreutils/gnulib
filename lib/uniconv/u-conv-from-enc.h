/* Conversion to UTF-16/UTF-32 from legacy encodings.
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
FUNC (const char *fromcode,
      enum iconv_ilseq_handler handler,
      const char *src, size_t srclen,
      size_t *offsets,
      UNIT **resultp, size_t *lengthp)
{
#if HAVE_UTF_NAME
  size_t length;

  if (mem_iconveha (src, srclen, fromcode, UTF_NAME, true, handler,
		    offsets, (char **) resultp, &length) < 0)
    return -1;
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
  return 0;
#else
  uint8_t *utf8_string = NULL;
  size_t utf8_length = 0;
  UNIT *result;

  if (u8_conv_from_encoding (fromcode, handler, src, srclen, offsets,
			     &utf8_string, &utf8_length) < 0)
    return -1;
  if (utf8_string == NULL)
    {
      *lengthp = 0;
      return 0;
    }
  result = U8_TO_U (utf8_string, utf8_length, *resultp, lengthp);
  if (result == NULL)
    {
      int saved_errno = errno;
      free (utf8_string);
      errno = saved_errno;
      return -1;
    }
  if (offsets != NULL)
    {
      size_t length = *lengthp;
      size_t *offsets_end = offsets + srclen;
      size_t *o;
      size_t off8 = 0;	/* offset into utf8_string */
      size_t offunit = 0;	/* offset into result */

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
  *resultp = result;
  return 0;
#endif
}
