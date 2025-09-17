/* Duplicate a locale object.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

/* Specification.  */
#include <locale.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

locale_t
duplocale (locale_t locale)
#undef duplocale
{
  /* Implement duplocale(LC_GLOBAL_LOCALE) on platforms without locale_t.
     Also, work around crash in the duplocale function in glibc < 2.12.
     See <https://sourceware.org/PR10969>.
     Also, on AIX 7.1, duplocale(LC_GLOBAL_LOCALE) returns (locale_t)0 with
     errno set to EINVAL.
     Also, on NetBSD 7.0, duplocale(LC_GLOBAL_LOCALE) returns a locale that
     corresponds to the C locale.  */
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
      char base_name[SETLOCALE_NULL_MAX];
      int err;
      locale_t base_copy;
      unsigned int i;

      err = setlocale_null_r (LC_CTYPE, base_name, sizeof (base_name));
      if (err)
        {
          errno = err;
          return NULL;
        }
      base_copy = newlocale (LC_ALL_MASK, base_name, NULL);
      if (base_copy == NULL)
        return NULL;

      for (i = 0; i < SIZEOF (categories); i++)
        {
          int category = categories[i].cat;
          int category_mask = categories[i].mask;
          char name[SETLOCALE_NULL_MAX];

          err = setlocale_null_r (category, name, sizeof (name));
          if (err)
            {
              errno = err;
              return NULL;
            }
          if (!streq (name, base_name))
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

#if GNULIB_defined_locale_t

  locale_t result = (struct gl_locale_t *) malloc (sizeof (struct gl_locale_t));
  if (result == NULL)
    {
      errno = ENOMEM;
      return NULL;
    }

  int i;
  int err;
  for (i = 0; i < 6; i++)
    {
      int log2_lcmask = gl_index_to_log2_lcmask (i);

      result->category[i].name = strdup (locale->category[i].name);
      if (result->category[i].name == NULL)
        {
          err = ENOMEM;
          goto fail_with_err;
        }
      result->category[i].is_c_locale = locale->category[i].is_c_locale;
# if HAVE_WINDOWS_LOCALE_T
      if (log2_lcmask == gl_log2_lc_mask (LC_MESSAGES)
          || result->category[i].is_c_locale)
        {
          /* Just to initialize it.  */
          result->category[i].system_locale = NULL;
        }
      else
        {
          int cat = log2_lcmask;
          (void) cat;
          /* Documentation:
             <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/create-locale-wcreate-locale>  */
          result->category[i].system_locale =
            _create_locale (LC_ALL /* or cat */, result->category[i].name);
          if (result->category[i].system_locale == NULL)
            {
              free (result->category[i].name);
              err = ENOENT;
              goto fail_with_err;
            }
        }
# endif
    }

  /* Success.  */
  return result;

 fail_with_err:
  while (--i >= 0)
    {
# if HAVE_WINDOWS_LOCALE_T
      if (!(i == gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MESSAGES))
            || result->category[i].is_c_locale))
        /* Documentation:
           <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/free-locale>  */
        _free_locale (result->category[i].system_locale);
# endif
      free (result->category[i].name);
    }
  free (result);
  errno = err;
  return NULL;

#else

  return duplocale (locale);

#endif
}
