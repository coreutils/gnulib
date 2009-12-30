/* Provide a sys/times.h header file.
   Copyright (C) 2008-2009 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson <simon@josefsson.org>, 2008.  */

/* This file is supposed to be used on platforms where <sys/times.h>
   is missing.  */

#ifndef _GL_SYS_TIMES_H

# if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
# endif

# if @HAVE_SYS_TIMES_H@
#  @INCLUDE_NEXT@ @NEXT_SYS_TIMES_H@
# endif

# define _GL_SYS_TIMES_H

/* Get clock_t. */
# include <time.h>

/* The definition of GL_LINK_WARNING is copied here.  */

/* The definition of _GL_ARG_NONNULL is copied here.  */

# ifdef __cplusplus
extern "C" {
# endif

# if !@HAVE_STRUCT_TMS@
  /* Structure describing CPU time used by a process and its children.  */
  struct tms
  {
    clock_t tms_utime;          /* User CPU time.  */
    clock_t tms_stime;          /* System CPU time.  */

    clock_t tms_cutime;         /* User CPU time of dead children.  */
    clock_t tms_cstime;         /* System CPU time of dead children.  */
  };
# endif

# if @GNULIB_TIMES@
#  if !@HAVE_TIMES@
  extern clock_t times (struct tms *buffer) _GL_ARG_NONNULL ((1));
#  endif
# elif defined GNULIB_POSIXCHECK
#  undef times
#  define times(s)                                              \
  (GL_LINK_WARNING ("times is unportable - "                    \
                    "use gnulib module times for portability"), \
   times (s))
# endif

# ifdef __cplusplus
}
# endif

#endif                          /* _GL_SYS_TIMES_H */
