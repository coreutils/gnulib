/* Test of <stddef.h> substitute in C++ mode.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

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

#include <config.h>

/* Define gl_unreachable.  */
#include <stddef.h>
/* Define std::unreachable (in C++23 or newer).  */
#include <utility>

void
test_cxx_unreachable_1 ()
{
  if (2 < 1)
    gl_unreachable ();
#if defined __cpp_lib_unreachable
  if (3 < 1)
    std::unreachable ();
#endif
}

#if defined __cpp_lib_unreachable
using std::unreachable;

void
test_cxx_unreachable_2 ()
{
  if (3 < 1)
    unreachable ();
}
#endif
