/* Convert calendar time to simple time, inverse of mktime.
   Copyright (C) 2003 Free Software Foundation, Inc.

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

#ifdef HAVE_TIMEGM

/* Get timegm declaration.  */
#include <time.h>

#else

/* Get time_t and struct tm.  */
#include <time.h>

/* Convert calendar time to simple time, inverse of mktime.  */
extern time_t timegm (struct tm *tm);

#endif
