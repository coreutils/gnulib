/* Test of uN_chr() functions.
   Copyright (C) 2008-2010 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Paolo Bonzini <bonzini@gnu.org>, 2010.
   Based on test-chr.h, by Eric Blake and Bruno Haible.  */

int
main (void)
{
  size_t size = 0x100000;
  size_t length;
  size_t i;
  UNIT *input;
  uint32_t *input32 = (uint32_t *) malloc ((size + 1) * sizeof (uint32_t));
  ASSERT (input32);

  input32[0] = 'a';
  input32[1] = 'b';
  u32_set (input32 + 2, 'c', 1024);
  for (i = 1026; i < size - 2; i += 63)
    {
      size_t last = i + 63 < size - 2 ? i + 63 : size - 2;
      ucs4_t uc = 'd' | (i - 1026);
      if (uc >= 0xd800 && uc <= 0xdfff)
        uc |= 0x100000;
      u32_set (input32 + i, uc, last - i);
    }
  input32[size - 2] = 'e';
  input32[size - 1] = 'a';
  input32[size] = 0;

  input = U32_TO_U (input32, size + 1, NULL, &length);
  ASSERT (input);

  /* Basic behavior tests.  */
  ASSERT (U_STRCHR (input, 'a') == input);
  ASSERT (U_STRCHR (input, 'b') == input + 1);
  ASSERT (U_STRCHR (input, 'c') == input + 2);

  {
    UNIT *exp = input + 1026;
    UNIT *prev = input + 2;

    for (i = 1026; i < size - 2; i += 63)
      {
        UNIT c[6];
        size_t n;
        ucs4_t uc = 'd' | (i - 1026);
        if (uc >= 0xd800 && uc <= 0xdfff)
          uc |= 0x100000;
        n = U_UCTOMB (c, uc, 6);
        ASSERT (exp < input + length - 2);
        ASSERT (U_STRCHR (prev, uc) == exp);
        ASSERT (memcmp (exp, c, n * sizeof (UNIT)) == 0);
        prev = exp;
        exp += n * 63;
      }
  }

  ASSERT (U_STRCHR (input + 1, 'a') == input + length - 2);
  ASSERT (U_STRCHR (input + 1, 'e') == input + length - 3);

  ASSERT (U_STRCHR (input, 'f') == NULL);
  ASSERT (U_STRCHR (input, '\0') == input + length - 1);

  /* Check that a very long haystack is handled quickly if the byte is
     found near the beginning.  */
  {
    size_t repeat = 10000;
    for (; repeat > 0; repeat--)
      {
        ASSERT (U_STRCHR (input, 'c') == input + 2);
      }
  }

  /* Alignment tests.  */
  {
    int i, j;
    for (i = 0; i < 32; i++)
      {
        for (j = 0; j < 127; j++)
          input[i + j] = j + 1;
        input[i + 128] = 0;
        for (j = 0; j < 127; j++)
          {
            ASSERT (U_STRCHR (input + i, j + 1) == input + i + j);
          }
      }
  }

  /* Check that uN_chr() does not read past the first occurrence of the
     byte being searched.  */
  {
    char *page_boundary = (char *) zerosize_ptr ();
    size_t n;

    if (page_boundary != NULL)
      {
        for (n = 2; n <= 500 / sizeof (UNIT); n++)
          {
            UNIT *mem = (UNIT *) (page_boundary - n * sizeof (UNIT));
            U_SET (mem, 'X', n - 2);
            mem[n - 2] = 0;
            ASSERT (U_STRCHR (mem, 'U') == NULL);
            mem[n - 2] = 'X';
            mem[n - 1] = 0;
            ASSERT (U_STRCHR (mem, 'U') == NULL);
          }
      }
  }

  free (input);

  return 0;
}
