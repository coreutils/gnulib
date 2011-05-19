/* Test of strerror_r() function.
   Copyright (C) 2007-2011 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

#include <string.h>

#include "signature.h"
SIGNATURE_CHECK (strerror_r, int, (int, char *, size_t));

#include <errno.h>

#include "macros.h"

int
main (void)
{
  char buf[100];
  int ret;

  /* Test results with valid errnum and enough room.  */

  errno = 0;
  buf[0] = '\0';
  ASSERT (strerror_r (EACCES, buf, sizeof (buf)) == 0);
  ASSERT (buf[0] != '\0');
  ASSERT (errno == 0);

  errno = 0;
  buf[0] = '\0';
  ASSERT (strerror_r (ETIMEDOUT, buf, sizeof (buf)) == 0);
  ASSERT (buf[0] != '\0');
  ASSERT (errno == 0);

  errno = 0;
  buf[0] = '\0';
  ASSERT (strerror_r (EOVERFLOW, buf, sizeof (buf)) == 0);
  ASSERT (buf[0] != '\0');
  ASSERT (errno == 0);

  /* POSIX requires strerror (0) to succeed.  Reject use of "Unknown
     error", but allow "Success", "No error", or even Solaris' "Error
     0" which are distinct patterns from true out-of-range strings.
     http://austingroupbugs.net/view.php?id=382  */
  errno = 0;
  buf[0] = '\0';
  ret = strerror_r (0, buf, sizeof (buf));
  ASSERT (ret == 0);
  ASSERT (buf[0]);
  ASSERT (errno == 0);
  ASSERT (strstr (buf, "nknown") == NULL);

  /* Test results with out-of-range errnum and enough room.  */

  errno = 0;
  buf[0] = '^';
  ret = strerror_r (-3, buf, sizeof (buf));
  ASSERT (ret == 0 || ret == EINVAL);
  if (ret == 0)
    ASSERT (buf[0] != '^');
  ASSERT (errno == 0);

  /* Test results with a too small buffer.  */

  ASSERT (strerror_r (EACCES, buf, sizeof (buf)) == 0);
  {
    size_t len = strlen (buf);
    size_t i;

    for (i = 0; i <= len; i++)
      {
        strcpy (buf, "BADFACE");
        errno = 0;
        ret = strerror_r (EACCES, buf, i);
        ASSERT (errno == 0);
        if (ret == 0)
          {
            /* Truncated result.  POSIX allows this, and it actually
               happens on AIX 6.1 and Cygwin.  */
            ASSERT ((strcmp (buf, "BADFACE") == 0) == (i == 0));
          }
        else
          {
            /* Failure.  */
            ASSERT (ret == ERANGE);
            /* buf is clobbered nevertheless, on FreeBSD and MacOS X.  */
          }
      }

    strcpy (buf, "BADFACE");
    errno = 0;
    ret = strerror_r (EACCES, buf, len + 1);
    ASSERT (ret == 0);
    ASSERT (errno == 0);
  }

  return 0;
}
