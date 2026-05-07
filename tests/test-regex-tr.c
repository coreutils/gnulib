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

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#if HAVE_DECL_ALARM
# include <unistd.h>
# include <signal.h>
#endif

#include "localcharset.h"

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

/* Check whether it's really a UTF-8 locale.
   On mingw, setlocale (LC_ALL, "en_US.UTF-8") succeeds but returns
   "English_United States.1252", with locale_charset () returning "CP1252".  */
static int
really_utf8 (void)
{
  return streq (locale_charset (), "UTF-8");
}

int
main (int argc, char **argv)
{
  struct re_pattern_buffer regex;
  const char *s;
  struct re_registers regs;

#if HAVE_DECL_ALARM
  /* In case a bug causes glibc to go into an infinite loop.
     The tests should take less than 10 s on a reasonably modern CPU.  */
  int alarm_value = 1000;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  if (! (setlocale (LC_ALL, "tr_TR.UTF-8")
         && really_utf8 ()
         && towupper (L'i') == 0x0130 /* U+0130; see below.  */))
    {
      fprintf (stderr, "Skipping test: no single-byte Greek locale found\n");
      return 77;
    }

  re_set_syntax (RE_SYNTAX_GREP | RE_ICASE);
  memset (&regex, 0, sizeof regex);
  static char const pat[] = "i";
  s = re_compile_pattern (pat, sizeof pat - 1, &regex);
  if (s)
    report_error ("%s: %s", pat, s);
  else
    {
      /* UTF-8 encoding of U+0130 LATIN CAPITAL LETTER I WITH DOT ABOVE.
         In Turkish, this is the upper-case equivalent of ASCII "i".
         Older versions of Gnulib failed to match "i" to U+0130 when
         ignoring case in Turkish <https://bugs.gnu.org/43577>.  */
      static char const data[] = "\xc4\xb0";

      memset (&regs, 0, sizeof regs);
      int ret =
        re_search (&regex, data, sizeof data - 1, 0, sizeof data - 1,
                   &regs);
      if (ret != 0)
        report_error ("re_search '%s' on '%s' returned %d",
                      pat, data, ret);
      regfree (&regex);
      free (regs.start);
      free (regs.end);
    }

  return exit_status;
}
