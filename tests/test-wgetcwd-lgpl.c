/* Test of wgetcwd() function.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

#include <wchar.h>

#include "signature.h"
#if defined _WIN32 && !defined __CYGWIN__
SIGNATURE_CHECK (wgetcwd, wchar_t *, (wchar_t *, size_t));
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

int
main ()
{
#if defined _WIN32 && !defined __CYGWIN__
  wchar_t *pwd1;
  wchar_t *pwd2;

  pwd1 = wgetcwd (NULL, 0);
  ASSERT (pwd1 && *pwd1);

  /* Make sure the result is usable.  */
  ASSERT (_wchdir (pwd1) == 0);
  ASSERT (_wchdir (L".//./.") == 0);

  /* Make sure that result is normalized.  */
  pwd2 = wgetcwd (NULL, 0);
  ASSERT (pwd2);
  ASSERT (wcscmp (pwd1, pwd2) == 0);
  free (pwd2);
  {
    size_t len = wcslen (pwd1);
    ssize_t i = len - 10;
    if (i < 1)
      i = 1;
    pwd2 = wgetcwd (NULL, len + 1);
    ASSERT (pwd2);
    free (pwd2);
    pwd2 = malloc ((len + 3) * sizeof (wchar_t));
    for ( ; i <= len; i++)
      {
        wchar_t *tmp;
        errno = 0;
        ASSERT (wgetcwd (pwd2, i) == NULL);
        ASSERT (errno == ERANGE);
        /* Allow either glibc or BSD behavior, since POSIX allows both.  */
        errno = 0;
        tmp = wgetcwd (NULL, i);
        if (tmp)
          {
            ASSERT (wcscmp (pwd1, tmp) == 0);
            free (tmp);
          }
        else
          {
            ASSERT (errno == ERANGE);
          }
      }
    /* The SIZE argument here needs to be len + 3, not len + 1.  */
    ASSERT (wgetcwd (pwd2, len + 3) == pwd2);
    pwd2[len] = L'/';
    pwd2[len + 1] = L'\0';
  }
  ASSERT (wcsstr (pwd2, L"/./") == NULL);
  ASSERT (wcsstr (pwd2, L"/../") == NULL);
  ASSERT (wcsstr (pwd2 + 1 + (pwd2[1] == L'/'), L"//") == NULL);

  /* Validate a POSIX requirement on size.  */
  errno = 0;
  ASSERT (wgetcwd (pwd2, 0) == NULL);
  ASSERT (errno == EINVAL);

  free (pwd1);
  free (pwd2);
#endif

  return test_exit_status;
}
