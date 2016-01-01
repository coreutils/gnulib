/* Duplicate a locale object.
   Copyright (C) 2009-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

/* Specification.  */
#include <locale.h>

#include <errno.h>
#include <string.h>

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

#undef duplocale

locale_t
rpl_duplocale (locale_t locale)
{
  /* Work around crash in the duplocale function in glibc < 2.12.
     See <http://sourceware.org/bugzilla/show_bug.cgi?id=10969>.
     Also, on AIX 7.1, duplocale(LC_GLOBAL_LOCALE) returns (locale_t)0 with
     errno set to EINVAL.  */
  if (locale == LC_GLOBAL_LOCALE)
    {
      /* Create a copy of the locale by fetching the name of each locale
         category, starting with LC_CTYPE.  */
      static struct { int cat; int mask; } const categories[] =
        {
            { LC_NUMERIC,        LC_NUMERIC_MASK },
            { LC_TIME,           LC_TIME_MASK },
            { LC_COLLATE,        LC_COLLATE_MASK },
            { LC_MONETARY,       LC_MONETARY_MASK },
            { LC_MESSAGES,       LC_MESSAGES_MASK }
#ifdef LC_PAPER
          , { LC_PAPER,          LC_PAPER_MASK }
#endif
#ifdef LC_NAME
          , { LC_NAME,           LC_NAME_MASK }
#endif
#ifdef LC_ADDRESS
          , { LC_ADDRESS,        LC_ADDRESS_MASK }
#endif
#ifdef LC_TELEPHONE
          , { LC_TELEPHONE,      LC_TELEPHONE_MASK }
#endif
#ifdef LC_MEASUREMENT
          , { LC_MEASUREMENT,    LC_MEASUREMENT_MASK }
#endif
#ifdef LC_IDENTIFICATION
          , { LC_IDENTIFICATION, LC_IDENTIFICATION_MASK }
#endif
        };
      const char *base_name;
      locale_t base_copy;
      unsigned int i;

      base_name = setlocale (LC_CTYPE, NULL);
      base_copy = newlocale (LC_ALL_MASK, base_name, NULL);
      if (base_copy == NULL)
        return NULL;

      for (i = 0; i < SIZEOF (categories); i++)
        {
          int category = categories[i].cat;
          int category_mask = categories[i].mask;
          const char *name = setlocale (category, NULL);
          if (strcmp (name, base_name) != 0)
            {
              locale_t copy = newlocale (category_mask, name, base_copy);
              if (copy == NULL)
                {
                  int saved_errno = errno;
                  freelocale (base_copy);
                  errno = saved_errno;
                  return NULL;
                }
              /* No need to call freelocale (base_copy) if copy != base_copy;
                 the newlocale function already takes care of doing it.  */
              base_copy = copy;
            }
        }

      return base_copy;
    }

  return duplocale (locale);
}
