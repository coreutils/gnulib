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

/* Tests supposed to match; copied from glibc posix/bug-regex11.c.  */
static struct
{
  const char *pattern;
  const char *string;
  int flags, nmatch;
  regmatch_t rm[5];
} const tests[] = {
  /* Test for newline handling in regex.  */
  { "[^~]*~", "\nx~y", 0, 2, { { 0, 3 }, { -1, -1 } } },
  /* Other tests.  */
  { "a(.*)b", "a b", REG_EXTENDED, 2, { { 0, 3 }, { 1, 2 } } },
  { ".*|\\([KIO]\\)\\([^|]*\\).*|?[KIO]", "10~.~|P|K0|I10|O16|?KSb", 0, 3,
    { { 0, 21 }, { 15, 16 }, { 16, 18 } } },
  { ".*|\\([KIO]\\)\\([^|]*\\).*|?\\1", "10~.~|P|K0|I10|O16|?KSb", 0, 3,
    { { 0, 21 }, { 8, 9 }, { 9, 10 } } },
  { "^\\(a*\\)\\1\\{9\\}\\(a\\{0,9\\}\\)\\([0-9]*;.*[^a]\\2\\([0-9]\\)\\)",
    "a1;;0a1aa2aaa3aaaa4aaaaa5aaaaaa6aaaaaaa7aaaaaaaa8aaaaaaaaa9aa2aa1a0", 0,
    5, { { 0, 67 }, { 0, 0 }, { 0, 1 }, { 1, 67 }, { 66, 67 } } },
  /* Test for BRE expression anchoring.  POSIX says just that this may match;
     in glibc regex it always matched, so avoid changing it.  */
  { "\\(^\\|foo\\)bar", "bar", 0, 2, { { 0, 3 }, { -1, -1 } } },
  { "\\(foo\\|^\\)bar", "bar", 0, 2, { { 0, 3 }, { -1, -1 } } },
  /* In ERE this must be treated as an anchor.  */
  { "(^|foo)bar", "bar", REG_EXTENDED, 2, { { 0, 3 }, { -1, -1 } } },
  { "(foo|^)bar", "bar", REG_EXTENDED, 2, { { 0, 3 }, { -1, -1 } } },
  /* Here ^ cannot be treated as an anchor according to POSIX.  */
  { "(^|foo)bar", "(^|foo)bar", 0, 2, { { 0, 10 }, { -1, -1 } } },
  { "(foo|^)bar", "(foo|^)bar", 0, 2, { { 0, 10 }, { -1, -1 } } },
  /* More tests on backreferences.  */
  { "()\\1", "x", REG_EXTENDED, 2, { { 0, 0 }, { 0, 0 } } },
  { "()x\\1", "x", REG_EXTENDED, 2, { { 0, 1 }, { 0, 0 } } },
  { "()\\1*\\1*", "", REG_EXTENDED, 2, { { 0, 0 }, { 0, 0 } } },
  { "([0-9]).*\\1(a*)", "7;7a6", REG_EXTENDED, 3, { { 0, 4 }, { 0, 1 }, { 3, 4 } } },
  { "([0-9]).*\\1(a*)", "7;7a", REG_EXTENDED, 3, { { 0, 4 }, { 0, 1 }, { 3, 4 } } },
  { "(b)()c\\1", "bcb", REG_EXTENDED, 3, { { 0, 3 }, { 0, 1 }, { 1, 1 } } },
  { "()(b)c\\2", "bcb", REG_EXTENDED, 3, { { 0, 3 }, { 0, 0 }, { 0, 1 } } },
  { "a(b)()c\\1", "abcb", REG_EXTENDED, 3, { { 0, 4 }, { 1, 2 }, { 2, 2 } } },
  { "a()(b)c\\2", "abcb", REG_EXTENDED, 3, { { 0, 4 }, { 1, 1 }, { 1, 2 } } },
  { "()(b)\\1c\\2", "bcb", REG_EXTENDED, 3, { { 0, 3 }, { 0, 0 }, { 0, 1 } } },
  { "(b())\\2\\1", "bbbb", REG_EXTENDED, 3, { { 0, 2 }, { 0, 1 }, { 1, 1 } } },
  { "a()(b)\\1c\\2", "abcb", REG_EXTENDED, 3, { { 0, 4 }, { 1, 1 }, { 1, 2 } } },
  { "a()d(b)\\1c\\2", "adbcb", REG_EXTENDED, 3, { { 0, 5 }, { 1, 1 }, { 2, 3 } } },
  { "a(b())\\2\\1", "abbbb", REG_EXTENDED, 3, { { 0, 3 }, { 1, 2 }, { 2, 2 } } },
  { "(bb())\\2\\1", "bbbb", REG_EXTENDED, 3, { { 0, 4 }, { 0, 2 }, { 2, 2 } } },
  { "^([^,]*),\\1,\\1$", "a,a,a", REG_EXTENDED, 2, { { 0, 5 }, { 0, 1 } } },
  { "^([^,]*),\\1,\\1$", "ab,ab,ab", REG_EXTENDED, 2, { { 0, 8 }, { 0, 2 } } },
  { "^([^,]*),\\1,\\1,\\1$", "abc,abc,abc,abc", REG_EXTENDED, 2,
    { { 0, 15 }, { 0, 3 } } },
  { "^(.?)(.?)(.?)(.?)(.?).?\\5\\4\\3\\2\\1$",
    "level", REG_NOSUB | REG_EXTENDED, 0, { { -1, -1 } } },
  { "^(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.).?\\9\\8\\7\\6\\5\\4\\3\\2\\1$|^.?$",
    "level", REG_NOSUB | REG_EXTENDED, 0, { { -1, -1 } } },
  { "^(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.).?\\9\\8\\7\\6\\5\\4\\3\\2\\1$|^.?$",
    "abcdedcba", REG_EXTENDED, 1, { { 0, 9 } } },
  { "^(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.).?\\9\\8\\7\\6\\5\\4\\3\\2\\1$|^.?$",
    "ababababa", REG_EXTENDED, 1, { { 0, 9 } } },
  { "^(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?).?\\9\\8\\7\\6\\5\\4\\3\\2\\1$",
    "level", REG_NOSUB | REG_EXTENDED, 0, { { -1, -1 } } },
  { "^(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?).?\\9\\8\\7\\6\\5\\4\\3\\2\\1$",
    "ababababa", REG_EXTENDED, 1, { { 0, 9 } } },
  /* Test for *+ match.  */
  { "^a*+(.)", "ab", REG_EXTENDED, 2, { { 0, 2 }, { 1, 2 } } },
  /* Test for ** match.  */
  { "^(a*)*(.)", "ab", REG_EXTENDED, 3, { { 0, 2 }, { 0, 1 }, { 1, 2 } } },
};

