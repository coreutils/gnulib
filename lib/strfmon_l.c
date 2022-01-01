/* strfmon_l override.
   Copyright (C) 2017-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <monetary.h>

#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#undef strfmon_l

/* This override can only support a limited number of arguments.  */
#define MAX_ARGS 16

/* A parsed directive.  */
typedef struct
{
  bool needs_long_double;
  const char *conversion_ptr;
}
directive_t;

/* A parsed format string.  */
typedef struct
{
  size_t count;
  directive_t dir[MAX_ARGS];
}
directives_t;

/* Parses a monetary format string.
   Returns 0 and fills *DIRECTIVESP if valid.
   Returns -1 if invalid.  */
static int
fmon_parse (const char *format, directives_t *directivesp)
{
  size_t count = 0;
  const char *cp = format;

  while (*cp != '\0')
    {
      if (*cp++ == '%')
        {
          /* Parse flags.  */
          while (*cp == '=' || *cp == '^' || *cp == '+' || *cp == '('
                 || *cp == '!' || *cp == '-')
            {
              if (*cp == '=')
                {
                  cp++;
                  if (*cp == '\0')
                    return -1;
                }
              cp++;
            }
          /* Parse field width.  */
          while (*cp >= '0' && *cp <= '9')
            cp++;
          /* Parse left precision.  */
          if (*cp == '#')
            {
              cp++;
              while (*cp >= '0' && *cp <= '9')
                cp++;
            }
          /* Parse right precision.  */
          if (*cp == '.')
            {
              cp++;
              while (*cp >= '0' && *cp <= '9')
                cp++;
            }
          /* Now comes the conversion specifier.  */
          if (*cp != '%')
            {
              if (count == MAX_ARGS)
                /* Too many arguments.  */
                return -1;

              /* glibc supports an 'L' modifier before the conversion specifier.  */
              if (*cp == 'L')
                {
                  cp++;
                  directivesp->dir[count].needs_long_double = true;
                }
              else
                directivesp->dir[count].needs_long_double = false;
              if (!(*cp == 'i' || *cp == 'n'))
                return -1;
              directivesp->dir[count].conversion_ptr = cp;
              count++;
            }
          cp++;
        }
    }

  directivesp->count = count;
  return 0;
}

ssize_t
rpl_strfmon_l (char *s, size_t maxsize, locale_t locale, const char *format, ...)
{
  /* Work around glibc 2.23 bug
     <https://sourceware.org/bugzilla/show_bug.cgi?id=19633>.  */
  va_list argptr;
  locale_t orig_locale;
  directives_t directives;
  ssize_t result;

  orig_locale = uselocale ((locale_t)0);

  if (uselocale (locale) == (locale_t)0)
    /* errno is set.  */
    return -1;

  /* The format string may consume 'double' or 'long double' arguments.
     In order not to have to link with libffcall or libffi, convert all
     arguments to 'long double', and use a modified format string that
     requests 'long double' arguments.  But since 'long double' arguments
     are only supported on glibc, do so only if the original format string
     consumes at least one 'long double' argument.  */
  if (fmon_parse (format, &directives) < 0)
    {
      errno = EINVAL;
      result = -1;
    }
  else
    {
      bool use_long_double;
      unsigned int i;

      use_long_double = false;
      for (i = 0; i < directives.count; i++)
        if (directives.dir[i].needs_long_double)
          {
            use_long_double = true;
            break;
          }

      va_start (argptr, format);

      if (use_long_double)
        {
          char *ld_format;

          /* Allocate room for the modified format string.  */
          ld_format = (char *) malloc (strlen (format) + directives.count + 1);
          if (ld_format == NULL)
            {
              errno = ENOMEM;
              result = -1;
            }
          else
            {
              long double args[MAX_ARGS];

              /* Create the modified format string.  */
              {
                const char *p = format;
                char *dest = ld_format;
                for (i = 0; i < directives.count; i++)
                  {
                    const char *q = directives.dir[i].conversion_ptr;
                    memcpy (dest, p, q - p);
                    dest += q - p;
                    if (!directives.dir[i].needs_long_double)
                      *dest++ = 'L';
                    p = q;
                  }
                strcpy (dest, p);
              }

              /* Set up arguments array.  */
              for (i = 0; i < directives.count; i++)
                args[i] = (directives.dir[i].needs_long_double
                           ? va_arg (argptr, long double)
                           : (long double) va_arg (argptr, double));
              /* Avoid uninitialized memory references.  */
              for (; i < MAX_ARGS; i++)
                args[i] = 0.0L;

              result = strfmon_l (s, maxsize, locale, ld_format,
                                  args[0], args[1], args[2], args[3], args[4],
                                  args[5], args[6], args[7], args[8], args[9],
                                  args[10], args[11], args[12], args[13],
                                  args[14], args[15]);

              free (ld_format);
            }
        }
      else
        {
          double args[MAX_ARGS];

          /* Set up arguments array.  */
          for (i = 0; i < directives.count; i++)
            args[i] = va_arg (argptr, double);
          /* Avoid uninitialized memory references.  */
          for (; i < MAX_ARGS; i++)
            args[i] = 0.0;

          result = strfmon_l (s, maxsize, locale, format,
                              args[0], args[1], args[2], args[3], args[4],
                              args[5], args[6], args[7], args[8], args[9],
                              args[10], args[11], args[12], args[13], args[14],
                              args[15]);
        }

      va_end (argptr);
    }

  if (uselocale (orig_locale) == (locale_t)0)
    /* errno is set.  */
    return -1;

  return result;
}
