/* Test of locking in multithreaded situations.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include <threads.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

/* Returns the effective type of a lock.  */
static const char *
get_effective_type (mtx_t *lock)
{
  /* Lock once.  */
  ASSERT (mtx_lock (lock) == thrd_success);

  /* Try to lock a second time.  */
  int err = mtx_trylock (lock);
  if (err == thrd_success)
    return "RECURSIVE";
  if (err == thrd_busy)
    return "NORMAL";

  return "unknown!";
}

int
main ()
{
  /* Find the effective type of a PLAIN lock.  */
  const char *type_plain;
  {
    mtx_t lock;
    ASSERT (mtx_init (&lock, mtx_plain) == thrd_success);
    type_plain = get_effective_type (&lock);
  }

  /* Find the effective type of a TIMED lock.  */
  const char *type_timed;
  {
    mtx_t lock;
    ASSERT (mtx_init (&lock, mtx_timed) == thrd_success);
    type_timed = get_effective_type (&lock);
  }

  /* Find the effective type of a PLAIN RECURSIVE lock.  */
  const char *type_plain_rec;
  {
    mtx_t lock;
    ASSERT (mtx_init (&lock, mtx_plain | mtx_recursive) == thrd_success);
    type_plain_rec = get_effective_type (&lock);
  }

  /* Find the effective type of a TIMED RECURSIVE lock.  */
  const char *type_timed_rec;
  {
    mtx_t lock;
    ASSERT (mtx_init (&lock, mtx_timed | mtx_recursive) == thrd_success);
    type_timed_rec = get_effective_type (&lock);
  }

  printf ("PLAIN           -> type = %s\n", type_plain);
  printf ("TIMED           -> type = %s\n", type_timed);
  printf ("PLAIN RECURSIVE -> type = %s\n", type_plain_rec);
  printf ("TIMED RECURSIVE -> type = %s\n", type_timed_rec);

  ASSERT (strcmp (type_plain,     "NORMAL") == 0);
  ASSERT (strcmp (type_timed,     "NORMAL") == 0);
  ASSERT (strcmp (type_plain_rec, "RECURSIVE") == 0);
  ASSERT (strcmp (type_timed_rec, "RECURSIVE") == 0);

  return test_exit_status;
}
