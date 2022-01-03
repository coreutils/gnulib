/* Decomposed printf argument list.
   Copyright (C) 1999, 2002, 2005-2007, 2009-2022 Free Software Foundation,
   Inc.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _U_PRINTF_ARGS_H
#define _U_PRINTF_ARGS_H

#include "unitypes.h"

/* Parametrization of printf-args.h.  */
#define ENABLE_UNISTDIO 1
#define PRINTF_FETCHARGS u_printf_fetchargs
#include "printf-args.h"

#endif /* _U_PRINTF_ARGS_H */
