/* Test alignalloc and alignfree.

   Copyright 2022 Free Software Foundation, Inc.

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

#include <alignalloc.h>

#include <stdint.h>
#include <string.h>
#include "intprops.h"

#include "signature.h"
SIGNATURE_CHECK (alignalloc, void *, (idx_t, idx_t));
SIGNATURE_CHECK (alignfree, void, (void *));

#include "macros.h"

int
main ()
{
  /* Check that alignalloc returns properly aligned storage,
     when it succeeds.  */
  for (idx_t alignment = 1; ; )
    {
      for (idx_t size = 0; size <= 1024; size = size ? 2 * size : 1)
        {
          void *p = alignalloc (alignment, size);
          if (p)
            {
              memset (p, 0, size);
              ASSERT ((uintptr_t) p % alignment == 0);
            }
          alignfree (p);
        }
      if (INT_MULTIPLY_WRAPV (alignment, 2, &alignment))
        break;
    }

  /* Check that alignfree is a no-op on null pointers.  */
  alignfree (NULL);

  return 0;
}
