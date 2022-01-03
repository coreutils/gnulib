/* Word break auxiliary table.  -*- coding: utf-8 -*-
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

#include <config.h>

/* Specification.  */
#include "wbrktable.h"

const int uniwbrk_prop_index[23] =
{
  /* WBP_OTHER */         0,
  /* WBP_KATAKANA */      1,
  /* WBP_ALETTER */       2,
  /* WBP_MIDNUMLET */     3,
  /* WBP_MIDLETTER */     4,
  /* WBP_MIDNUM */        5,
  /* WBP_NUMERIC */       6,
  /* WBP_EXTENDNUMLET */  7,
  /* WBP_EXTEND */       -1,
  /* WBP_FORMAT */       -1,
  /* WBP_NEWLINE */      -1,
  /* WBP_CR */           -1,
  /* WBP_LF */           -1,
  /* WBP_RI */           -1,
  /* WBP_DQ */            8,
  /* WBP_SQ */            9,
  /* WBP_HL */           10,
  /* WBP_ZWJ */          -1,
  /* WBP_EB */           -1,
  /* WBP_EM */           -1,
  /* WBP_GAZ */          -1,
  /* WBP_EBG */          -1,
  /* WBP_WSS */          11
};

/* This table contains the following rules (see UAX #29):

                           last         current

                     (ALetter | HL) × (ALetter | HL)                       (WB5)
                     (ALetter | HL) × Numeric                              (WB9)
                                 HL × SQ                                   (WB7a)
                            Numeric × (ALetter | HL)                       (WB10)
                            Numeric × Numeric                              (WB8)
                           Katakana × Katakana                             (WB13)
(ALetter | HL | Numeric | Katakana) × ExtendNumLet                         (WB13a)
                       ExtendNumLet × ExtendNumLet                         (WB13a)
                       ExtendNumLet × (ALetter | HL | Numeric | Katakana)  (WB13b)

   Note that the following rules are not handled here but in the loop in u-wordbreaks.h:
   - The rules need to look back or look ahead the second character (WB6, WB7, WB7b, WB7c, WB11, WB12)
   - The rules with a higher precedence over the "ignore" rule (WB4), such as WB3c
 */

const unsigned char uniwbrk_table[12][12] =
{        /* current:        OTHER       MIDNUMLET   NUMERIC     SQ          */
         /*                     KATAKANA    MIDLETTER   EXNUMLET    HL      */
         /*                         ALETTER     MIDNUM      DQ          WSS */
  /* last */
  /* WBP_OTHER */        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_KATAKANA */     {  1,  0,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1 },
  /* WBP_ALETTER */      {  1,  1,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1 },
  /* WBP_MIDNUMLET */    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_MIDLETTER */    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_MIDNUM */       {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_NUMERIC */      {  1,  1,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1 },
  /* WBP_EXTENDNUMLET */ {  1,  0,  0,  1,  1,  1,  0,  0,  1,  1,  0,  1 },
  /* WBP_DQ */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_SQ */           {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
  /* WBP_HL */           {  1,  1,  0,  1,  1,  1,  0,  0,  1,  0,  0,  1 },
  /* WBP_WSS */          {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 }
};
