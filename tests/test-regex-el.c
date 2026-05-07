/* Test regular expressions
   Copyright 1996-2001, 2003-2026 Free Software Foundation, Inc.

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

#include "regex.h"

#include <ctype.h>
#include <locale.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if HAVE_DECL_ALARM
# include <unistd.h>
# include <signal.h>
#endif

static int exit_status;

static void
report_error (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  fprintf (stderr, "test-regex: ");
  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  va_end (args);
  exit_status = 1;
}

int
main ()
{
  struct re_pattern_buffer regex;
  const char *s;

#if HAVE_DECL_ALARM
  /* In case a bug causes glibc to go into an infinite loop.
     The tests should take less than 10 s on a reasonably modern CPU.  */
  int alarm_value = 1000;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  /* Test for glibc bug 20381
     <https://sourceware.org/bugzilla/show_bug.cgi?id=20381>.
     Check this only in Greek locales that seem to be working.
     In macOS 26, for example, setlocale (LC_ALL, "el_GR.ISO8859-7")
     succeed but acts like the C locale.  */
  if (! ((setlocale (LC_ALL, "el_GR.iso88597")
          || setlocale (LC_ALL, "el_GR.ISO8859-7")
          || setlocale (LC_ALL, "el_GR.iso8859-7"))
         && toupper (0xf2) == 0xd3 && toupper (0xf3) == 0xd3))
    {
      fprintf (stderr, "Skipping test: no single-byte Greek locale found\n");
      return 77;
    }

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
        static char const str[3][2] = { "\xd3", "\xf2", "\xf3" };
        re_set_syntax (RE_ICASE);
        memset (&regex, 0, sizeof regex);
        s = re_compile_pattern (str[i], 1, &regex);
        if (s)
          {
            report_error ("re_compile_pattern \\x%02x failed: %s",
                          (unsigned char) str[i][0], s);
            continue;
          }
        int without = re_search (&regex, str[j], 1, 0, 1, NULL);
        regfree (&regex);

        memset (&regex, 0, sizeof regex);
        regex.fastmap = malloc (UCHAR_MAX + 1);
        if (!regex.fastmap)
          {
            report_error ("malloc failed");
            continue;
          }
        s = re_compile_pattern (str[i], 1, &regex);
        if (s)
          {
            report_error ("re_compile_pattern \\x%02x failed(!): %s",
                          (unsigned char) str[i][0], s);
            continue;
          }
        int with = re_search (&regex, str[j], 1, 0, 1, NULL);

        if (with != without)
          report_error
            ("fastmap mismatch: pattern = \\x%02x, string = \\x%02x,"
             " with = %d, without = %d",
             (unsigned char) str[i][0], (unsigned char) str[j][0],
             with, without);

        regfree (&regex);
      }

  return exit_status;
}
