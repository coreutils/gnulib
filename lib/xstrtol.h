/* A more useful interface to strtol.

   Copyright (C) 1995, 1996, 1998, 1999, 2001-2004, 2006-2007
   Free Software Foundation, Inc.

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

#ifndef XSTRTOL_H_
# define XSTRTOL_H_ 1

# include "exitfail.h"

# include <inttypes.h>

# include "gettext.h"

# ifndef _STRTOL_ERROR
enum strtol_error
  {
    LONGINT_OK = 0,

    /* These two values can be ORed together, to indicate that both
       errors occurred.  */
    LONGINT_OVERFLOW = 1,
    LONGINT_INVALID_SUFFIX_CHAR = 2,

    LONGINT_INVALID_SUFFIX_CHAR_WITH_OVERFLOW = (LONGINT_INVALID_SUFFIX_CHAR
						 | LONGINT_OVERFLOW),
    LONGINT_INVALID = 4
  };
typedef enum strtol_error strtol_error;
# endif

# define _DECLARE_XSTRTOL(name, type) \
  strtol_error name (const char *, char **, int, type *, const char *);
_DECLARE_XSTRTOL (xstrtol, long int)
_DECLARE_XSTRTOL (xstrtoul, unsigned long int)
_DECLARE_XSTRTOL (xstrtoimax, intmax_t)
_DECLARE_XSTRTOL (xstrtoumax, uintmax_t)

/* Signal an error for an out-of-range integer argument, through the error()
   function.
   EXIT_CODE is the exit code (0 for a non-fatal error).
   STR is the value of the given argument value.
   OPTION is the option that takes the argument (usually starting with one
   or two minus signs).
   ERR is the error code returned by one of the xstrto* functions.  */
# define _STRTOL_ERROR(Exit_code, Str, Option, Err)			\
  do									\
    {									\
      switch ((Err))							\
	{								\
	default:							\
	  abort ();							\
									\
	case LONGINT_INVALID:						\
	  error ((Exit_code), 0, gettext ("invalid %s argument `%s'"),	\
		 (Option), (Str));					\
	  break;							\
									\
	case LONGINT_INVALID_SUFFIX_CHAR:				\
	case LONGINT_INVALID_SUFFIX_CHAR | LONGINT_OVERFLOW:		\
	  error ((Exit_code), 0,					\
		 gettext ("invalid character following %s argument in `%s'"), \
		 (Option), (Str));					\
	  break;							\
									\
	case LONGINT_OVERFLOW:						\
	  error ((Exit_code), 0, gettext ("%s argument `%s' too large"), \
		 (Option), (Str));					\
	  break;							\
	}								\
    }									\
  while (0)

# define STRTOL_FATAL_ERROR(Str, Option, Err)				\
  _STRTOL_ERROR (exit_failure, Str, Option, Err)

# define STRTOL_FAIL_WARN(Str, Option, Err)				\
  _STRTOL_ERROR (0, Str, Option, Err)

#endif /* not XSTRTOL_H_ */
