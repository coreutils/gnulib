/* Iteration over all valid error numbers.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#ifndef _ERRNO_ITER_H
#define _ERRNO_ITER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*errno_iterate_callback_fn) (void *data, int err);

/* Iterate over the all valid error numbers defined by the system.
   The callback is called once for every valid error number, with the
   following arguments:
     - DATA is the same argument as passed to errno_iterate.
     - ERR is the error number.
   If the callback returns 0, the iteration continues.  If it returns 1,
   the iteration terminates prematurely.  */
extern void errno_iterate (errno_iterate_callback_fn callback, void *data);


#ifdef __cplusplus
}
#endif

#endif /* _ERRNO_ITER_H */
