/* Return name of a single locale category.
   Copyright (C) 1995-2025 Free Software Foundation, Inc.

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
#include <locale.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "getlocalename_l-unsafe.h"

#include "flexmember.h"
#include "glthread/lock.h"
#include "thread-optim.h"

/* Define a local struniq() function.  */
#include "struniq.h"

const char *
getlocalename_l (int category, locale_t locale)
{
  if (category == LC_ALL)
    /* Unsupported in this simple implementation.  */
    abort ();

  struct string_with_storage ret = getlocalename_l_unsafe (category, locale);
  /* Return the result as a string of indefinite extent.
     In the case (ret.storage == STORAGE_GLOBAL) this is required because
     POSIX says that
       "the returned string pointer might be invalidated or the string content
        might be overwritten by a subsequent call in the same thread to
        getlocalename_l() with LC_GLOBAL_LOCALE"
     and a setlocale_null call in a different thread would also invalidate
     the result.  For the other cases, it's a convenience to avoid undefined
     behaviour in the calling program.  */
  return (ret.storage != STORAGE_INDEFINITE
          ? struniq (ret.value)
          : ret.value);
}
