/* Convert wide string to string.
   Copyright (C) 2008, 2010-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2008.

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

size_t
FUNC (char *dest, const SCHAR_T **srcp, size_t len, mbstate_t *ps)
{
  if (ps == NULL)
    ps = &INTERNAL_STATE;
  {
    const SCHAR_T *src = *srcp;
    size_t cur_max = MB_CUR_MAX;
    char buf[64];

    if (!(cur_max <= sizeof (buf)))
      abort ();

    if (dest != NULL)
      {
        char *destptr = dest;

        for (; len > 0; src++)
          {
            SCHAR_T wc = *src;
            size_t ret = WCRTOMB (len >= cur_max ? destptr : buf, wc, ps);

            if (ret == (size_t)(-1))
              goto bad_input;
            if (!(ret <= cur_max))
              abort ();
            if (len < ret)
              break;
            if (len < cur_max)
              memcpy (destptr, buf, ret);
            if (wc == 0)
              {
                src = NULL;
                /* Here mbsinit (ps).  */
                break;
              }
            destptr += ret;
            len -= ret;
          }
        *srcp = src;
        return destptr - dest;
      }
    else
      {
        /* Ignore dest and len, don't store *srcp at the end, and
           don't clobber *ps.  */
        mbstate_t state = *ps;
        size_t totalcount = 0;

        for (;; src++)
          {
            SCHAR_T wc = *src;
            size_t ret = WCRTOMB (buf, wc, &state);

            if (ret == (size_t)(-1))
              goto bad_input2;
            if (wc == 0)
              {
                /* Here mbsinit (&state).  */
                break;
              }
            totalcount += ret;
          }
        return totalcount;
      }

   bad_input:
    *srcp = src;
   bad_input2:
    errno = EILSEQ;
    return (size_t)(-1);
  }
}
