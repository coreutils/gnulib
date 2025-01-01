/* Functions for controlling the floating-point environment as a whole.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.

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
#include <fenv.h>

int
feholdexcept (fenv_t *envp)
{
  if (fegetenv (envp))
    return -1;
  if (feclearexcept (FE_ALL_EXCEPT))
    return -1;
  if (fedisableexcept (FE_ALL_EXCEPT) == -1)
    return -1;
  return 0;
}
