/* Test the servent module.
   Copyright (C) 2024-2026 Free Software Foundation, Inc.

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

/* Written by Collin Funk and Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <netdb.h>

#include "signature.h"
SIGNATURE_CHECK (getservbyname, struct servent *, (const char *, const char *));
SIGNATURE_CHECK (getservbyport, struct servent *, (int, const char *));

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "macros.h"

static bool
is_aliases_member (const struct servent *se, const char *name)
{
  for (int i = 0; se->s_aliases[i] != NULL; i++)
    if (streq (se->s_aliases[i], name))
      return true;
  return false;
}

int
main (void)
{
  {
    struct servent *se = getservbyname ("domain", "tcp");
    if (se == NULL)
      fputs ("getservbyname failed\n", stderr);
  }
  {
    struct servent *se = getservbyport (htons (53), "tcp");
    if (se == NULL)
      fputs ("getservbyport failed\n", stderr);
  }
  {
    struct servent *se = getservbyport (htons (80), "tcp");
    ASSERT (se != NULL);
    ASSERT (se->s_port == htons (80));
    ASSERT (streq (se->s_proto, "tcp"));
    ASSERT ((streq (se->s_name, "http") && is_aliases_member (se, "www"))
            || (streq (se->s_name, "www") && is_aliases_member (se, "http")));
  }
  {
    struct servent *se = getservbyname ("http", "tcp");
    ASSERT (se != NULL);
    ASSERT (se->s_port == htons (80));
    ASSERT (streq (se->s_proto, "tcp"));
    ASSERT ((streq (se->s_name, "http") && is_aliases_member (se, "www"))
            || (streq (se->s_name, "www") && is_aliases_member (se, "http")));
  }
  {
    struct servent *se = getservbyname ("www", "tcp");
    ASSERT (se != NULL);
    ASSERT (se->s_port == htons (80));
    ASSERT (streq (se->s_proto, "tcp"));
    ASSERT ((streq (se->s_name, "http") && is_aliases_member (se, "www"))
            || (streq (se->s_name, "www") && is_aliases_member (se, "http")));
  }

  return test_exit_status;
}
