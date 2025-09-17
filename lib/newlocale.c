/* Create a locale object.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <locale.h>

#include <errno.h>

#if HAVE_NEWLOCALE
/* Only provide workarounds.  */

# include <sys/types.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

locale_t
newlocale (int category_mask, const char *name, locale_t base)
# undef newlocale
{
  if ((category_mask & ~LC_ALL_MASK) != 0)
    {
      errno = EINVAL;
      return NULL;
    }

# if defined __NetBSD__
  /* Work around a NetBSD bug: newlocale does not fail (unlike setlocale)
     when NAME is an invalid locale name.  */
  if (category_mask != 0)
    {
      /* Test whether NAME is valid.  */
      if (!(streq (name, "C") || streq (name, "POSIX")))
        {
          char *filename = (char *) malloc (18 + strlen (name) + 9 + 1);
          if (filename == NULL)
            {
              errno = ENOMEM;
              return NULL;
            }
          sprintf (filename, "/usr/share/locale/%s/LC_CTYPE", name);
          struct stat statbuf;
          if (stat (filename, &statbuf) < 0)
            {
              free (filename);
              errno = ENOENT;
              return NULL;
            }
          free (filename);
        }
    }
# endif

  if (category_mask != LC_ALL_MASK && base == NULL)
    base = newlocale (LC_ALL_MASK, "C", NULL);

  return newlocale (category_mask, name, base);
}

#else
/* Implement from scratch.  */

# include <stdlib.h>
# include <string.h>

# include "localename.h"

locale_t
newlocale (int category_mask, const char *name, locale_t base)
{
  if ((category_mask & ~LC_ALL_MASK) != 0)
    {
      errno = EINVAL;
      return NULL;
    }

  struct gl_locale_t tmp;
  struct gl_locale_t *result;
  if (base != NULL)
    {
      /* Work on tmp, so as not to destroy BASE until we're successful.  */
      result = &tmp;
    }
  else
    {
      result = (struct gl_locale_t *) malloc (sizeof (struct gl_locale_t));
      if (result == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
    }

  /* Canonicalize the name.  */
  if (streq (name, "POSIX"))
    name = "C";

# if !HAVE_WINDOWS_LOCALE_T
  /* In this case, the only NAMEs that we support are "C" and (equivalently)
     "POSIX".  */
  if (category_mask != 0 && !streq (name, "C"))
    {
      errno = ENOENT;
      return NULL;
    }
# endif

  int i;
  int err;
  for (i = 0; i < 6; i++)
    {
      int log2_lcmask = gl_index_to_log2_lcmask (i);

      if ((category_mask & (1 << log2_lcmask)) != 0)
        {
          const char *lcname;
          if (name[0] == '\0')
            {
              /* name == "" means to look at the environment variables.  */
              static struct { int cat; char cat_name[11 + 1]; } const categories[6] =
                {
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_COLLATE))]  =
                    { LC_COLLATE,  "LC_COLLATE" },
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_CTYPE))]    =
                    { LC_CTYPE,    "LC_CTYPE" },
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MESSAGES))] =
                    { LC_MESSAGES, "LC_MESSAGES" },
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MONETARY))] =
                    { LC_MONETARY, "LC_MONETARY" },
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_NUMERIC))]  =
                    { LC_NUMERIC,  "LC_NUMERIC" },
                  [gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_TIME))]     =
                    { LC_TIME,     "LC_TIME" }
                };
              lcname = gl_locale_name_environ (categories[i].cat,
                                               categories[i].cat_name);
              if (lcname == NULL)
                lcname = "C";
            }
          else
            lcname = name;

          result->category[i].name = strdup (lcname);
          if (result->category[i].name == NULL)
            {
              err = ENOMEM;
              goto fail_with_err;
            }
          if (streq (lcname, "C"))
            {
              result->category[i].is_c_locale = true;
# if HAVE_WINDOWS_LOCALE_T
              /* Just to initialize it.  */
              result->category[i].system_locale = NULL;
# endif
            }
          else
            {
              result->category[i].is_c_locale = false;
# if HAVE_WINDOWS_LOCALE_T
              if (log2_lcmask == gl_log2_lc_mask (LC_MESSAGES))
                result->category[i].system_locale = NULL;
              else
                {
                  int cat = log2_lcmask;
                  (void) cat;
                  /* Documentation:
                     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/create-locale-wcreate-locale>  */
                  result->category[i].system_locale =
                    _create_locale (LC_ALL /* or cat */, lcname);
                  if (result->category[i].system_locale == NULL)
                    {
                      free (result->category[i].name);
                      err = ENOENT;
                      goto fail_with_err;
                    }
                }
# endif
            }
        }
      else
        {
          if (base == NULL)
            {
              result->category[i].name = strdup ("C");
              if (result->category[i].name == NULL)
                {
                  err = ENOMEM;
                  goto fail_with_err;
                }
              result->category[i].is_c_locale = true;
# if HAVE_WINDOWS_LOCALE_T
              /* Just to initialize it.  */
              result->category[i].system_locale = NULL;
# endif
            }
        }
    }

  /* Success.  */
  if (base != NULL)
    {
      /* Copy the modified entries from RESULT to BASE.  */
      for (i = 0; i < 6; i++)
        {
          int log2_lcmask = gl_index_to_log2_lcmask (i);
          if ((category_mask & (1 << log2_lcmask)) != 0)
            {
# if HAVE_WINDOWS_LOCALE_T
              if (!(i == gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MESSAGES))
                    || base->category[i].is_c_locale))
                /* Documentation:
                   <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/free-locale>  */
                _free_locale (base->category[i].system_locale);
# endif
              free (base->category[i].name);

              base->category[i] = result->category[i];
            }
        }
      return base;
    }
  else
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
  if (base == NULL)
    free (result);
  errno = err;
  return NULL;
}

#endif
