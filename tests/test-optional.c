/* Test of Optional wrapper type.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

#include "optional.h"

#include "macros.h"

/* A point in 3D.  */
struct point { double x, y, z; };

/* A type that may contain a point or none.  */
typedef OPTIONAL_TYPE(struct point) optional_point;

/* A function that returns an optional_point.  */
static optional_point
spherical_inversion (struct point p)
{
  double r = p.x * p.x + p.y * p.y + p.z * p.z;
  if (r == 0)
    /* We cannot represent the point at infinity as a point in the
       affine space.  */
    return (optional_point) NONE;
  else
    return (optional_point) SOME ((struct point) { .x = p.x / r, .y = p.y / r, .z = p.z / r });
}

int
main ()
{
  optional_point a = spherical_inversion ((struct point) { .x = 3, .y = 4, .z = 12 });
  optional_point b = spherical_inversion ((struct point) { .x = 0, .y = 0, .z = 0 });
  ASSERT (has_value (a));
  ASSERT (!has_value (b));

  struct point p = get_value (a);
  ASSERT (p.x > 0.0177 && p.x < 0.0178);
  ASSERT (p.y > 0.0236 && p.y < 0.0237);
  ASSERT (p.z > 0.0710 && p.z < 0.0711);

  return test_exit_status;
}
