/* Line break function test, using test data from UCD.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification. */
#include <unilbrk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  const char *filename;
  FILE *stream;
  int exit_code;
  int lineno;
  char line[16384];

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s FILENAME\n"
               "where FILENAME is the location of the LineBreakTest.txt test file.\n",
               argv[0]);
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
      uint32_t input[1024];
      char breaks[1024];
      char breaks_expected[1025];
      int i;

      i = 0;
      p = line;
      do
        {
          p += strspn (p, " \t\r\n");
          if (!strncmp (p, "\303\267" /* ÷ */, 2))
            {
              breaks_expected[i] = 1;
              p += 2;
            }
          else if (!strncmp (p, "\303\227" /* × */, 2))
            {
              breaks_expected[i] = 0;
              p += 2;
            }
          else
            {
              fprintf (stderr, "%s:%d.%d: syntax error expecting '÷' or '×'\n",
                       filename, lineno, (int) (p - line + 1));
              exit (1);
            }

          p += strspn (p, " \t\r\n");
          if (*p != '\0')
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

              input[i] = next_int;
            }

          p += strspn (p, " \t\r\n");
          i++;
        }
      while (*p != '\0');

      u32_possible_linebreaks (input, i - 1, "UTF-8", breaks);

      int matches = 1;
      {
        int j;
        for (j = 0; j < i - 1; j++)
          {
            /* The character U+FFFC has line break property CB, which according
               to rule (LB1) is resolved "into other line breaking classes
               depending on criteria outside the scope of this algorithm".
               Thus it makes no sense to check the breaks[] entry before or
               after such a character.  */
            if (!(input[j] == 0xFFFC
                  || (j > 0 && input[j - 1] == 0xFFFC)
                  /* Also consider intervening characters with property LBP_CM
                     or LBP_ZWJ, per (LB9).  */
                  || (j > 1 && (input[j - 1] == 0x0308 || input[j - 1] == 0x200D)
                      && input[j - 2] == 0xFFFC)))
              /* A regional indicator with a combining character is nonsense,
                 because regional indicators are supposed to come in pairs.  */
              if (!(j >= 2 && (input[0] >= 0x1F1E6 && input[0] <= 0x1F1FF)
                    && input[1] == 0x0308))
                /* It is nonsense to treat U+1F8FF differently than U+1F02C.
                   Both are unassigned Extended_Pictographic characters and
                   should therefore be treated like LBP_EB (or LBP_ID, if you
                   want), not like LBP_AL.  See rule (LB30b).  */
                if (!(input[j] == 0x1F8FF
                      || (j > 0 && input[j - 1] == 0x1F8FF)
                      /* Also consider intervening characters with property LBP_CM
                         or LBP_ZWJ, per (LB9).  */
                      || (j > 1 && (input[j - 1] == 0x0308 || input[j - 1] == 0x200D)
                          && input[j - 2] == 0x1F8FF)))
                  /* There is a disagreement regarding whether to allow a line break
                     after a U+0020 SPACE character at the start of the text.
                     We consider that the start of the text is equivalent to the
                     state after a newline was seen; hence the loop starts with
                     property LBP_BK.  By the rules (LB4,LB5,LB6) an extra line
                     break after a mandatory line break is undesired, even with
                     intervening spaces (because these rules come before (LB18)).
                     Whereas the LineBreakTest.txt file allows a line break after
                     the space.
                     Similarly when the first two characters at the start of the
                     text have property LBP_CM and LBP_ZWJ, respectively. (LB9).  */
                  if (!(((j == 1 || (j > 1 && ((input[j - 2] >= 0x000A && input[j - 2] <= 0x000D) || input[j - 2] == 0x0085)))
                         && input[j - 1] == 0x0020)
                        || ((j == 2 || (j > 2 && ((input[j - 3] >= 0x000A && input[j - 3] <= 0x000D) || input[j - 3] == 0x0085)))
                            && ((input[j - 2] == 0x0020 && input[j - 1] == 0x0020)
                                || (input[j - 2] == 0x0308 && input[j - 1] == 0x200D)
                                || (input[j - 2] == 0x200D && input[j - 1] == 0x0308)))))
                    matches &= (!(breaks[j] == UC_BREAK_PROHIBITED
                                  || breaks[j] == UC_BREAK_MANDATORY
                                  || breaks[j] == UC_BREAK_CR_BEFORE_LF)
                                || (j > 0 && breaks[j - 1] == UC_BREAK_MANDATORY))
                               == breaks_expected[j];
          }
      }
      if (!matches)
        {
          int j;

          fprintf (stderr, "%s:%d: expected: ", filename, lineno);
          for (j = 0; j < i - 1; j++)
            fprintf (stderr, "%s U+%04X ",
                     breaks_expected[j] == 1 ? "\303\267" : "\303\227",
                     input[j]);
          fprintf (stderr, "\n");
          fprintf (stderr, "%s:%d: actual:   ", filename, lineno);
          for (j = 0; j < i - 1; j++)
            fprintf (stderr, "%s U+%04X ",
                     (!(breaks[j] == UC_BREAK_PROHIBITED
                        || breaks[j] == UC_BREAK_MANDATORY
                        || breaks[j] == UC_BREAK_CR_BEFORE_LF)
                      || (j > 0 && breaks[j - 1] == UC_BREAK_MANDATORY))
                     ? "\303\267" : "\303\227",
                     input[j]);
          fprintf (stderr, "\n");
          exit_code = 1;
        }
    }

  return exit_code;
}
