/* Test of inet_ntop function.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

#include <arpa/inet.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);                                                   \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

int
main (void)
{
#if defined AF_INET /* HAVE_IPV4 */
  {
    struct in_addr internal;
    char printable[16];
    const char *result;

    /* This machine was for a long time known as
       ma2s2.mathematik.uni-karlsruhe.de.  */
# ifdef WORDS_BIGENDIAN
    internal.s_addr = 0x810D7302;
# else
    internal.s_addr = 0x02730D81;
# endif
    result = inet_ntop (AF_INET, &internal, printable, sizeof (printable));
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "129.13.115.2") == 0);
  }
#endif

  return 0;
}
