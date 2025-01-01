/* Test of saving the floating-point exception status flags.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

/* Test the link dependencies: that $(FENV_EXCEPTIONS_STATE_LIBM) is
   sufficient.  */

int
main ()
{
  fexcept_t saved_flags;

  fegetexceptflag (&saved_flags, FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT);
  fesetexceptflag (&saved_flags, FE_OVERFLOW | FE_UNDERFLOW);

  return 0;
}
