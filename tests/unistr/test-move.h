/* Test of uN_move() functions.
   Copyright (C) 2010-2026 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

int
main ()
{
  /* Test copying operations with disjoint source and destination.  */
  {
    static const UNIT src[] = { 'c', 'l', 'i', 'm', 'a', 't', 'e' };

    for (size_t n = 0; n <= countof (src); n++)
      {
        UNIT dest[1 + countof (src) + 1] =
          { MAGIC, MAGIC, MAGIC, MAGIC, MAGIC, MAGIC, MAGIC, MAGIC, MAGIC };
        UNIT *ret;

        ret = U_MOVE (dest + 1, src, n);
        ASSERT (ret == dest + 1);
        ASSERT (dest[0] == MAGIC);
        for (size_t i = 0; i < n; i++)
          ASSERT (dest[1 + i] == src[i]);
        ASSERT (dest[1 + n] == MAGIC);
      }
  }

  /* Test copying operations with overlap, in-place.  */
  {
    static const UNIT src[] = { 'c', 'l', 'i', 'm', 'a', 't', 'e' };

    for (size_t n = 0; n <= countof (src); n++)
      {
        UNIT dest[1 + countof (src) + 1];
        UNIT *ret;

        dest[0] = MAGIC;
        for (size_t i = 0; i < n; i++)
          dest[1 + i] = src[i];
        dest[1 + n] = MAGIC;

        ret = U_MOVE (dest + 1, dest + 1, n);
        ASSERT (ret == dest + 1);
        ASSERT (dest[0] == MAGIC);
        for (size_t i = 0; i < n; i++)
          ASSERT (dest[1 + i] == src[i]);
        ASSERT (dest[1 + n] == MAGIC);
      }
  }

  /* Test copying operations with overlap, moving downward.  */
  {
    static const UNIT src[] = { 'c', 'l', 'i', 'm', 'a', 't', 'e' };
    static const UNIT src2[] = { 'C', 'L', 'I', 'M', 'A', 'T', 'E' };

    ASSERT (countof (src) == countof (src2));
    for (size_t d = 0; d <= countof (src); d++)
      {
        for (size_t n = 0; n <= countof (src); n++)
          {
            UNIT dest[1 + 2 * countof (src) + 1];
            UNIT *ret;

            dest[0] = MAGIC;
            for (size_t i = 0; i < countof (src2); i++)
              dest[1 + i] = src2[i];
            for (size_t i = 0; i < countof (src); i++)
              dest[1 + countof (src) + i] = src[i];
            dest[1 + 2 * countof (src)] = MAGIC;

            ret =
              U_MOVE (dest + 1 + countof (src) - d, dest + 1 + countof (src), n);
            ASSERT (ret == dest + 1 + countof (src) - d);
            ASSERT (dest[0] == MAGIC);
            for (size_t i = 0; i < countof (src) - d; i++)
              ASSERT (dest[1 + i] == src2[i]);
            for (size_t i = 0; i < n; i++)
              ASSERT (dest[1 + countof (src) - d + i] == src[i]);
            for (size_t i = countof (src) - d + n; i < countof (src2); i++)
              ASSERT (dest[1 + i] == src2[i]);
            for (size_t i = (n >= d ? n - d : 0); i < countof (src); i++)
              ASSERT (dest[1 + countof (src) + i] == src[i]);
            ASSERT (dest[1 + 2 * countof (src)] == MAGIC);
          }
      }
  }

  /* Test copying operations with overlap, moving upward.  */
  {
    static const UNIT src[] = { 'c', 'l', 'i', 'm', 'a', 't', 'e' };
    static const UNIT src2[] = { 'C', 'L', 'I', 'M', 'A', 'T', 'E' };

    ASSERT (countof (src) == countof (src2));
    for (size_t d = 0; d <= countof (src); d++)
      {
        for (size_t n = 0; n <= countof (src); n++)
          {
            UNIT dest[1 + 2 * countof (src) + 1];
            UNIT *ret;

            dest[0] = MAGIC;
            for (size_t i = 0; i < countof (src); i++)
              dest[1 + i] = src[i];
            for (size_t i = 0; i < countof (src2); i++)
              dest[1 + countof (src) + i] = src2[i];
            dest[1 + 2 * countof (src)] = MAGIC;

            ret = U_MOVE (dest + 1 + d, dest + 1, n);
            ASSERT (ret == dest + 1 + d);
            ASSERT (dest[0] == MAGIC);
            for (size_t i = 0; i < d; i++)
              ASSERT (dest[1 + i] == src[i]);
            for (size_t i = 0; i < n; i++)
              ASSERT (dest[1 + d + i] == src[i]);
            for (size_t i = d + n; i < countof (src); i++)
              ASSERT (dest[1 + i] == src[i]);
            for (size_t i = (d + n >= countof (src) ? d + n - countof (src) : 0);
                 i < countof (src2);
                 i++)
              ASSERT (dest[1 + countof (src) + i] == src2[i]);
            ASSERT (dest[1 + 2 * countof (src)] == MAGIC);
          }
      }
  }

  return test_exit_status;
}
