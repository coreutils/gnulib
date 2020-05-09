/* ATTRIBUTE_* macros for using attributes in GCC and similar compilers

   Copyright 2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

/* Provide public ATTRIBUTE_* names for the private _GL_ATTRIBUTE_*
   macros used within Gnulib.  */

/* These attributes can be placed in two ways:
     - At the start of a declaration (i.e. even before storage-class
       specifiers!); then they apply to all entities that are declared
       by the declaration.
     - Immediately after the name of an entity being declared by the
       declaration; then they apply to that entity only.  */

#ifndef _GL_ATTRIBUTE_H
#define _GL_ATTRIBUTE_H


/* C2X standard attributes have macro names that do not begin with
   'ATTRIBUTE_'.  */

/* Tells that the entity is deprecated.  A use of the entity produces a
   warning.  */
/* Applies to:
     - function, variable,
     - struct, union, struct/union member,
     - enumeration, enumeration item,
     - typedef,
   in C++ also: namespace, class, template specialization.  */
#define DEPRECATED _GL_ATTRIBUTE_DEPRECATED

/* Tells that it's intentional that the control flow can go on to the
   immediately following 'case' or 'default' label.  */
/* Applies to: Empty statement (;), inside a 'switch' statement.  */
#define FALLTHROUGH _GL_ATTRIBUTE_FALLTHROUGH

/* Tells that it's OK if the entity is not used.  No "is not used" warning
   shall be produced for the entity.  */
/* Applies to:
     - function, variable,
     - struct, union, struct/union member,
     - enumeration, enumeration item,
     - typedef,
   in C++ also: class.  */
#define MAYBE_UNUSED _GL_ATTRIBUTE_MAYBE_UNUSED

/* Tells that it is important for the caller of a function to inspect its
   return value.  Discarding the return value produces a warning.  */
/* Applies to: function, enumeration, class.  */
#define NODISCARD _GL_ATTRIBUTE_NODISCARD


/* Selected GCC attributes; see:
   https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html
   https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html
   https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html
   These names begin with 'ATTRIBUTE_' to avoid name clashes.  */

/* Used for memory allocation functions.
   ATTRIBUTE_ALLOC_SIZE ((n)) specifies that the function returns a block
   of memory whose size is the n-th argument (n counts from 1).
   ATTRIBUTE_ALLOC_SIZE ((m, n)) specifies that the function returns a block
   of memory whose size is the product of the m-th argument and the n-th
   argument.  */
/* Applies to: functions, pointers to functions, function types */
#define ATTRIBUTE_ALLOC_SIZE(args) _GL_ATTRIBUTE_ALLOC_SIZE (args)

/* Forces the function to be inlined.  Results in an error if the compiler
   cannot honour the inlining request.  */
/* Applies to: functions */
#define ATTRIBUTE_ALWAYS_INLINE _GL_ATTRIBUTE_ALWAYS_INLINE

/* Used for small inline functions.
   Specifies that the function should be omitted from stack traces.  */
/* Applies to: functions */
#define ATTRIBUTE_ARTIFICIAL _GL_ATTRIBUTE_ARTIFICIAL

/* Used to denote the result of manual profiling.
   Specifies that the function is unlikely to be executed.  */
/* Applies to: functions */
#define ATTRIBUTE_COLD _GL_ATTRIBUTE_COLD

/* Used on functions that don't inspect memory and don't do side effects
   and don't invoke random behaviour.
   Specifies that the compiler is allowed to replace multiple invocations
   of the function with the same arguments with a single invocation.  */
/* Applies to: functions */
#define ATTRIBUTE_CONST _GL_ATTRIBUTE_CONST

/* Same as DEPRECATED, above.  */
#define ATTRIBUTE_DEPRECATED _GL_ATTRIBUTE_DEPRECATED

/* Used on functions that should not be called.
   ATTRIBUTE_ERROR (MSG) specifies that the compiler is allowed to produce
   an error when an invocation of the function cannot be optimized away.
   The error message will include MSG.  */
/* Applies to: functions */
#define ATTRIBUTE_ERROR(msg) _GL_ATTRIBUTE_ERROR (msg)

/* Used on functions or variables that shall persist across the
   '-fwhole-program' optimization.  */
/* Applies to: functions */
#define ATTRIBUTE_EXTERNALLY_VISIBLE _GL_ATTRIBUTE_EXTERNALLY_VISIBLE

