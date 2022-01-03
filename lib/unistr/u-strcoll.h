/* Compare UTF-8/UTF-16/UTF-32 strings using the collation rules of the current
   locale.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

int
FUNC (const UNIT *s1, const UNIT *s2)
{
  /* When this function succeeds, it sets errno back to its original value.
     When it fails, it sets errno, but also returns a meaningful return value,
     for the sake of callers which ignore errno.  */
  int final_errno = errno;
  const char *encoding = locale_charset ();
  char *sl1;
  char *sl2;
  int result;

  /* Pass iconveh_error here, not iconveh_question_mark.  Otherwise the
     conversion to locale encoding can do transliteration or map some
     characters to question marks, leading to results that depend on the
     iconv() implementation and are not obvious.  */
  sl1 = U_STRCONV_TO_ENCODING (s1, encoding, iconveh_error);
  if (sl1 != NULL)
    {
      sl2 = U_STRCONV_TO_ENCODING (s2, encoding, iconveh_error);
      if (sl2 != NULL)
        {
          /* Compare sl1 and sl2.  */
          errno = 0;
          result = strcoll (sl1, sl2);
          if (errno == 0)
            {
              /* strcoll succeeded.  */
              free (sl1);
              free (sl2);
              /* The conversion to locale encoding can drop Unicode TAG
                 characters.  Therefore sl1 and sl2 may be equal when s1
                 and s2 were in fact different.  Return a nonzero result
                 in this case.  */
              if (result == 0)
                result = U_STRCMP (s1, s2);
            }
          else
            {
              /* strcoll failed.  */
              final_errno = errno;
              free (sl1);
              free (sl2);
              result = U_STRCMP (s1, s2);
            }
        }
      else
        {
          /* s1 could be converted to locale encoding, s2 not.  */
          final_errno = errno;
          free (sl1);
          result = -1;
        }
    }
  else
    {
      final_errno = errno;
      sl2 = U_STRCONV_TO_ENCODING (s2, encoding, iconveh_error);
      if (sl2 != NULL)
        {
          /* s2 could be converted to locale encoding, s1 not.  */
          free (sl2);
          result = 1;
        }
      else
        {
          /* Neither s1 nor s2 could be converted to locale encoding.  */
          result = U_STRCMP (s1, s2);
        }
    }

  errno = final_errno;
  return result;
}
