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
  ASSERT (strcmp (uc_indic_conjunct_break_name (UC_INDIC_CONJUNCT_BREAK_NONE),      "None") == 0);
  ASSERT (strcmp (uc_indic_conjunct_break_name (UC_INDIC_CONJUNCT_BREAK_CONSONANT), "Consonant") == 0);
  ASSERT (strcmp (uc_indic_conjunct_break_name (UC_INDIC_CONJUNCT_BREAK_LINKER),    "Linker") == 0);
  ASSERT (strcmp (uc_indic_conjunct_break_name (UC_INDIC_CONJUNCT_BREAK_EXTEND),    "Extend") == 0);
  ASSERT (uc_indic_conjunct_break_name (-1) == NULL);

  return test_exit_status;
}
