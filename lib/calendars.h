/* Support for the non-Gregorian calendars.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

struct calendar_month_name
{
  const char *full;
  const char *abbrev;
};

struct calendar_date
{
  int year;
  int month; /* >= 0 */
  int day;   /* >= 1 */
  const struct calendar_month_name *month_names;
};

struct calendar
{
  /* Converts a Gregorian date
     (greg_year = year, greg_month = month - 1, greg_day = day)
     to a date in this calendar and returns 0.
     Upon failure, returns -1.  */
  int (*from_gregorian) (struct calendar_date *result,
                         int greg_year, int greg_month, int greg_day);
  /* Format string for the %x directive.  */
  const char *d_fmt;
  /* Base of alternate digits (assuming UTF-8 encoding).  */
  unsigned int alt_digits_base;
};

#include "calendar-thai.h"
#include "calendar-persian.h"
#include "calendar-ethiopian.h"
