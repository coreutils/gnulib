/* Substitute for and wrapper around <utime.h>.
   Copyright (C) 2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef _@GUARD_PREFIX@_UTIME_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_UTIME_H@
# @INCLUDE_NEXT@ @NEXT_UTIME_H@
#endif

#ifndef _@GUARD_PREFIX@_UTIME_H
#define _@GUARD_PREFIX@_UTIME_H

#if !@HAVE_UTIME_H@
# include <sys/utime.h>
#endif

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

/* Define 'struct utimbuf' as an alias of 'struct _utimbuf'
   (or possibly, if present, 'struct __utimbuf64').  */
# define utimbuf _utimbuf

#endif

#endif /* _@GUARD_PREFIX@_UTIME_H */
#endif /* _@GUARD_PREFIX@_UTIME_H */
