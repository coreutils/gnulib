/* Line breaking of UTF-32 strings.
   Copyright (C) 2001-2003, 2006-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

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
#include "unilbrk.h"

#include "unilbrk/internal.h"
#include "unilbrk/lbrktables.h"
#include "uniwidth.h"

static int
u32_width_linebreaks_internal (const uint32_t *s, size_t n,
                              int width, int start_column, int at_end_columns,
                              const char *o, const char *encoding, int cr,
                              char *p)
{
  const uint32_t *s_end;
  char *last_p;
  int last_column;
  int piece_width;

  u32_possible_linebreaks_loop (s, n, encoding, cr, p);

  s_end = s + n;
  last_p = NULL;
  last_column = start_column;
  piece_width = 0;
  while (s < s_end)
    {
      ucs4_t uc = *s;

      /* Respect the override.  */
      if (o != NULL && *o != UC_BREAK_UNDEFINED)
        *p = *o;

      if (*p == UC_BREAK_POSSIBLE
          || *p == UC_BREAK_MANDATORY || *p == UC_BREAK_CR_BEFORE_LF)
        {
          /* An atomic piece of text ends here.  */
          if (last_p != NULL && last_column + piece_width > width)
            {
              /* Insert a line break.  */
              *last_p = UC_BREAK_POSSIBLE;
              last_column = 0;
            }
        }

      if (*p == UC_BREAK_MANDATORY || *p == UC_BREAK_CR_BEFORE_LF)
        {
          /* uc is a line break character.  */
          /* Start a new piece at column 0.  */
          last_p = NULL;
          last_column = 0;
          piece_width = 0;
        }
      else
        {
          /* uc is not a line break character.  */
          int w;

          if (*p == UC_BREAK_POSSIBLE)
            {
              /* Start a new piece.  */
              last_p = p;
              last_column += piece_width;
              piece_width = 0;
              /* No line break for the moment, may be turned into
                 UC_BREAK_POSSIBLE later, via last_p. */
            }

          *p = UC_BREAK_PROHIBITED;

          w = uc_width (uc, encoding);
          if (w >= 0) /* ignore control characters in the string */
            piece_width += w;
        }

      s++;
      p++;
      if (o != NULL)
        o++;
    }

  /* The last atomic piece of text ends here.  */
  if (last_p != NULL && last_column + piece_width + at_end_columns > width)
    {
      /* Insert a line break.  */
      *last_p = UC_BREAK_POSSIBLE;
      last_column = 0;
    }

  return last_column + piece_width;
}

#undef u32_width_linebreaks

int
u32_width_linebreaks (const uint32_t *s, size_t n,
                      int width, int start_column, int at_end_columns,
                      const char *o, const char *encoding,
                      char *p)
{
  return u32_width_linebreaks_internal (s, n,
                                        width, start_column, at_end_columns,
                                        o, encoding, -1, p);
}

int
u32_width_linebreaks_v2 (const uint32_t *s, size_t n,
                         int width, int start_column, int at_end_columns,
                         const char *o, const char *encoding,
                         char *p)
{
  return u32_width_linebreaks_internal (s, n,
                                        width, start_column, at_end_columns,
                                        o, encoding, LBP_CR, p);
}