static void
bug_regex11 (void)
{
  regex_t re;
  regmatch_t rm[5];

  for (size_t i = 0; i < sizeof (tests) / sizeof (tests[0]); ++i)
    {
      int err = regcomp (&re, tests[i].pattern, tests[i].flags);
      if (err != 0)
        {
          char buf[500];
          regerror (err, &re, buf, sizeof (buf));
          report_error ("%s: regcomp %zd failed: %s", tests[i].pattern, i, buf);
          continue;
        }

      if (regexec (&re, tests[i].string, tests[i].nmatch, rm, 0))
        {
          report_error ("%s: regexec %zd failed", tests[i].pattern, i);
          regfree (&re);
          continue;
        }

      for (int n = 0; n < tests[i].nmatch; ++n)
        if (rm[n].rm_so != tests[i].rm[n].rm_so
              || rm[n].rm_eo != tests[i].rm[n].rm_eo)
          {
            if (tests[i].rm[n].rm_so == -1 && tests[i].rm[n].rm_eo == -1)
              break;
            report_error ("%s: regexec %zd match failure rm[%d] %d..%d",
                          tests[i].pattern, i, n,
                          (int) rm[n].rm_so, (int) rm[n].rm_eo);
            break;
          }

      regfree (&re);
    }
}

int
main (int argc, char *argv[])
{
  struct re_pattern_buffer regex;
  unsigned char folded_chars[UCHAR_MAX + 1];
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
