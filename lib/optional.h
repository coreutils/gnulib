/* An Optional type wrapper.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#ifndef _OPTIONAL_H
#define _OPTIONAL_H 1

/* Pointers in C can be NULL; this is typically used to indicate the absence
   of a valid value.  The same feature is not present in structs or unions.

   This file defines a facility to define an Optional type wrapper for a given
   type.  This is a type whose values may be of two kinds:
     SOME (value)     is a value,
     NONE             denotes the absence of a value.

   Loosely modeled after
     Rust Option        <https://doc.rust-lang.org/std/option/>
     C++ std::optional  <https://en.cppreference.com/cpp/utility/optional>
     metalang99 maybe   <https://metalang99.readthedocs.io/en/latest/maybe.html>
 */

#include <stdbool.h>

/* Get abort().  */
#include <stdlib.h>

/* Whether the compiler supports statement-expressions.
   Test program:
     int f (int x) { return ({ x; x; }); }
 */
#if defined __GNUC__ /* both C and C++ mode */ \
    || defined __clang__ /* both C and C++ mode */ \
    || (defined __SUNPRO_C && __SUNPRO_C >= 0x5150) /* C mode */ \
    || (defined __SUNPRO_CC && __SUNPRO_CC >= 0x5150) /* C++ mode */
# define HAVE_STATEMENT_EXPRESSIONS 1
#endif

/* Whether the compiler supports typeof.
   Test program:
     int f (int x) { typeof (x) y = x; return y; }
 */
#if (((defined __GNUC__ && __GNUC__ + (__GNUC_MINOR__ >= 1) > 3) /* both C and C++ mode */ \
      || (defined __clang__ && __clang_major__ >= 3 /* both C and C++ mode */ \
          && !(defined __cplusplus && !defined __GNUC__))) /* except for clang-cl in C++ mode */ \
     && !defined __STRICT_ANSI__) /* but not with -std=c99 or -std=c11 */ \
    || (defined __SUNPRO_C && __SUNPRO_C >= 0x5110) /* C mode */ \
    || __STDC_VERSION__ >= 202311L /* C mode */
# define HAVE_TYPEOF 1
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* A type expression for the Optional wrapper.
   Usually used in a 'typedef'.  */
#define OPTIONAL_TYPE(_TYPE_) \
  struct \
    { \
      /* The fields of this struct should be considered private.  */ \
      _TYPE_ _value; \
      bool _valid; \
    }

/* An instance with a value.
   Usually used within a compound literal.  */
#define SOME(...) \
  { ._value = (__VA_ARGS__), ._valid = true }

/* An instance without a value.
   Usually used within a compound literal.  */
#define NONE \
  { ._valid = false }

/* Returns true for an instance with a value,
   false for an instance without a value.  */
#define has_value(instance) \
  ((instance)._valid)

/* Returns the value of an instance with a value.  */
#if HAVE_STATEMENT_EXPRESSIONS && HAVE_TYPEOF
# define get_value(instance) \
    ({typeof (instance) _instance_ = (instance); \
      if (!_instance_._valid) \
        abort (); \
      _instance_._value; \
    })
#else
# define get_value(instance) \
    ((instance)._value)
#endif


#ifdef __cplusplus
}
#endif

#endif /* _OPTIONAL_H */
