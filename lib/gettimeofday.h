/* Provide gettimeofday
   1. for systems that don't have it,
   2. for some systems where gettimeofday clobbers the static buffer that
      localtime uses for it's return value.  The gettimeofday function from
      Mac OS X 10.0.4, i.e. Darwin 1.3.7 has this problem.
      The tzset replacement is necessary for at least Solaris 2.5, 2.5.1, and
      2.6.

   Copyright (C) 2001, 2002, 2003, 2005, 2007 Free Software Foundation, Inc.

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

#ifndef GETTIMEOFDAY_H
#define GETTIMEOFDAY_H

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if !HAVE_STRUCT_TIMEVAL
struct timeval
{
  time_t tv_sec;
  suseconds_t tv_usec;
};
#endif

#if !HAVE_GETTIMEOFDAY_POSIX_SIGNATURE || GETTIMEOFDAY_CLOBBERS_LOCALTIME
# undef gettimeofday
# define gettimeofday rpl_gettimeofday
extern int gettimeofday (struct timeval *restrict tp, void *restrict tzp);
#endif

#endif /* GETTIMEOFDAY_H */
