/* Computed initializations before the program starts.
   Copyright (C) 2025 Free Software Foundation, Inc.
   Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald

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

#ifndef _GL_AT_INIT_H
#define _GL_AT_INIT_H


/* AT_INIT (function);
   registers a function to be executed when the program is initialized, i.e.
   even before the main() function is entered.

   AT_FINI (function);
   registers a function to be executed when the program is about to exit.
   Similar to 'atexit (function)' or 'at_quick_exit (function)'.

   The function must have an empty argument list and return 'void'.

   An initialization function must not depend on the result of other
   initialization functions.  The order in which the initialization functions
   are executed is unspecified.

   These macro invocations must *precede* the definition of the function.
   (This is necessary for clang.)

   These macros apply to executables.  They are not supported in shared
   libraries.

   Note: It would be easy to achieve the same functionality by using a small
   C++ compilation unit.  But it's better to stay with C all the way.  */

/* This is implemented for each compiler separately.  Basically, the idea is
   to look how the corresponding C++ compiler implements static initializations
   of C++ variables with a non-POD type, and do the same thing in C through a
   mix of pragmas and asms.  */
/* For clang on native Windows (which defines both __clang__ and _MSC_VER):
     - Both definitions of AT_INIT work.
     - Only the .CRT$XCU based definition of AT_FINI works; the __destructor__
       based definition of AT_FINI does not work.  */
#if defined __GNUC__ || (defined __clang__ && !defined _MSC_VER)
/* GCC and compatible compilers */

/* Documentation:
   <https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html>  */

# define AT_INIT(function) \
    static void __attribute__ ((__constructor__)) function (void)
# define AT_FINI(function) \
    static void __attribute__ ((__destructor__)) function (void)

#elif defined _MSC_VER
/* MSVC */

/* Taken from glib-2.83 gconstructor.h.
   Documentation:
   <https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-initialization>
   See also <https://stackoverflow.com/questions/1113409/>.  */

/* Avoid an error in the pragma below.  */
# undef read

# ifdef _WIN64
#  define GL_MSVC_SYMBOL_PREFIX ""
# else
#  define GL_MSVC_SYMBOL_PREFIX "_"
# endif

# define AT_INIT(function) GL_MSVC_CTOR (function, GL_MSVC_SYMBOL_PREFIX)
# define AT_FINI(function) GL_MSVC_DTOR (function, GL_MSVC_SYMBOL_PREFIX)

# define GL_MSVC_CTOR(function,_sym_prefix) \
  static void function(void); \
  extern int (* _array ## function)(void); \
  int function ## _wrapper(void); \
  int function ## _wrapper(void) { function(); gl_at_init_dummy (_array ## function); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # function "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## function)(void) = function ## _wrapper

# define GL_MSVC_DTOR(function,_sym_prefix) \
  static void function(void); \
  extern int (* _array ## function)(void); \
  int function ## _constructor(void); \
  int function ## _constructor(void) { atexit (function); gl_at_init_dummy (_array ## function); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # function "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## function)(void) = function ## _constructor

extern void gl_at_init_dummy (const void *);

#elif defined __SUNPRO_C
/* Sun C */

/* Needs also: #pragma init(function)  */
# define AT_INIT(function) \
    static void function (void)

/* Needs also: #pragma fini(function)  */
# define AT_FINI(function) \
    static void function (void)

#elif defined __xlC__
/* AIX xlc */

/* Needs also: #pragma init(function)
   and a C++ linker.  See
   <https://www.ibm.com/docs/de/xl-c-and-cpp-aix/13.1.0?topic=descriptions-pragma-init-only>  */
# define AT_INIT(function) \
    static void function (void)

/* Needs also: #pragma fini(function)
   and a C++ linker.  See
   <https://www.ibm.com/docs/de/xl-c-and-cpp-aix/13.1.0?topic=descriptions-pragma-fini-only>  */
# define AT_FINI(function) \
    static void function (void)

#else
/* Another compiler */

/* This is just a dummy.  It will not work.  */
# define AT_INIT(function) \
    static void function (void)
# define AT_FINI(function) \
    static void function (void)

#endif

#endif /* _GL_AT_INIT_H */
