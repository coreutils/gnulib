/* Test whether a Unicode character is case-ignorable.
   Copyright (C) 2009 Free Software Foundation, Inc.
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "caseprop.h"

#include "unictype.h"
#include "uniwbrk.h"

bool
uc_is_case_ignorable (ucs4_t uc)
{
  /* Quoting the Unicode standard:
       Definition: A character is defined to be "case-ignorable" if it has the
       value MidLetter {or the value MidNumLet} for the Word_Break property or
       its General_Category is one of Nonspacing_Mark (Mn), Enclosing_Mark (Me),
       Format (Cf), Modifier_Letter (Lm), or Modifier_Symbol (Sk).
     The text marked in braces was added in Unicode 5.1.0, see
     <http://www.unicode.org/versions/Unicode5.1.0/> section "Update of
     Definition of case-ignorable".   */

  int wbp = uc_wordbreak_property (uc);

  return (wbp == WBP_MIDLETTER || wbp == WBP_MIDNUMLET
	  || uc_is_general_category_withtable (uc, UC_CATEGORY_MASK_Mn
						   | UC_CATEGORY_MASK_Me
						   | UC_CATEGORY_MASK_Cf
						   | UC_CATEGORY_MASK_Lm
						   | UC_CATEGORY_MASK_Sk));
}
