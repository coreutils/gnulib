/* Bidi categories of Unicode characters.
   Copyright (C) 2002, 2006, 2009-2010 Free Software Foundation, Inc.
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

int
uc_bidi_category_byname (const char *category_name)
{
  switch (category_name[0])
    {
    case 'A':
      switch (category_name[1])
        {
        case 'L':
          if (category_name[2] == '\0')
            return UC_BIDI_AL;
          break;
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_AN;
          break;
        }
      break;
    case 'B':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_B;
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_BN;
          break;
        }
      break;
    case 'C':
      switch (category_name[1])
        {
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_CS;
          break;
        }
      break;
    case 'E':
      switch (category_name[1])
        {
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_EN;
          break;
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_ES;
          break;
        case 'T':
          if (category_name[2] == '\0')
            return UC_BIDI_ET;
          break;
        }
      break;
    case 'L':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_L;
        case 'R':
          switch (category_name[2])
            {
            case 'E':
              if (category_name[3] == '\0')
                return UC_BIDI_LRE;
              break;
            case 'O':
              if (category_name[3] == '\0')
                return UC_BIDI_LRO;
              break;
            }
          break;
        }
      break;
    case 'N':
      switch (category_name[1])
        {
        case 'S':
          switch (category_name[2])
            {
            case 'M':
              if (category_name[3] == '\0')
                return UC_BIDI_NSM;
              break;
            }
          break;
        }
      break;
    case 'O':
      switch (category_name[1])
        {
        case 'N':
          if (category_name[2] == '\0')
            return UC_BIDI_ON;
          break;
        }
      break;
    case 'P':
      switch (category_name[1])
        {
        case 'D':
          switch (category_name[2])
            {
            case 'F':
              if (category_name[3] == '\0')
                return UC_BIDI_PDF;
              break;
            }
          break;
        }
      break;
    case 'R':
      switch (category_name[1])
        {
        case '\0':
          return UC_BIDI_R;
        case 'L':
          switch (category_name[2])
            {
            case 'E':
              if (category_name[3] == '\0')
                return UC_BIDI_RLE;
              break;
            case 'O':
              if (category_name[3] == '\0')
                return UC_BIDI_RLO;
              break;
            }
          break;
        }
      break;
    case 'S':
      if (category_name[1] == '\0')
        return UC_BIDI_S;
      break;
    case 'W':
      switch (category_name[1])
        {
        case 'S':
          if (category_name[2] == '\0')
            return UC_BIDI_WS;
          break;
        }
      break;
    }
  /* Invalid category name.  */
  return -1;
}
