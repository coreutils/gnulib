/* Ad-hoc testing program for <termcap.h>.
   Copyright (C) 2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "terminfo.h"

#include <stdio.h>
/* Get getenv().  */
#include <stdlib.h>
/* Get STDOUT_FILENO.  */
#include <unistd.h>

int
main (int argc, char *argv[])
{
  const char *underline_on = NULL;
  const char *underline_off = NULL;
  const char *bold_on = NULL;
  const char *invert_on = NULL;
  const char *attributes_off = NULL;

  {
    const char *term = getenv ("TERM");
    if (term != NULL && *term != '\0')
      {
        #if HAVE_TERMINFO
        int err = 1;
        if (setupterm (term, STDOUT_FILENO, &err) == 0 || err == 1)
          {
            underline_on = tigetstr ("smul");
            underline_off = tigetstr ("rmul");
            bold_on = tigetstr ("bold");
            invert_on = tigetstr ("rev");
            attributes_off = tigetstr ("sgr0");
          }
        #elif HAVE_TERMCAP
        static char tbuf[2048];
        if (tgetent (tbuf, term) > 0)
          {
            underline_on = tgetstr ("us", NULL);
            underline_off = tgetstr ("ue", NULL);
            bold_on = tgetstr ("md", NULL);
            invert_on = tgetstr ("mr", NULL);
            attributes_off = tgetstr ("me", NULL);
          }
        #endif
      }
  }

  if (bold_on) tputs (bold_on, 1, putchar);
  printf ("#include");
  if (attributes_off) tputs (attributes_off, 1, putchar);
  printf (" <stdio.h>\n");
  if (underline_on) tputs (underline_on, 1, putchar);
  printf ("int");
  if (underline_off) tputs (underline_off, 1, putchar);
  printf ("\n");
  if (invert_on) tputs (invert_on, 1, putchar);
  printf ("main");
  if (attributes_off) tputs (attributes_off, 1, putchar);
  printf (" ()\n");
  printf ("{\n");
  printf ("  printf (\"Hello world\\n\");\n");
  printf ("  ");
  if (bold_on) tputs (bold_on, 1, putchar);
  printf ("return");
  if (attributes_off) tputs (attributes_off, 1, putchar);
  printf (" 0;\n");
  printf ("}\n");

  return 0;
}
