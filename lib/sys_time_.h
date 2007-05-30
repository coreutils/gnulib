/* Provide a more complete sys/time.h.

   Copyright (C) 2007 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#ifndef _GL_SYS_TIME_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_SYS_TIME_H@
# if @HAVE_INCLUDE_NEXT@
#  include_next <sys/time.h>
# else
#  include @ABSOLUTE_SYS_TIME_H@
# endif
#else
# include <time.h>
#endif

#ifndef _GL_SYS_TIME_H
#define _GL_SYS_TIME_H

#if ! @HAVE_STRUCT_TIMEVAL@
struct timeval
{
  time_t tv_sec;
  long int tv_usec;
};
#endif

#if @REPLACE_GETTIMEOFDAY@
# undef gettimeofday
# define gettimeofday rpl_gettimeofday
int gettimeofday (struct timeval *restrict, void *restrict);
#endif

#endif /* _GL_SYS_TIME_H */
#endif /* _GL_SYS_TIME_H */
