/* print_stack_trace() function that prints a stack trace.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

#if HAVE_LIBBACKTRACE

# include <backtrace.h>

static struct backtrace_state *state /* = NULL */;

static inline void
# if (__GNUC__ >= 3) || (__clang_major__ >= 4)
__attribute__ ((always_inline))
# endif
print_stack_trace_to (FILE *stream)
{
  if (state == NULL)
    state = backtrace_create_state (NULL, 0, NULL, NULL);
  fprintf (stream, "Stack trace:\n");
  /* Pass skip=0, to work around <https://github.com/ianlancetaylor/libbacktrace/issues/60>.  */
  backtrace_print (state, 0, stream);
}

#elif HAVE_LIBASAN

# include <stdio.h>

/* We need only one declaration from <sanitizer/asan_interface.h>.  */
extern
# ifdef __cplusplus
"C"
# endif
void __sanitizer_print_stack_trace (void);

/* The only supported stream, in this case, is stderr.  */
static inline void
# if (__GNUC__ >= 3) || (__clang_major__ >= 4)
__attribute__ ((always_inline))
# endif
print_stack_trace_to (FILE *stream)
{
  fprintf (stream, "Stack trace:\n");
  __sanitizer_print_stack_trace ();
}

#elif HAVE_EXECINFO_H

# include <stdio.h>

# include "execinfo.h"

static inline void
# if (__GNUC__ >= 3) || (__clang_major__ >= 4)
__attribute__ ((always_inline))
# endif
print_stack_trace_to (FILE *stream)
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
