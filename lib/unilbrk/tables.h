/* Line breaking auxiliary tables.
   Copyright (C) 2001-2003, 2006-2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "unitypes.h"

/* Line breaking classification.  */

enum
{
  /* Values >= 20 are resolved at run time. */
  LBP_BK =  0, /* mandatory break */
/*LBP_CR,         carriage return - not used here because it's a DOSism */
/*LBP_LF,         line feed - not used here because it's a DOSism */
  LBP_CM = 20, /* attached characters and combining marks */
/*LBP_SG,         surrogates - not used here because they are not characters */
  LBP_ZW =  1, /* zero width space */
  LBP_IN =  2, /* inseparable */
  LBP_GL =  3, /* non-breaking (glue) */
  LBP_CB = 22, /* contingent break opportunity */
  LBP_SP = 21, /* space */
  LBP_BA =  4, /* break opportunity after */
  LBP_BB =  5, /* break opportunity before */
  LBP_B2 =  6, /* break opportunity before and after */
  LBP_HY =  7, /* hyphen */
  LBP_NS =  8, /* non starter */
  LBP_OP =  9, /* opening punctuation */
  LBP_CL = 10, /* closing punctuation */
  LBP_QU = 11, /* ambiguous quotation */
  LBP_EX = 12, /* exclamation/interrogation */
  LBP_ID = 13, /* ideographic */
  LBP_NU = 14, /* numeric */
  LBP_IS = 15, /* infix separator (numeric) */
  LBP_SY = 16, /* symbols allowing breaks */
  LBP_AL = 17, /* ordinary alphabetic and symbol characters */
  LBP_PR = 18, /* prefix (numeric) */
  LBP_PO = 19, /* postfix (numeric) */
  LBP_SA = 23, /* complex context (South East Asian) */
  LBP_AI = 24, /* ambiguous (alphabetic or ideograph) */
  LBP_XX = 25  /* unknown */
};

#include "lbrkprop1.h"

static inline unsigned char
unilbrkprop_lookup (ucs4_t uc)
{
  unsigned int index1 = uc >> lbrkprop_header_0;
  if (index1 < lbrkprop_header_1)
    {
      int lookup1 = unilbrkprop.level1[index1];
      if (lookup1 >= 0)
	{
	  unsigned int index2 = (uc >> lbrkprop_header_2) & lbrkprop_header_3;
	  int lookup2 = unilbrkprop.level2[lookup1 + index2];
	  if (lookup2 >= 0)
	    {
	      unsigned int index3 = uc & lbrkprop_header_4;
	      return unilbrkprop.level3[lookup2 + index3];
	    }
	}
    }
  return LBP_XX;
}

/* Table indexed by two line breaking classifications.  */
#define D 1  /* direct break opportunity, empty in table 7.3 of UTR #14 */
#define I 2  /* indirect break opportunity, '%' in table 7.3 of UTR #14 */
#define P 3  /* prohibited break,           '^' in table 7.3 of UTR #14 */

extern const unsigned char unilbrk_table[19][19];

/* We don't support line breaking of complex-context dependent characters
   (Thai, Lao, Myanmar, Khmer) yet, because it requires dictionary lookup. */
