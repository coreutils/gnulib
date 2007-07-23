/* popcount.h -- population count
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Ben Pfaff.  */

#ifndef POPCOUNT_H
# define POPCOUNT_H 1

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR >= 4)
#define POPCOUNT_CALCULATION(NAME)              \
        return __builtin_##NAME (x);
#else
#define POPCOUNT_CALCULATION(NAME)              \
        int pop;                                \
        for (pop = 0; x != 0; pop++)            \
          x &= x - 1;                           \
        return pop;
#endif

/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcount (unsigned int x)
{
  POPCOUNT_CALCULATION (popcount);
}

/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcountl (unsigned long int x)
{
  POPCOUNT_CALCULATION (popcountl);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcountll (unsigned long long int x)
{
  POPCOUNT_CALCULATION (popcountll);
}
#endif

#endif /* POPCOUNT_H */
