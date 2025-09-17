/* mbs_endswith function.
   Copyright (C) 2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include "mbiter.h"

bool
mbs_endswith (const char *string, const char *suffix)
{
  if ((unsigned char) suffix[0] < 0x30)
    /* Some knowledge about the possible multibyte encodings:

       Encoding    First byte of character    Which of these can occur
                                              as second or later byte?

        EUC-JP     0x00..0x7F, 0x8E..0xFE     0xA1..0xFE
        EUC-KR     0x00..0x7F, 0xA1..0xFD     0xA1..0xFD
        GB2312     0x00..0x7F, 0xA1..0xF7     0xA1..0xF7
        EUC-TW     0x00..0x7F, 0x8E..0xFD     0xA1..0xFD
        BIG5       0x00..0x7F, 0xA1..0xF9     0x40..0x7E, 0xA1..0xF9
        GB18030    0x00..0x7F, 0x81..0xFE     0x30..0x39, 0x40..0x7E, 0x81..0xFE
        SJIS       0x00..0x7F, 0x81..0xF9     0x40..0x7E, 0x81..0xF9
        UTF-8      0x00..0x7F, 0xC2..0xF7     none

       Therefore, if the first byte of SUFFIX is < 0x30, it cannot occur as
       second or later byte, and therefore it is OK to do a bytewise search.  */
    return str_endswith (string, suffix);

  /* Here, suffix is not empty.  */

  size_t nbytes = strlen (string);
  if (nbytes >= strlen (suffix))
    {
      size_t len = mbslen (string);
      size_t n = mbslen (suffix);
      if (len >= n)
        {
          mbi_iterator_t iter;
          mbi_init (iter, string, nbytes);
          /* Advance past (len - n) multibyte characters.  */
          for (; len > n; len--)
            {
              if (!mbi_avail (iter))
                /* We can get here due to incomplete multibyte characters.  */
                return false;
              mbi_advance (iter);
            }
          if (!mbi_avail (iter))
            /* We can get here due to incomplete multibyte characters.  */
            return false;
          return streq (mbi_cur_ptr (iter), suffix);
        }
    }
  return false;
}
