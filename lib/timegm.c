/* Convert calendar time to simple time, inverse of mktime.
   Copyright (C) 1995, 1996, 1997, 1998, 2000, 2001, 2003
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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  Get mktime and gmtime declarations.  */
#include "timegm.h"

/* Converts struct tm to time_t, assuming the data in tm is UTC rather
   than local timezone.

   mktime is similar but assumes struct tm, also known as the
   "broken-down" form of time, is in local time zone.  mktime_from_utc
   uses mktime to make the conversion understanding that an offset
   will be introduced by the local time assumption.

   mktime_from_utc then measures the introduced offset by applying
   gmtime to the initial result and applying mktime to the resulting
   "broken-down" form.  The difference between the two mktime results
   is the measured offset which is then subtracted from the initial
   mktime result to yield a calendar time which is the value returned.

   tm_isdst in struct tm is set to 0 to force mktime to introduce a
   consistent offset (the non DST offset) since tm and tm+o might be
   on opposite sides of a DST change.

   Some implementations of mktime return -1 for the nonexistent
   localtime hour at the beginning of DST.  In this event, use
   mktime(tm - 1hr) + 3600.

   Schematically
   mktime(tm)   --> t+o
   gmtime(t+o)  --> tm+o
   mktime(tm+o) --> t+2o
   t+o - (t+2o - t+o) = t

   Note that glibc contains a function of the same purpose named
   `timegm' (reverse of gmtime).  But obviously, it is not universally
   available, and unfortunately it is not straightforwardly
   extractable for use here.  Perhaps configure should detect timegm
   and use it where available.

   Contributed by Roger Beeman <beeman@cisco.com>, with the help of
   Mark Baushke <mdb@cisco.com> and the rest of the Gurus at CISCO.
   Further improved by Roger with assistance from Edward J. Sabol
   based on input by Jamie Zawinski.  */

time_t
timegm (struct tm *tm)
{
  time_t tl, tb;
  struct tm *tg;

  tl = mktime (tm);
  if (tl == (time_t) -1)
    {
      tm->tm_hour--;
      tl = mktime (tm);
      if (tl == (time_t) -1)
	return (time_t) -1;
      tl += 3600;
    }
#if HAVE_GMTIME_R && HAVE_DECL_GMTIME_R
  tg = gmtime_r (&tl, tg);
#else
  tg = gmtime (&tl);
#endif
  tg->tm_isdst = 0;
  tb = mktime (tg);
  if (tb == (time_t) -1)
    {
      tg->tm_hour--;
      tb = mktime (tg);
      if (tb == (time_t) -1)
	return (time_t) -1;
      tb += 3600;
    }
  return (tl - (tb - tl));
}
