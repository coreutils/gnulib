/* Test of explicit_bzero() function.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include "signature.h"
SIGNATURE_CHECK (explicit_bzero, void, (void *, size_t));

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "vma-iter.h"
#include "macros.h"

#define SECRET "xyzzy1729"
#define SECRET_SIZE 9

static char zero[SECRET_SIZE] = { 0 };

/* Enable this to verify that the test is effective.  */
#if 0
# define explicit_bzero(a, n)  memset (a, '\0', n)
#endif

/* Suppress GCC 13.2.1 false alarm, as this test needs a dangling pointer.  */
#if _GL_GNUC_PREREQ (12, 0)
# pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif

/* =================== Verify operation on static memory =================== */

static char stbuf[SECRET_SIZE];

static void
test_static (void)
{
  memcpy (stbuf, SECRET, SECRET_SIZE);
  explicit_bzero (stbuf, SECRET_SIZE);
  ASSERT (memcmp (zero, stbuf, SECRET_SIZE) == 0);
}

/* =============== Verify operation on heap-allocated memory =============== */

/* Skip this part when an address sanitizer is in use, because it would report
   a "heap use after free".  */
#ifndef __has_feature
# define __has_feature(a) 0
#endif
#if defined __SANITIZE_ADDRESS__ || __has_feature (address_sanitizer)

static void
test_heap (void)
{
}

#else

/* Test whether an address range is mapped in memory.  */
# if VMA_ITERATE_SUPPORTED

struct locals
{
  uintptr_t range_start;
  uintptr_t range_end;
};

static int
vma_iterate_callback (void *data, uintptr_t start, uintptr_t end,
                      unsigned int flags)
{
  struct locals *lp = (struct locals *) data;

  /* Remove from [range_start, range_end) the part at the beginning or at the
     end that is covered by [start, end).  */
  if (start <= lp->range_start && end > lp->range_start)
    lp->range_start = (end < lp->range_end ? end : lp->range_end);
  if (start < lp->range_end && end >= lp->range_end)
    lp->range_end = (start > lp->range_start ? start : lp->range_start);

  return 0;
}

static bool
is_range_mapped (uintptr_t range_start, uintptr_t range_end)
{
  struct locals l;

  l.range_start = range_start;
  l.range_end = range_end;
  vma_iterate (vma_iterate_callback, &l);
  return l.range_start == l.range_end;
}

# else

static bool
is_range_mapped (uintptr_t range_start, uintptr_t range_end)
{
  return true;
}

# endif

static void
test_heap (void)
{
  char *heapbuf = (char *) malloc (SECRET_SIZE);
  ASSERT (heapbuf);
  uintptr_t volatile addr = (uintptr_t) heapbuf;
  memcpy (heapbuf, SECRET, SECRET_SIZE);
  explicit_bzero (heapbuf, SECRET_SIZE);
  free (heapbuf);
  heapbuf = (char *) addr;
  if (is_range_mapped (addr, addr + SECRET_SIZE))
    {
      /* some implementation could override freed memory by canaries so
         compare against secret */
      ASSERT (memcmp (heapbuf, SECRET, SECRET_SIZE) != 0);
      printf ("test_heap: address range is still mapped after free().\n");
    }
  else
    printf ("test_heap: address range is unmapped after free().\n");
}

#endif /* ! address sanitizer enabled */

/* =============== Verify operation on stack-allocated memory =============== */

/* Skip this part when an address sanitizer is in use, because it would report
   a "stack use after return".  */
#ifndef __has_feature
# define __has_feature(a) 0
#endif
#if defined __SANITIZE_ADDRESS__ || __has_feature (address_sanitizer)

static void
test_stack (void)
{
}

#else

/* There are two passes:
     1. Put a secret in memory and invoke explicit_bzero on it.
     2. Verify that the memory has been erased.
   Implement them in the same function, so that they access the same memory
   range on the stack.  Declare the local scalars to be volatile so they
   are not optimized away.  That way, the test verifies that the compiler
   does not eliminate a call to explicit_bzero, even if data flow analysis
   reveals that the stack area is dead at the end of the function.  */
static bool _GL_ATTRIBUTE_NOINLINE
# if _GL_GNUC_PREREQ (4, 5)
__attribute__ ((__noclone__))
# endif
# if _GL_GNUC_PREREQ (8, 0)
__attribute__ ((__noipa__))
# endif
do_secret_stuff (int volatile pass, char *volatile *volatile last_stackbuf)
{
  char stackbuf[SECRET_SIZE];
  if (pass == 1)
    {
      memcpy (stackbuf, SECRET, SECRET_SIZE);
      explicit_bzero (stackbuf, SECRET_SIZE);
      *last_stackbuf = stackbuf;
      return false;
    }
  else /* pass == 2 */
    {
      /* Use *last_stackbuf here, because stackbuf may be allocated at a
         different address than *last_stackbuf.  This can happen
         when the compiler splits this function into different functions,
         one for pass == 1 and one for pass != 1.  */
      return memcmp (zero, *last_stackbuf, SECRET_SIZE) != 0;
    }
}

static void
test_stack (void)
{
  int count = 0;
  int repeat;
  char *volatile last_stackbuf;

  for (repeat = 2 * 1000; repeat > 0; repeat--)
    {
      /* This odd way of writing two consecutive statements
           do_secret_stuff (1, &last_stackbuf);
           count += do_secret_stuff (2, &last_stackbuf);
         ensures that the two do_secret_stuff calls are performed with the same
         stack pointer value, on m68k.  */
      if ((repeat % 2) == 0)
        do_secret_stuff (1, &last_stackbuf);
      else
        count += do_secret_stuff (2, &last_stackbuf);
    }
  /* If explicit_bzero works, count is near 0.  (It may be > 0 if there were
     some asynchronous signal invocations between the two calls of
     do_secret_stuff.)
     If explicit_bzero is optimized away by the compiler, count comes out as
     approximately 1000.  */
  printf ("test_stack: count = %d\n", count);
  ASSERT (count < 50);
}

#endif /* ! address sanitizer enabled */

/* ========================================================================== */

int
main ()
{
  test_static ();
  test_heap ();
  test_stack ();

  return test_exit_status;
}