/* Used on functions that consume arguments according to a format string.
   ATTRIBUTE_FORMAT ((FORMAT-STRING-TYPE, FORMAT-STRING-ARGUMENT-NUMBER,
                      FIRST-CONSUMED-ARGUMENT-NUMBER))
   specifies that the argument FORMAT-STRING-ARGUMENT-NUMBER is a format string.
   The syntax of this format string is described by FORMAT-STRING-TYPE, namely
   one of:
     printf, gnu_printf
     scanf, gnu_scanf,
     strftime, gnu_strftime,
     strfmon,
   or the same thing prefixed and suffixed with '__'.
   If FIRST-CONSUMED-ARGUMENT-NUMBER is not 0, the compiler checks that the
   arguments starting at FIRST-CONSUMED-ARGUMENT-NUMBER are suitable for the
   format string.  */
/* Applies to: functions */
#define ATTRIBUTE_FORMAT(spec) _GL_ATTRIBUTE_FORMAT (spec)

/* Specifies that the function does not - directly or indirectly - invoke any
   code from the current compilation unit.
   This attribute allows the compiler to do more aggressive optimizations on
   the current compilation unit.  */
/* Applies to: functions */
#define ATTRIBUTE_LEAF _GL_ATTRIBUTE_LEAF

/* Specifies that pointers to the type may point to the same storage as
   pointers to other types.
   This attribute disables compiler optimizations that are based on
   "no aliasing" assumptions for the given type.  */
/* Applies to: types */
#define ATTRIBUTE_MAY_ALIAS _GL_ATTRIBUTE_MAY_ALIAS

/* Used for memory allocation functions.
   Specifies that the function returns a pointer to freshly allocated
   memory.  */
/* Applies to: functions */
#define ATTRIBUTE_MALLOC _GL_ATTRIBUTE_MALLOC

/* Specifies that the function should not be inlined.  */
/* Applies to: functions */
#define ATTRIBUTE_NOINLINE _GL_ATTRIBUTE_NOINLINE

/* ATTRIBUTE_NONNULL ((ARGNO1, ARGNO2, ...)) specifies that the argument number
   ARGNO1, the argument number ARGNO2, etc. must not be NULL pointers.  */
/* Applies to: functions */
#define ATTRIBUTE_NONNULL(args) _GL_ATTRIBUTE_NONNULL (args)

/* Specifies that the contents of a character array is not meant to be NUL-
   terminated.  */
/* Applies to: struct/union members and variables that are arrays of element
   type '[[un]signed] char'.  */
#define ATTRIBUTE_NONSTRING _GL_ATTRIBUTE_NONSTRING

/* Specifies that the function does not throw exceptions.  */
/* Applies to: functions */
#define ATTRIBUTE_NOTHROW _GL_ATTRIBUTE_NOTHROW

/* For struct members: Specifies that the struct member shall have the minimum
   possible alignment.
   For struct, union, class: Specifies that all members shall have the minimum
   possible alignment, thus minimizing the memory size of the type.  */
/* Applies to: struct members, struct, union,
   in C++ also: class.  */
#define ATTRIBUTE_PACKED _GL_ATTRIBUTE_PACKED

/* Used on functions that don't do side effects and don't invoke random
   behaviour.
   Specifies that the compiler is allowed to replace multiple invocations
   of the function with the same arguments with a single invocation, if
   there are no memory stores in between the invocations.  */
/* Applies to: functions */
#define ATTRIBUTE_PURE _GL_ATTRIBUTE_PURE

/* Specifies that the function's return value is a non-NULL pointer.  */
/* Applies to: functions */
#define ATTRIBUTE_RETURNS_NONNULL _GL_ATTRIBUTE_RETURNS_NONNULL

/* Used on variadic functions.
   ATTRIBUTE_SENTINEL () and ATTRIBUTE_SENTINEL (0) specify that the last
   argument must be NULL.
   ATTRIBUTE_SENTINEL (n) specifies that the (n+1)st argument from the end
   must be NULL.  */
/* Applies to: functions */
#define ATTRIBUTE_SENTINEL(pos) _GL_ATTRIBUTE_SENTINEL (pos)

/* Used on functions that should not be called.
   ATTRIBUTE_WARNING (MSG) specifies that the compiler is allowed to produce
   a warning when an invocation of the function cannot be optimized away.
   The warning message will include MSG.  */
/* Applies to: functions */
#define ATTRIBUTE_WARNING(msg) _GL_ATTRIBUTE_WARNING (msg)


#endif /* _GL_ATTRIBUTE_H */
