/* Support for the Thai solar calendar (used in Thailand).
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

/* Reference: https://en.wikipedia.org/wiki/Thai_solar_calendar  */

static const struct calendar_month_name thai_month_names[15] =
{
  /* This array actually contains two overlapping arrays:
     [0..11] are for greg_year >= 1941,
     [3..14] are for greg_year < 1941.  */
  /* January */   { "มกราคม", "ม.ค." },
  /* February */  { "กุมภาพันธ์", "ก.พ." },
  /* March */     { "มีนาคม", "มี.ค." },
  /* April */     { "เมษายน", "เม.ย." },
  /* May */       { "พฤษภาคม", "พ.ค." },
  /* June */      { "มิถุนายน", "มิ.ย." },
  /* July */      { "กรกฎาคม", "ก.ค." },
  /* August */    { "สิงหาคม", "ส.ค." },
  /* September */ { "กันยายน", "ก.ย." },
  /* October */   { "ตุลาคม", "ต.ค." },
  /* November */  { "พฤศจิกายน", "พ.ย." },
  /* December */  { "ธันวาคม", "ธ.ค." },
  /* January */   { "มกราคม", "ม.ค." },
  /* February */  { "กุมภาพันธ์", "ก.พ." },
  /* March */     { "มีนาคม", "มี.ค." },
};

static int
gregorian_to_thai (struct calendar_date *result,
                   int greg_year, int greg_month, int greg_day)
{
  if (greg_year > 1912)
    {
      result->day = greg_day;
      if (greg_year < 1941)
        {
          if (greg_month < 3)
            {
              result->year = greg_year + 542;
              result->month = greg_month + 9;
            }
          else
            {
              result->year = greg_year + 543;
              result->month = greg_month - 3;
            }
          result->month_names = thai_month_names + 3;
        }
      else
        {
          result->year = greg_year + 543;
          result->month = greg_month;
          result->month_names = thai_month_names;
        }
      return 0;
    }
  return -1;
}

static const struct calendar thai_calendar =
{
  gregorian_to_thai,
  "%d/%m/%Y",
  '0'
};


#ifdef TEST

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
  int greg_year = atoi (argv[1]);
  int greg_month = atoi (argv[2]) - 1;
  int greg_day = atoi (argv[3]);
  struct calendar_date cday;

  if (gregorian_to_thai (&cday, greg_year, greg_month, greg_day) == 0)
    {
      printf ("%d-%d-%d -> %d-%d(%s)-%02d\n",
              greg_year, 1+greg_month, greg_day,
              cday.year, 1+cday.month, cday.month_names[cday.month].full, cday.day);
    }
}

/*
 * Local Variables:
 * compile-command: "gcc -ggdb -DTEST -Wall -x c calendars.h"
 * End:
 */

#endif
