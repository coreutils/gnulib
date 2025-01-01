/* Calculate the size of physical memory.

   Copyright (C) 2000, 2003, 2009-2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#ifndef PHYSMEM_H_
# define PHYSMEM_H_ 1

#ifdef __cplusplus
extern "C" {
#endif


/* Returns the total amount of physical memory.
   This value is more or less a hard limit for the working set.  */
double physmem_total (void);

/* Returns the amount of physical memory available.
   This value is the amount of memory the application can use without hindering
   any other process (assuming that no other process increases its memory
   usage).  */
double physmem_available (void);

/* Returns the amount of physical memory that can be claimed, with a given
   aggressivity.
   For AGGRESSIVITY == 0.0, the result is like physmem_available (): the amount
   of memory the application can use without hindering any other process.
   For AGGRESSIVITY == 1.0, the result is the amount of memory the application
   can use, while causing memory shortage to other processes, but without
   bringing the machine into an out-of-memory state.
   Values in between, for example AGGRESSIVITY == 0.5, are a reasonable middle
   ground.  */
double physmem_claimable (double aggressivity);


#ifdef __cplusplus
}
#endif

#endif /* PHYSMEM_H_ */
