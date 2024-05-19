/* abort() function that prints a stack trace before aborting.
   Copyright (C) 2024 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <signal.h>

#if HAVE_LIBBACKTRACE

# include <backtrace.h>

static struct backtrace_state *state /* = NULL */;

static inline void
# if (__GNUC__ >= 3) || (__clang_major__ >= 4)
__attribute__ ((always_inline))
# endif
print_stack_trace (FILE *stream)
{
  if (state == NULL)
    state = backtrace_create_state (NULL, 0, NULL, NULL);
  /* Pass skip=0, to work around <https://github.com/ianlancetaylor/libbacktrace/issues/60>.  */
  fprintf (stream, "Stack trace:\n");
  backtrace_print (state, 0, stream);
}

#elif HAVE_EXECINFO_H

# include <stdio.h>

# include "execinfo.h"

static inline void
# if (__GNUC__ >= 3) || (__clang_major__ >= 4)
__attribute__ ((always_inline))
# endif
print_stack_trace (FILE *stream)
{
  void *buffer[100];
  int max_size = sizeof (buffer) / sizeof (buffer[0]);
  int size = backtrace (buffer, max_size);
  if (size > 0)
    {
      char **symbols = backtrace_symbols (buffer, size);
      if (symbols != NULL)
        {
          int i;

          fprintf (stream, "Stack trace:\n");
          for (i = 0; i < size; i++)
            fprintf (stream, "%s\n", symbols[i]);
          fflush (stream);

          free (symbols);
        }
    }
}

#endif

void
_gl_pre_abort (void)
{
#if HAVE_LIBBACKTRACE || HAVE_EXECINFO_H
  print_stack_trace (stderr);
#endif
}

/*   rpl_abort ();
   is equivalent to
     _gl_pre_abort ();
     original abort (); // i.e. raise (SIGABRT);
 */
void
rpl_abort (void)
{
#if HAVE_LIBBACKTRACE || HAVE_EXECINFO_H
  print_stack_trace (stderr);
#endif
  raise (SIGABRT);
}
