/* Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _STDBOOL_H
#define _STDBOOL_H

/* ISO C 99 <stdbool.h> for platforms that lack it.  */

/* 7.16. Boolean type and values */

/* BeOS <sys/socket.h> already #defines false 0, true 1.  We use the same
   definitions below, but temporarily we have to #undef them.  */
#ifdef __BEOS__
# undef false
# undef true
#endif

/* For the sake of symbolic names in gdb, we define true and false as
   enum constants, not only as macros.
   It is tempting to write
      typedef enum { false = 0, true = 1 } _Bool;
   so that gdb prints values of type 'bool' symbolically. But if we do
   this, values of type '_Bool' may promote to 'int' or 'unsigned int'
   (see ISO C 99 6.7.2.2.(4)); however, '_Bool' must promote to 'int'
   (see ISO C 99 6.3.1.1.(2)).  */
#ifndef __cplusplus
# if !@HAVE__BOOL@
enum { false = 0, true = 1 };
typedef signed char _Bool;
# endif
#else
typedef bool _Bool;
#endif
#define bool _Bool

/* The other macros must be usable in preprocessor directives.  */
#define false 0
#define true 1
#define __bool_true_false_are_defined 1

#endif /* _STDBOOL_H */
