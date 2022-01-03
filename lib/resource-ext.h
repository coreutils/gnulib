/* Extra functions for resource usage.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

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

#ifndef _RESOURCE_EXT_H
#define _RESOURCE_EXT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Returns the amount of address space currently in use by the current
   process, or zero if unknown.
   This is the quantity which is limited by setrlimit(RLIMIT_AS,...).
   Note: This function always returns zero on AIX.  */
extern uintptr_t get_rusage_as (void);

/* Returns the size of the data segment, or zero if unknown.
   This is the quantity which is limited by setrlimit(RLIMIT_DATA,...).
   Note: This function always returns zero on HP-UX 11.00.
   Note: The total size of all malloc()ed memory is bounded by the size of
   the data segment only on the following platforms:
   FreeBSD, AIX, HP-UX 11.23 and newer, IRIX, OSF/1, Solaris, BeOS, Haiku.  */
extern uintptr_t get_rusage_data (void);


#ifdef __cplusplus
}
#endif

#endif /* _RESOURCE_EXT_H */
