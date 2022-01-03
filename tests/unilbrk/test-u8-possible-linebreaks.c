/* Test of line breaking of UTF-8 strings.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#include "unilbrk.h"

#include <stdlib.h>

#include "macros.h"

static void
test_function (void (*my_u8_possible_linebreaks) (const uint8_t *, size_t, const char *, char *_UC_RESTRICT),
               int version)
{
  /* Test case n = 0.  */
  my_u8_possible_linebreaks (NULL, 0, "GB18030", NULL);

  {
    static const uint8_t input[91] =
      /* "Grüß Gott. Здравствуйте! x=(-b±sqrt(b²-4ac))/(2a)  日本語,中文,한글" */
      "Gr\303\274\303\237 Gott. \320\227\320\264\321\200\320\260\320\262\321\201\321\202\320\262\321\203\320\271\321\202\320\265! x=(-b\302\261sqrt(b\302\262-4ac))/(2a)  \346\227\245\346\234\254\350\252\236,\344\270\255\346\226\207,\355\225\234\352\270\200\n";

    {
      char *p = (char *) malloc (SIZEOF (input));
      size_t i;

      my_u8_possible_linebreaks (input, SIZEOF (input), "GB18030", p);
      for (i = 0; i < 91; i++)
        {
          ASSERT (p[i] == (i == 90 ? UC_BREAK_MANDATORY :
                           i == 7
                           || i == 13 || i == 39
                           || i == 43
                           || i == 61 || i == 67
                           || i == 70 || i == 73 || i == 77 || i == 80
                           || i == 84 || i == 87 ? UC_BREAK_POSSIBLE :
                           UC_BREAK_PROHIBITED));
        }
      free (p);
    }

    {
      char *p = (char *) malloc (SIZEOF (input));
      size_t i;

      my_u8_possible_linebreaks (input, SIZEOF (input), "GB2312", p);
      for (i = 0; i < 91; i++)
        {
          ASSERT (p[i] == (i == 90 ? UC_BREAK_MANDATORY :
                           i == 7
                           || i == 13 || i == 39
                           || i == 43
                           || i == 52 || i == 61 || i == 67
                           || i == 70 || i == 73 || i == 77 || i == 80
                           || i == 84 || i == 87 ? UC_BREAK_POSSIBLE :
                           UC_BREAK_PROHIBITED));
        }
      free (p);
    }
  }

  /* CR LF handling.  */
  {
    static const uint8_t input[8] = "a\nb\rc\r\nd";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 8; i++)
      {
        ASSERT (p[i] == (i == 1 || i == 3 || i == 6 ? UC_BREAK_MANDATORY :
                         i == 5 ? (version < 2 ? UC_BREAK_MANDATORY : UC_BREAK_CR_BEFORE_LF) :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test that a break is possible after a zero-width space followed by some
     regular spaces (rule LB8 in Unicode TR#14 revision 26).  */
  {
    static const uint8_t input[6] = "x\342\200\213 y";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 6; i++)
      {
        ASSERT (p[i] == (i == 5 ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test line breaking in a string with HTML markup.  */
  {
    static const uint8_t input[21] = "<P>Some sentence.</P>";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 21; i++)
      {
        ASSERT (p[i] == (i == 8 || i == 17 || i == 19 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test line breaking of combining marks.  */
  {
    static const uint8_t input[24] =
      "a\314\200\314\201e\314\200 \314\201o \314\200 o\302\240\314\200\n"
      "\314\200";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 24; i++)
      {
        ASSERT (p[i] == (i == 21 ? UC_BREAK_MANDATORY :
                         i == 9 || i == 13 || i == 16 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test line breaking of zero-width joiners (U+200D).  */
  {
    static const uint8_t input[101] =
      "\346\234\211\347\204\241\347\252\256\345\244\232\345\200\213\347\264\240\346\225\270\343\200\202\n" /* "有無窮多個素數。" */
      "\346\234\211\342\200\215\347\204\241\342\200\215\347\252\256\345\244\232\345\200\213\347\264\240\342\200\215\346\225\270\343\200\202\n"
      "\344\275\240\342\200\224\344\270\215\n" /* "你—不" */
      "\344\275\240\342\200\224\342\200\215\344\270\215\n"
      "\342\230\235\360\237\217\277\n" /* "☝🏿" */
      "\342\230\235\342\200\215\360\237\217\277\n";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 101; i++)
      {
        ASSERT (p[i] == (i == 24 || i == 58
                         || i == 68 || i == 81
                         || i == 89 || i == 100 ? UC_BREAK_MANDATORY :
                         i == 3 || i == 6 || i == 9 || i == 12 || i == 15 || i == 18
                         || i == 40 || i == 43 || i == 46
                         || i == 62 || i == 65
                         || i == 72 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test line breaking of regional indicators.  */
  {
    static const uint8_t input[16] =
      "\360\237\207\251\360\237\207\252\360\237\207\253\360\237\207\267";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 16; i++)
      {
        ASSERT (p[i] == (i == 8 ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test special behaviour of hyphen/break-after character after
     Hebrew letter.  */
  {
    static const uint8_t input[15] = /* "ab-אב-αβ-ω" */
      "ab-\327\220\327\221-\316\261\316\262-\317\211";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 15; i++)
      {
        ASSERT (p[i] == (i == 3 || i == 13 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test special behaviour before East Asian opening parenthesis (LB30).  */
  {
    static const uint8_t input[49] = /* "日中韓統合漢字拡張G「ユニコード」" */
      "\346\227\245\344\270\255\351\237\223\347\265\261\345\220\210\346\274\242"
      "\345\255\227\346\213\241\345\274\265G\343\200\214\343\203\246"
      "\343\203\213\343\202\263\343\203\274\343\203\211\343\200\215";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 49; i++)
      {
        ASSERT (p[i] == (i == 3 || i == 6 || i == 9 || i == 12 || i == 15
                         || i == 18 || i == 21 || i == 24 || i == 27
                         || i == 28 /* This is the desired break position.  */
                         || i == 34 || i == 37 || i == 43 ? UC_BREAK_POSSIBLE :
                         UC_BREAK_PROHIBITED));
      }
    free (p);
  }

  /* Test special behaviour of potential future emoji (LB30b).  */
  {
    static const uint8_t input[8] = "\360\237\277\274\360\237\217\277";
    char *p = (char *) malloc (SIZEOF (input));
    size_t i;

    my_u8_possible_linebreaks (input, SIZEOF (input), "UTF-8", p);
    for (i = 0; i < 8; i++)
      {
        ASSERT (p[i] == UC_BREAK_PROHIBITED);
      }
    free (p);
  }
}

int
main ()
{
  test_function (u8_possible_linebreaks, 2);
#undef u8_possible_linebreaks
  test_function (u8_possible_linebreaks, 1);

  return 0;
}
