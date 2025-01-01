/* Indic_Conjunct_Break values.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2024.

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
#include "unictype.h"

static const char u_indic_conjunct_break_name[4][10] =
  {
#define ELEM(tag,string) string,
#include "unictype/incb_name.h"
#undef ELEM
  };

const char *
uc_indic_conjunct_break_name (int indic_conjunct_break)
{
  if (indic_conjunct_break >= 0
      && indic_conjunct_break < sizeof (u_indic_conjunct_break_name) / sizeof (u_indic_conjunct_break_name[0]))
    return u_indic_conjunct_break_name[indic_conjunct_break];
  return NULL;
}
