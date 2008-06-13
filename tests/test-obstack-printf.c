/* Test of obstack_printf() and obstack_vprintf() functions.
   Copyright (C) 2008 Free Software Foundation, Inc.

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

/* Written by Eric Blake <ebb9@byu.net>, 2008.  */

#include <config.h>

#include <stdio.h>

#include "obstack.h"
#include "xalloc.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free

static void
test_function (int (*my_obstack_printf) (struct obstack *, const char *, ...))
{
  struct obstack obs;
  obstack_init (&obs);
  /* In general, don't invoke obstack_* functions inside ASSERT, as
     not all compilers can avoid multiple side effects.  */

  /* Grow the obstack to near its boundary, then check that output
     longer than the obstack free space grows the obstack.  */
  {
    char *base = obstack_base (&obs);
    char *new_base;
    int result;
    int size;
    int room = obstack_room (&obs) - 4;

    obstack_blank_fast (&obs, room);
    result = my_obstack_printf (&obs, "%d %s", 123, "456");
    ASSERT (result == 7);
    size = obstack_object_size (&obs);
    ASSERT (result + room == size);
    obstack_1grow (&obs, 0);
    new_base = obstack_finish (&obs);
    ASSERT (base != new_base);
    ASSERT (strcmp (new_base + room, "123 456") == 0);
  }

  /* Check that strings shorter than the obstack free space don't
     cause a reshuffling of the obstack.  */
  {
    char *base = obstack_base (&obs);
    char *new_base;
    int result;
    int size;
    int room = obstack_room (&obs);

    ASSERT (8 < room);
    result = my_obstack_printf (&obs, "%d %s", 123, "456");
    ASSERT (result == 7);
    size = obstack_object_size (&obs);
    ASSERT (result == size);
    new_base = obstack_base (&obs);
    ASSERT (base == new_base);
    ASSERT (strncmp (base, "123 456", result) == 0);
  }

  obstack_free (&obs, NULL);
}

static int
my_obstack_printf (struct obstack *obs, const char *format, ...)
{
  va_list args;
  int ret;

  va_start (args, format);
  ret = obstack_vprintf (obs, format, args);
  va_end (args);
  return ret;
}

static void
test_obstack_vprintf ()
{
  test_function (my_obstack_printf);
}

static void
test_obstack_printf ()
{
  test_function (obstack_printf);
}

int
main (int argc, char *argv[])
{
  test_obstack_vprintf ();
  test_obstack_printf ();
  return 0;
}
