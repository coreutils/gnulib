/* Word break auxiliary table.  -*- coding: utf-8 -*-
   Copyright (C) 2009-2018 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "wbrktable.h"

const int uniwbrk_prop_index[22] =
{
   0, /* WBP_OTHER */
   1, /* WBP_KATAKANA */
   2, /* WBP_ALETTER */
   3, /* WBP_MIDNUMLET */
   4, /* WBP_MIDLETTER */
   5, /* WBP_MIDNUM */
   6, /* WBP_NUMERIC */
   7, /* WBP_EXTENDNUMLET */
  -1, /* WBP_EXTEND */
  -1, /* WBP_FORMAT */
  -1, /* WBP_NEWLINE */
  -1, /* WBP_CR */
  -1, /* WBP_LF */
  -1, /* WBP_RI */
   8, /* WBP_DQ */
   9, /* WBP_SQ */
  10, /* WBP_HL */
  -1, /* WBP_ZWJ */
  11, /* WBP_EB */
  12, /* WBP_EM */
  -1, /* WBP_GAZ */
  13  /* WBP_EBG */
};

/* This table contains the following rules (see UAX #29):

                           last         current

                     (ALetter | HL) × (ALetter | HL)                  (WB5)
                     (ALetter | HL) × Numeric                         (WB9)
                                 HL × SQ                              (WB7a)
                            Numeric × (ALetter | HL)                  (WB10)
                            Numeric × Numeric                         (WB8)
                           Katakana × Katakana                        (WB13)
(ALetter | HL | Numeric | Katakana) × ExtendNumLet                    (WB13a)
                       ExtendNumLet × ExtendNumLet                    (WB13a)
                   ExtendNumLet × (ALetter | HL | Numeric | Katakana) (WB13b)
                     (E_Base | EBG) × E_Modifier                      (WB14)

   Note that the following rules are not handled here but in the loop in u-wordbreaks.h:
   - The rules need to look back or look ahead the second character (WB6, WB7, WB7b, WB7c, WB11, WB12)
   - The rules with a higher precedence over the "ignore" rule (WB4), such as WB3c
 */

const unsigned char uniwbrk_table[14][14] =
{        /* current:        OTHER       MIDNUMLET   NUMERIC     SQ          EM      */
         /*                     KATAKANA    MIDLETTER   EXNUMLET    HL          EBG */
         /*                         ALETTER     MIDNUM      DQ          EB          */
  /* last */
  /* WBP_OTHER */        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_KATAKANA */     {  1,  0,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1 },
  /* WBP_ALETTER */      {  1,  1,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  1 },
  /* WBP_MIDNUMLET */    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_MIDLETTER */    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_MIDNUM */       {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_NUMERIC */      {  1,  1,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  1 },
  /* WBP_EXTENDNUMLET */ {  1,  0,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  1 },
  /* WBP_DQ */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_SQ */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_HL */           {  1,  1,  0,  1,  1,  1,  0,  0,  1,  0,  0,  1,  1,  1 },
  /* WBP_EB */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1 },
  /* WBP_EM */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_EBG */          {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1 }
};
