/* Test of execvpe().
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include "signature.h"
SIGNATURE_CHECK (execvpe, int, (const char *, char * const *, char * const *));

#include <stdio.h>
#include <string.h>

/* Looks up the NAME=VALUE assignment among the environment variables.
   Returns it, or NULL if not found.  */
static const char *
get_environ_assignment (const char *name)
{
  size_t name_len = strlen (name);
  char **p;
  for (p = environ; *p != NULL; p++)
    {
      const char *assignment = *p;
      if (strncmp (assignment, name, name_len) == 0
          && assignment[name_len] == '=')
        return assignment;
    }
  return NULL;
}

/* Creates a minimal environment.  */
static void
create_minimal_env (const char *env[5])
{
  const char **p = env;
  *p++ =
    #ifdef __CYGWIN__
    /* The Cygwin DLLs needed by the program are in /bin.  */
    "PATH=.:/bin";
    #else
    "PATH=.";
    #endif
  *p = get_environ_assignment ("QEMU_LD_PREFIX");
  if (*p != NULL)
    p++;
  *p = get_environ_assignment ("QEMU_CPU");
  if (*p != NULL)
    p++;
  *p++ = "Hommingberg=Gepardenforelle";
  *p = NULL;
}

int
main ()
{
  const char *progname = "test-exec-child";
  const char *argv[12] =
    {
      progname,
      "abc def",
      "abc\"def\"ghi",
      "xyz\"",
      "abc\\def\\ghi",
      "xyz\\",
      "???",
      "***",
      "",
      "foo",
      "",
      NULL
    };
  const char *env[5];
  create_minimal_env (env);
  execvpe (progname, (char * const *) argv, (char * const *) env);

  perror ("execvpe");
  return 1;
}
