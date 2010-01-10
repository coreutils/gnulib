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

/* Written by Eric Blake and Bruno Haible <bruno@clisp.org>, 2010.  */

int
main (void)
{
  size_t n = 0x100000;
  UNIT *input = (UNIT *) malloc (n * sizeof (UNIT));
  ASSERT (input);

  input[0] = 'a';
  input[1] = 'b';
  U_SET (input + 2, 'c', 1024);
  U_SET (input + 1026, 'd', n - 1028);
  input[n - 2] = 'e';
  input[n - 1] = 'a';

  /* Basic behavior tests.  */
  ASSERT (U_CHR (input, n, 'a') == input);

  ASSERT (U_CHR (input, 0, 'a') == NULL);
  ASSERT (U_CHR (zerosize_ptr (), 0, 'a') == NULL);

  ASSERT (U_CHR (input, n, 'b') == input + 1);
  ASSERT (U_CHR (input, n, 'c') == input + 2);
  ASSERT (U_CHR (input, n, 'd') == input + 1026);

  ASSERT (U_CHR (input + 1, n - 1, 'a') == input + n - 1);
  ASSERT (U_CHR (input + 1, n - 1, 'e') == input + n - 2);

  ASSERT (U_CHR (input, n, 'f') == NULL);
  ASSERT (U_CHR (input, n, '\0') == NULL);

  /* Check that a very long haystack is handled quickly if the byte is
     found near the beginning.  */
  {
    size_t repeat = 10000;
    for (; repeat > 0; repeat--)
      {
        ASSERT (U_CHR (input, n, 'c') == input + 2);
      }
  }

  /* Alignment tests.  */
  {
    int i, j;
    for (i = 0; i < 32; i++)
      {
        for (j = 0; j < 128; j++)
          input[i + j] = j;
        for (j = 0; j < 128; j++)
          {
            ASSERT (U_CHR (input + i, 128, j) == input + i + j);
          }
      }
  }

  /* Check that uN_chr() does not read past the first occurrence of the
     byte being searched.  */
  {
    char *page_boundary = (char *) zerosize_ptr ();

    if (page_boundary != NULL)
      {
        for (n = 1; n <= 500 / sizeof (UNIT); n++)
          {
            UNIT *mem = (UNIT *) (page_boundary - n * sizeof (UNIT));
            U_SET (mem, 'X', n);
            ASSERT (U_CHR (mem, n, 'U') == NULL);

            {
              size_t i;

              for (i = 0; i < n; i++)
                {
                  mem[i] = 'U';
                  ASSERT (U_CHR (mem, 4000, 'U') == mem + i);
                  mem[i] = 'X';
                }
            }
          }
      }
  }

  free (input);

  return 0;
}
