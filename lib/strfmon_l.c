/* strfmon_l override.
   Copyright (C) 2017-2018 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <monetary.h>

#include <locale.h>
#include <stdarg.h>

#undef strfmon_l

/* This override can only support a limited number of arguments.  */
#define MAX_ARG_WORDS 16

ssize_t
rpl_strfmon_l (char *s, size_t maxsize, locale_t locale, const char *format, ...)
{
  /* Work around glibc 2.23 bug
     <https://sourceware.org/bugzilla/show_bug.cgi?id=19633>.  */
  va_list argptr;
  double args[MAX_ARG_WORDS];
  int i;
  locale_t orig_locale;
  ssize_t result;

  orig_locale = uselocale ((locale_t)0);

  if (uselocale (locale) == (locale_t)0)
    /* errno is set.  */
    return -1;

  va_start (argptr, format);
  /* Hack: Consume more arguments than those that are actually given.  */
  for (i = 0; i < MAX_ARG_WORDS; i++)
    args[i] = va_arg (argptr, double);

  result = strfmon_l (s, maxsize, locale, format,
                      args[0], args[1], args[2], args[3], args[4], args[5],
                      args[6], args[7], args[8], args[9], args[10], args[11],
                      args[12], args[13], args[14], args[15]);

  va_end (argptr);

  if (uselocale (orig_locale) == (locale_t)0)
    /* errno is set.  */
    return -1;

  return result;
}
