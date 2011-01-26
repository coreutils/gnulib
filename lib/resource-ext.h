/* Extra functions for resource usage.
   Copyright (C) 2011 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _RESOURCE_EXT_H
#define _RESOURCE_EXT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Returns the amount of address space currently in use by the current
   process, or zero if unknown.
   This is the quantity which is limited by setrlimit(RLIMIT_AS,...).
   Note: This function always returns zero on OpenBSD and AIX.  */
extern uintptr_t get_rusage_as (void);


#ifdef __cplusplus
}
#endif

#endif /* _RESOURCE_EXT_H */
