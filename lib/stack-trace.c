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

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include "stack-trace-impl.h"

void
print_stack_trace (void)
{
#if HAVE_LIBBACKTRACE || HAVE_LIBASAN || HAVE_EXECINFO_H
  print_stack_trace_to (stderr);
#endif
}
