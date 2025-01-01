/* Grapheme cluster break function test.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff <blp@cs.stanford.edu>, 2010. */

#include <config.h>

/* Specification. */
#include <unigbrk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unictype.h"

const char *
graphemebreakproperty_to_string (int gbp)
{
  switch (gbp)
    {
#define CASE(VALUE) case GBP_##VALUE: return #VALUE;
      CASE(OTHER)
      CASE(CR)
      CASE(LF)
      CASE(CONTROL)
      CASE(EXTEND)
      CASE(PREPEND)
      CASE(SPACINGMARK)
      CASE(L)
      CASE(V)
      CASE(T)
      CASE(LV)
      CASE(LVT)
      CASE(RI)
      CASE(ZWJ)
      CASE(EB)
      CASE(EM)
      CASE(GAZ)
      CASE(EBG)
    }
  abort ();
}

int
main (int argc, char *argv[])
{
  const char *filename;
  FILE *stream;
  int exit_code;
  int lineno;
  char line[1024];

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s FILENAME\n"
               "where FILENAME is the location of the GraphemeBreakTest.txt\n"
               "test file.\n", argv[0]);
      exit (1);
    }

  filename = argv[1];
  stream = fopen (filename, "r");
  if (stream == NULL)
    {
      fprintf (stderr, "error during fopen of '%s'\n", filename);
      exit (1);
    }

  exit_code = 0;
  lineno = 0;
  while (fgets (line, sizeof (line), stream))
    {
      lineno++;

      /* Cut off the trailing comment, if any.  */
      char *comment = strchr (line, '#');
      if (comment != NULL)
        *comment = '\0';
      /* Is the remaining line blank?  */
      if (line[strspn (line, " \t\r\n")] == '\0')
        continue;

      const char *p;
      ucs4_t prev;
      int last_char_prop;
      bool incb_consonant_extended;
      bool incb_consonant_extended_linker;
      bool incb_consonant_extended_linker_extended;
      bool emoji_modifier_sequence;
      bool emoji_modifier_sequence_before_last_char;
      size_t ri_count;

      last_char_prop = -1;
      incb_consonant_extended = false;
      incb_consonant_extended_linker = false;
      incb_consonant_extended_linker_extended = false;
      emoji_modifier_sequence = false;
      emoji_modifier_sequence_before_last_char = false;
      ri_count = 0;
      prev = 0;
      p = line;
      do
        {
          bool should_break;
          ucs4_t next;

          p += strspn (p, " \t\r\n");
          if (!strncmp (p, "\303\267" /* ÷ */, 2))
            {
              should_break = true;
              p += 2;
            }
          else if (!strncmp (p, "\303\227" /* × */, 2))
            {
              should_break = false;
              p += 2;
            }
          else
            {
              fprintf (stderr, "%s:%d.%d: syntax error expecting '÷' or '×'\n",
                       filename, lineno, (int) (p - line + 1));
              exit (1);
            }

          p += strspn (p, " \t\r\n");
          if (*p == '\0')
            next = 0;
          else
            {
              unsigned int next_int;
              int n;

              if (sscanf (p, "%x%n", &next_int, &n) != 1)
                {
                  fprintf (stderr, "%s:%d.%d: syntax error at '%s' "
                           "expecting hexadecimal Unicode code point number\n",
                           filename, lineno, (int) (p - line + 1), p);
                  exit (1);
                }
              p += n;

              next = next_int;
            }

          int incb = uc_indic_conjunct_break (next);

          /* Skip unsupported rules involving 3 or more characters.  */
          if (incb_consonant_extended_linker_extended
              && incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT)
            fprintf (stderr, "%s:%d: skipping GB9c: should join U+%04X "
                     "and U+%04X\n",
                     filename, lineno, prev, next);
          else if (last_char_prop == GBP_ZWJ
                   && emoji_modifier_sequence_before_last_char
                   && uc_is_property_extended_pictographic (next))
            {
              int prev_gbp = uc_graphemeclusterbreak_property (prev);
              int next_gbp = uc_graphemeclusterbreak_property (next);
              fprintf (stderr, "%s:%d: skipping GB11: should join U+%04X (%s) "
                       "and U+%04X (%s)\n",
                       filename, lineno,
                       prev, graphemebreakproperty_to_string (prev_gbp),
                       next, graphemebreakproperty_to_string (next_gbp));
            }
          else if (uc_graphemeclusterbreak_property (next) == GBP_RI
                   && ri_count % 2 != 0)
            {
              int prev_gbp = uc_graphemeclusterbreak_property (prev);
              int next_gbp = uc_graphemeclusterbreak_property (next);
              fprintf (stderr, "%s:%d: skipping GB12: should join U+%04X (%s) "
                       "and U+%04X (%s)\n",
                       filename, lineno,
                       prev, graphemebreakproperty_to_string (prev_gbp),
                       next, graphemebreakproperty_to_string (next_gbp));
            }
          else if (uc_is_grapheme_break (prev, next) != should_break)
            {
              int prev_gbp = uc_graphemeclusterbreak_property (prev);
              int next_gbp = uc_graphemeclusterbreak_property (next);
              fprintf (stderr, "%s:%d: should %s U+%04X (%s) and "
                       "U+%04X (%s)\n",
                       filename, lineno,
                       should_break ? "break" : "join",
                       prev, graphemebreakproperty_to_string (prev_gbp),
                       next, graphemebreakproperty_to_string (next_gbp));
              exit_code = 1;
            }

          p += strspn (p, " \t\r\n");
          prev = next;

          incb_consonant_extended_linker =
            incb_consonant_extended && incb == UC_INDIC_CONJUNCT_BREAK_LINKER;
          incb_consonant_extended_linker_extended =
            (incb_consonant_extended_linker
             || (incb_consonant_extended_linker_extended
                 && incb >= UC_INDIC_CONJUNCT_BREAK_LINKER));
          incb_consonant_extended =
            (incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT
             || (incb_consonant_extended
                 && incb >= UC_INDIC_CONJUNCT_BREAK_LINKER));

          emoji_modifier_sequence_before_last_char = emoji_modifier_sequence;
          emoji_modifier_sequence =
            (emoji_modifier_sequence
             && uc_graphemeclusterbreak_property (next) == GBP_EXTEND)
            || uc_is_property_extended_pictographic (next);

          last_char_prop = uc_graphemeclusterbreak_property (next);

          if (uc_graphemeclusterbreak_property (next) == GBP_RI)
            ri_count++;
          else
            ri_count = 0;
        }
      while (*p != '\0');
    }

  return exit_code;
}
