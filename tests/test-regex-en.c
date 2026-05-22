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
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
main ()
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

  char const *locale = getenv ("LOCALE_EN_UTF8");
  if (!locale)
    locale = "en_US.utf8";
  if (!setlocale (LC_ALL, locale))
    {
      fprintf (stderr, "Skipping test: locale %s absent\n", locale);
      return 77;
    }

  /* https://sourceware.org/ml/libc-hacker/2006-09/msg00008.html
     This test needs valgrind to catch the bug on Debian
     GNU/Linux 3.1 x86, but it might catch the bug better
     on other platforms and it shouldn't hurt to try the
     test here.  */
  {
    static char const pat[] = "insert into";
    static char const data[] =
      "\xFF\0\x12\xA2\xAA\xC4\xB1,K\x12\xC4\xB1*\xACK";
    re_set_syntax (RE_SYNTAX_GREP | RE_HAT_LISTS_NOT_NEWLINE
                   | RE_ICASE);
    memset (&regex, 0, sizeof regex);
    s = re_compile_pattern (pat, sizeof pat - 1, &regex);
    if (s)
      report_error ("%s: %s", pat, s);
    else
      {
        memset (&regs, 0, sizeof regs);
        int ret = re_search (&regex, data, sizeof data - 1,
                             0, sizeof data - 1, &regs);
        if (ret != -1)
          report_error ("re_search '%s' on '%s' returned %d",
                        pat, data, ret);
        regfree (&regex);
        free (regs.start);
        free (regs.end);
      }
  }

  if (really_utf8 ())
    {
      /* This test is from glibc bug 15078.
         The test case is from Andreas Schwab in
         <https://sourceware.org/ml/libc-alpha/2013-01/msg00967.html>.
      */
      static char const pat[] = "[^x]x";
      static char const data[] =
        /* <U1000><U103B><U103D><U1014><U103A><U102F><U1015><U103A> */
        "\xe1\x80\x80"
        "\xe1\x80\xbb"
        "\xe1\x80\xbd"
        "\xe1\x80\x94"
        "\xe1\x80\xba"
        "\xe1\x80\xaf"
        "\xe1\x80\x95"
        "\xe1\x80\xba"
        "x";
      re_set_syntax (0);
      memset (&regex, 0, sizeof regex);
      s = re_compile_pattern (pat, sizeof pat - 1, &regex);
      if (s)
        report_error ("%s: %s", pat, s);
      else
        {
          int ret = re_search (&regex, data, sizeof data - 1,
                               0, sizeof data - 1, NULL);
          if (ret != 0 && ret != 21)
            report_error ("re_search '%s' on '%s' returned %d",
                          pat, data, ret);
          regfree (&regex);
        }
    }

  return exit_status;
}
