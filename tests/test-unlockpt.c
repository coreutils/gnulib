/* Test unlocking of the slave side of a pseudo-terminal.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

#include <stdlib.h>

#include "signature.h"
SIGNATURE_CHECK (unlockpt, int, (int));

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "macros.h"

int
main (void)
{
  /* Test behaviour for invalid file descriptors.  */
#if !(defined __NetBSD__ || defined __HAIKU__) /* known bug on NetBSD 5.1, Haiku */
  {
    errno = 0;
    ASSERT (unlockpt (-1) == -1);
    ASSERT (errno == EBADF
            || errno == EINVAL /* seen on FreeBSD 6.4 */
           );
  }
  {
    close (99);
    errno = 0;
    ASSERT (unlockpt (99) == -1);
    ASSERT (errno == EBADF
            || errno == EINVAL /* seen on FreeBSD 6.4 */
           );
  }
# ifdef AT_FDCWD
  {
    errno = 0;
    ASSERT (unlockpt (AT_FDCWD) == -1);
    ASSERT (errno == EBADF
            || errno == EINVAL /* seen on FreeBSD 6.4 */
           );
  }
# endif
#endif

  return test_exit_status;
}
