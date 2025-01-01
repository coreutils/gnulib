/* Test the Indic_Conjunct_Break value functions.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2024.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unictype.h"

#include <string.h>

#include "macros.h"

int
main ()
{
  ASSERT (uc_indic_conjunct_break_byname ("NONE") == UC_INDIC_CONJUNCT_BREAK_NONE);
  ASSERT (uc_indic_conjunct_break_byname ("None") == UC_INDIC_CONJUNCT_BREAK_NONE);
  ASSERT (uc_indic_conjunct_break_byname ("CONSONANT") == UC_INDIC_CONJUNCT_BREAK_CONSONANT);
  ASSERT (uc_indic_conjunct_break_byname ("Consonant") == UC_INDIC_CONJUNCT_BREAK_CONSONANT);
  ASSERT (uc_indic_conjunct_break_byname ("LINKER") == UC_INDIC_CONJUNCT_BREAK_LINKER);
  ASSERT (uc_indic_conjunct_break_byname ("Linker") == UC_INDIC_CONJUNCT_BREAK_LINKER);
  ASSERT (uc_indic_conjunct_break_byname ("EXTEND") == UC_INDIC_CONJUNCT_BREAK_EXTEND);
  ASSERT (uc_indic_conjunct_break_byname ("Extend") == UC_INDIC_CONJUNCT_BREAK_EXTEND);
  ASSERT (uc_indic_conjunct_break_byname ("VOWEL") < 0);
  ASSERT (uc_indic_conjunct_break_byname ("X") < 0);
  ASSERT (uc_indic_conjunct_break_byname ("") < 0);

  return test_exit_status;
}
