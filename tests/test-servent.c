/* Test the servent module.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Collin Funk <collin.funk1@gmail.com>, 2024.  */

#include <config.h>

/* Specification.  */
#include <netdb.h>

#include "signature.h"
SIGNATURE_CHECK (getservbyname, struct servent *,
                 (char const *, char const *));
SIGNATURE_CHECK (getservbyport, struct servent *, (int, char const *));

#include <stdio.h>
#include <arpa/inet.h>

int
main (void)
{
  struct servent *result;

  result = getservbyname ("domain", "tcp");
  if (result == NULL)
    fputs ("getservbyname failed\n", stderr);

  result = getservbyport (htons (53), "tcp");
  if (result == NULL)
    fputs ("getservbyport failed\n", stderr);

  return 0;
}