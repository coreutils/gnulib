/* Line breaking of UTF-16 strings.
   Copyright (C) 2001-2003, 2006-2010 Free Software Foundation, Inc.
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

#include <config.h>

/* Specification.  */
#include "unilbrk.h"

#include <stdlib.h>
#include <string.h>

#include "unilbrk/lbrktables.h"
#include "uniwidth/cjk.h"
#include "unistr.h"

void
u16_possible_linebreaks (const uint16_t *s, size_t n, const char *encoding, char *p)
{
  int LBP_AI_REPLACEMENT = (is_cjk_encoding (encoding) ? LBP_ID : LBP_AL);
  const uint16_t *s_end = s + n;
  int last_prop = LBP_BK; /* line break property of last non-space character */
  char *seen_space = NULL; /* Was a space seen after the last non-space character? */
  char *seen_space2 = NULL; /* At least two spaces after the last non-space? */

  /* Don't break inside multibyte characters.  */
  memset (p, UC_BREAK_PROHIBITED, n);

  while (s < s_end)
    {
      ucs4_t uc;
      int count = u16_mbtouc_unsafe (&uc, s, s_end - s);
      int prop = unilbrkprop_lookup (uc);

      if (prop == LBP_BK)
        {
          /* Mandatory break.  */
          *p = UC_BREAK_MANDATORY;
          last_prop = LBP_BK;
          seen_space = NULL;
          seen_space2 = NULL;
        }
      else
        {
          char *q;

          /* Resolve property values whose behaviour is not fixed.  */
          switch (prop)
            {
            case LBP_AI:
              /* Resolve ambiguous.  */
              prop = LBP_AI_REPLACEMENT;
              break;
            case LBP_CB:
              /* This is arbitrary.  */
              prop = LBP_ID;
              break;
            case LBP_SA:
              /* We don't handle complex scripts yet.
                 Treat LBP_SA like LBP_XX.  */
            case LBP_XX:
              /* This is arbitrary.  */
              prop = LBP_AL;
              break;
            }

          /* Deal with spaces and combining characters.  */
          q = p;
          if (prop == LBP_SP)
            {
              /* Don't break just before a space.  */
              *p = UC_BREAK_PROHIBITED;
              seen_space2 = seen_space;
              seen_space = p;
            }
          else if (prop == LBP_ZW)
            {
              /* Don't break just before a zero-width space.  */
              *p = UC_BREAK_PROHIBITED;
              last_prop = LBP_ZW;
              seen_space = NULL;
              seen_space2 = NULL;
            }
          else if (prop == LBP_CM)
            {
              /* Don't break just before a combining character, except immediately after a
                 zero-width space.  */
              if (last_prop == LBP_ZW)
                {
                  /* Break after zero-width space.  */
                  *p = UC_BREAK_POSSIBLE;
                  /* A combining character turns a preceding space into LBP_ID.  */
                  last_prop = LBP_ID;
                }
              else
                {
                  *p = UC_BREAK_PROHIBITED;
                  /* A combining character turns a preceding space into LBP_ID.  */
                  if (seen_space != NULL)
                    {
                      q = seen_space;
                      seen_space = seen_space2;
                      prop = LBP_ID;
                      goto lookup_via_table;
                    }
                }
            }
          else
            {
             lookup_via_table:
              /* prop must be usable as an index for table 7.3 of UTR #14.  */
              if (!(prop >= 0 && prop < sizeof (unilbrk_table) / sizeof (unilbrk_table[0])))
                abort ();

              if (last_prop == LBP_BK)
                {
                  /* Don't break at the beginning of a line.  */
                  *q = UC_BREAK_PROHIBITED;
                }
              else if (last_prop == LBP_ZW)
                {
                  /* Break after zero-width space.  */
                  *q = UC_BREAK_POSSIBLE;
                }
              else
                {
                  switch (unilbrk_table [last_prop] [prop])
                    {
                    case D:
                      *q = UC_BREAK_POSSIBLE;
                      break;
                    case I:
                      *q = (seen_space != NULL ? UC_BREAK_POSSIBLE : UC_BREAK_PROHIBITED);
                      break;
                    case P:
                      *q = UC_BREAK_PROHIBITED;
                      break;
                    default:
                      abort ();
                    }
                }
              last_prop = prop;
              seen_space = NULL;
              seen_space2 = NULL;
            }
        }

      s += count;
      p += count;
    }
}
