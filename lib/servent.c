/* Functions getservbyname(), getservbyport().
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <netdb.h>

#if defined _WIN32 && !defined __CYGWIN__

# include "sockets.h"
# include <winsock2.h>

struct servent *
getservbyname (const char *name, const char *protocol)
# undef getservbyname
{
  (void) gl_sockets_startup (SOCKETS_1_1);
  return getservbyname (name, protocol);
}

struct servent *
getservbyport (int n_port, const char *protocol)
# undef getservbyport
{
  (void) gl_sockets_startup (SOCKETS_1_1);
  return getservbyport (n_port, protocol);
}

#else

# include <arpa/inet.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/* Cache of the contents of /etc/services.  */
static struct servent *db /* = NULL */;
static size_t db_length /* = 0 */;
static size_t db_allocated /* = 0 */;
static bool db_initialized /* = false */;

static void
initialize (void)
{
  FILE *fp = fopen ("/etc/services", "re");
  if (fp != NULL)
    {
      while (!(ferror (fp) || feof (fp)))
        {
          /* It's OK to use fixed-length buffers here, because /etc/services
             is under control of the system administrator, not of an arbitrary
             user.  */
          char name[32+1];
          unsigned int num;
          char proto[32+1];
          char aliases[8][32+1];
          char rest[256+1];
          int n_aliases;
          int ret;

          /* Parse a line.  */
          ret = fscanf (fp, "%32s"
                            "%*[ \t]%u/%32s"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%32[^ \t#\n]"
                            "%*[ \t]%256[^#\n]",
                        name, &num, proto,
                        aliases[0], aliases[1], aliases[2], aliases[3],
                        aliases[4], aliases[5], aliases[6], aliases[7],
                        rest);
          if (ret >= 3)
            {
              for (n_aliases = 0; n_aliases < 8 && n_aliases < ret - 3; n_aliases++)
                if (aliases[n_aliases][0] == '\0')
                  break;

              /* Successfully parsed an entry.  Now allocate memory.  */
              if (db_length == db_allocated)
                {
                  size_t new_allocated = 2 * db_allocated;
                  if (new_allocated < 10)
                    new_allocated = 10;
                  db = (struct servent *) realloc (db, new_allocated * sizeof (struct servent));
                  if (db == NULL)
                    {
                      db_length = 0;
                      db_allocated = 0;
                      break;
                    }
                  db_allocated = new_allocated;
                }
              size_t need =
                (n_aliases > 0 ? n_aliases + 1 : 0) * sizeof (char *)
                + (strlen (name) + 1) + (strlen (proto) + 1);
              for (int i = 0; i < n_aliases; i++)
                need += strlen (aliases[i]) + 1;
              char *memory = (char *) malloc (need);
              if (memory == NULL)
                {
                  db = NULL;
                  db_length = 0;
                  db_allocated = 0;
                  break;
                }

              /* Fill the new 'struct servent'.  */
              struct servent *se = &db[db_length++];

              if (n_aliases > 0)
                {
                  se->s_aliases = (char **) memory;
                  memory += (n_aliases + 1) * sizeof (char *);
                }
              else
                {
                  static char *no_aliases /* = NULL */;
                  se->s_aliases = &no_aliases;
                }

              memcpy (memory, name, strlen (name) + 1);
              se->s_name = memory;
              memory += strlen (name) + 1;

              memcpy (memory, proto, strlen (proto) + 1);
              se->s_proto = memory;
              memory += strlen (proto) + 1;

              for (int i = 0; i < n_aliases; i++)
                {
                  memcpy (memory, aliases[i], strlen (aliases[i]) + 1);
                  se->s_aliases[i] = memory;
                  memory += strlen (aliases[i]) + 1;
                }
              se->s_aliases[n_aliases] = NULL;

              se->s_port = htons (num);
            }

          for (;;)
            {
              int c = fgetc (fp);
              if (c == EOF || c == '\n')
                break;
            }
        }

      fclose (fp);
    }

  db_initialized = true;
}

static bool
is_aliases_member (const struct servent *se, const char *name)
{
  for (int i = 0; se->s_aliases[i] != NULL; i++)
    if (streq (se->s_aliases[i], name))
      return true;
  return false;
}

struct servent *
getservbyname (const char *name, const char *protocol)
{
  if (!db_initialized)
    initialize ();

  size_t n = db_length;
  for (size_t i = 0; i < n; i++)
    {
      struct servent *se = &db[i];
      if ((streq (se->s_name, name) || is_aliases_member (se, name))
          && (protocol == NULL || streq (se->s_proto, protocol)))
        return se;
    }
  return NULL;
}

struct servent *
getservbyport (int n_port, const char *protocol)
{
  if (!db_initialized)
    initialize ();

  size_t n = db_length;
  for (size_t i = 0; i < n; i++)
    {
      struct servent *se = &db[i];
      if (se->s_port == n_port
          && (protocol == NULL || streq (se->s_proto, protocol)))
        return se;
    }
  return NULL;
}

#endif
