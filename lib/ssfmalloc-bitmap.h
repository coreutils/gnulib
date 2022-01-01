/* Simple and straight-forward malloc implementation (front end).

   Copyright (C) 2020-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

/* ===================== Low-level functions for bitmaps ==================== */

/* A bitmap is represented as an array of uint32_t = 'unsigned int', each with
   32 bits.  The bit i in the word with index j therefore represents bit
   k = 32 * j + i of the entire bit sequence.  */

/* Initializes a bitmap.  */
static inline void
init_bitmap_all_bits_clear (size_t num_words, uint32_t *words)
{
  size_t i;
  for (i = 0; i < num_words; i++)
    words[i] = 0;
}

/* Initializes a bitmap.  */
static inline void
init_bitmap_all_bits_set (size_t num_words, uint32_t *words)
{
  size_t i;
  for (i = 0; i < num_words; i++)
    words[i] = ~(uint32_t)0;
}

/* Returns the smallest index k >= k0 for which the bit k is set in the bitmap
   consisting of num_words words.  Returns (size_t)(-1) if there is none.  */
static size_t
find_first_bit_set (size_t num_words, const uint32_t *words, size_t k0)
{
  size_t j0 = k0 / 32;
  if (j0 < num_words)
    {
      size_t i0 = k0 % 32;
      const uint32_t *ptr = words + j0;
      /* Look at the word j0, ignoring the i0 least significant bits.  */
      {
        size_t found = ffs (*ptr & (-1U << i0));
        if (found > 0)
          return 32 * j0 + (found - 1);
      }
      /* Look at the subsequent words.  */
      const uint32_t *words_end = words + num_words;
      while (++ptr < words_end)
        {
          size_t found = ffs (*ptr);
          if (found > 0)
            return 32 * (ptr - words) + (found - 1);
        }
    }
  return (size_t)(-1);
}

/* Returns the smallest index k >= 0 for which the bit packet of c consecutive
   bits (1 <= c <= 32) is all set in the bitmap consisting of num_words words.
   Returns (size_t)(-1) if there is none.  */
static size_t
find_first_packet_set (size_t num_words, const uint32_t *words, size_t c)
{
  const uint32_t *ptr = words;
  const uint32_t *words_end = words + num_words;
  switch (c)
    {
    case 1:
      {
        /* A simplified variant of find_first_bit_set.  */
        for (; ptr < words_end; ptr++)
          {
            size_t found = ffs (*ptr);
            if (found > 0)
              return 32 * (ptr - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 2:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t combined = longword & (longword >> 1);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 3:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t combined = longword & (longword >> 1) & (longword >> 2);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 4:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t combined = tmp1 & (tmp1 >> 2);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 5:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t combined = tmp2 & (longword >> 4);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 6:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t combined = tmp1 & (tmp1 >> 2) & (tmp1 >> 4);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 7:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t combined =
              tmp1 & (tmp1 >> 2) & (tmp1 >> 4) & (longword >> 6);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 8:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t combined = tmp2 & (tmp2 >> 4);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 9:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined = tmp3 & (longword >> 8);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 10:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined = tmp3 & (tmp1 >> 8);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 11:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined = tmp3 & (tmp1 >> 8) & (longword >> 10);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 12:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t combined = tmp2 & (tmp2 >> 4) & (tmp2 >> 8);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 13:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t combined =
              tmp2 & (tmp2 >> 4) & (tmp2 >> 8) & (longword >> 12);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 14:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t combined = tmp2 & (tmp2 >> 4) & (tmp2 >> 8) & (tmp1 >> 12);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 15:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            /* Optimized: Use 5, not 6, '&' operations.  */
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (longword >> 4);
            uint64_t combined = tmp3 & (tmp3 >> 5) & (tmp3 >> 10);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 16:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined = tmp3 & (tmp3 >> 8);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 17:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (longword >> 16);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 18:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp1 >> 16);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 19:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp1 >> 16) & (longword >> 18);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 20:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp2 >> 16);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 21:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp2 >> 16) & (longword >> 20);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 22:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp2 >> 16) & (tmp1 >> 20);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 23:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined =
              tmp4 & (tmp2 >> 16) & (tmp1 >> 20) & (longword >> 22);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 24:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined = tmp3 & (tmp3 >> 8) & (tmp3 >> 16);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 25:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined =
              tmp3 & (tmp3 >> 8) & (tmp3 >> 16) & (longword >> 24);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 26:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined =
              tmp3 & (tmp3 >> 8) & (tmp3 >> 16) & (tmp1 >> 24);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 27:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            /* Optimized: Use 6, not 7, '&' operations.  */
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (longword >> 8);
            uint64_t combined = tmp4 & (tmp4 >> 9) & (tmp4 >> 18);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 28:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined =
              tmp3 & (tmp3 >> 8) & (tmp3 >> 16) & (tmp2 >> 24);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 29:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t combined =
              tmp3 & (tmp3 >> 8) & (tmp3 >> 16) & (tmp2 >> 24) & (longword >> 28);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 30:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            /* Optimized: Use 6, not 7, '&' operations.  */
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp1 >> 8);
            uint64_t combined = tmp4 & (tmp4 >> 10) & (tmp4 >> 20);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 31:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined =
              tmp4 & (tmp3 >> 16) & (tmp2 >> 24) & (tmp1 >> 28) & (longword >> 30);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    case 32:
      {
        while (ptr < words_end)
          {
            uint64_t longword = *ptr++;
            if (likely (ptr < words_end))
              longword |= ((uint64_t) *ptr) << 32;
            uint64_t tmp1 = longword & (longword >> 1);
            uint64_t tmp2 = tmp1 & (tmp1 >> 2);
            uint64_t tmp3 = tmp2 & (tmp2 >> 4);
            uint64_t tmp4 = tmp3 & (tmp3 >> 8);
            uint64_t combined = tmp4 & (tmp4 >> 16);
            size_t found = ffsll (combined);
            if (found > 0)
              return 32 * (ptr - 1 - words) + (found - 1);
          }
        return (size_t)(-1);
      }
    default:
      /* Invalid argument.  */
      abort ();
    }
}
