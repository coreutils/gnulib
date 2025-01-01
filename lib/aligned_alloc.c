/* An aligned_alloc() function that works around platform bugs.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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
#include <stdlib.h>

#include <errno.h>

void *
aligned_alloc (size_t alignment, size_t size)
#undef aligned_alloc
{
  if (alignment != 0 && (alignment & (alignment - 1)) == 0)
    {
      /* alignment is a power of 2.  */
      if (size == 0)
        size = 1;
      if (alignment >= sizeof (void *))
        {
          /* Make size a multiple of alignment.  */
          size = (size + (alignment - 1)) & -alignment;
          if (size > 0)
            return aligned_alloc (alignment, size);
          else
            {
              errno = ENOMEM;
              return NULL;
            }
        }
      else
        return malloc (size);
    }
  else
    {
      errno = EINVAL;
      return NULL;
    }
}
