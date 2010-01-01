/* Categories of Unicode characters.
   Copyright (C) 2002, 2006-2007, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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
#include "unictype.h"

uc_general_category_t
uc_general_category_byname (const char *category_name)
{
  if (category_name[0] != '\0'
      && (category_name[1] == '\0' || category_name[2] == '\0'))
    switch (category_name[0])
      {
      case 'L':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_L;
          case 'u': return UC_CATEGORY_Lu;
          case 'l': return UC_CATEGORY_Ll;
          case 't': return UC_CATEGORY_Lt;
          case 'm': return UC_CATEGORY_Lm;
          case 'o': return UC_CATEGORY_Lo;
          }
        break;
      case 'M':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_M;
          case 'n': return UC_CATEGORY_Mn;
          case 'c': return UC_CATEGORY_Mc;
          case 'e': return UC_CATEGORY_Me;
          }
        break;
      case 'N':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_N;
          case 'd': return UC_CATEGORY_Nd;
          case 'l': return UC_CATEGORY_Nl;
          case 'o': return UC_CATEGORY_No;
          }
        break;
      case 'P':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_P;
          case 'c': return UC_CATEGORY_Pc;
          case 'd': return UC_CATEGORY_Pd;
          case 's': return UC_CATEGORY_Ps;
          case 'e': return UC_CATEGORY_Pe;
          case 'i': return UC_CATEGORY_Pi;
          case 'f': return UC_CATEGORY_Pf;
          case 'o': return UC_CATEGORY_Po;
          }
        break;
      case 'S':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_S;
          case 'm': return UC_CATEGORY_Sm;
          case 'c': return UC_CATEGORY_Sc;
          case 'k': return UC_CATEGORY_Sk;
          case 'o': return UC_CATEGORY_So;
          }
        break;
      case 'Z':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_Z;
          case 's': return UC_CATEGORY_Zs;
          case 'l': return UC_CATEGORY_Zl;
          case 'p': return UC_CATEGORY_Zp;
          }
        break;
      case 'C':
        switch (category_name[1])
          {
          case '\0': return UC_CATEGORY_C;
          case 'c': return UC_CATEGORY_Cc;
          case 'f': return UC_CATEGORY_Cf;
          case 's': return UC_CATEGORY_Cs;
          case 'o': return UC_CATEGORY_Co;
          case 'n': return UC_CATEGORY_Cn;
          }
        break;
      }
  /* Invalid category name.  */
  return _UC_CATEGORY_NONE;
}
