/* Support for the Ethiopian / Ge'ez calendar (used in Ethiopia).
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

/* Reference: https://en.wikipedia.org/wiki/Ethiopian_calendar  */

static const struct calendar_month_name ethiopian_month_names[13] =
{
  /* Mäskäräm */ { "መስከረም", "መስ" },
  /* Ṭəqəmt */   { "ጥቅምት", "ጥን" },
  /* Ḫədar */    { "ኅዳር", "ኅዳ" },
  /* Taḫśaś */   { "ታኅሣሥ", "ታህ" },
  /* Ṭərr */     { "ጥር", "ጥር" },
  /* Yäkatit */  { "የካቲት", "የካ" },
  /* Mägabit */  { "መጋቢት", "መጋ" },
  /* Miyazya */  { "ሚያዝያ", "ሚያ" },
  /* Gənbot */   { "ግንቦት", "ግን" },
  /* Säne */     { "ሰኔ", "ሰኔ" },
  /* Ḥamle */    { "ሐምሌ", "ሐም" },
  /* Nähase */   { "ነሐሴ", "ነሐ" },
  /* Ṗagume */   { "ጳጉሜን" /* or "ጳጐሜን" or "ጳጉሜ" */, "ጳጉ" },
};

static int
gregorian_to_ethiopian (struct calendar_date *result,
                        int greg_year, int greg_month, int greg_day)
{
  if (greg_year > 1900 && greg_year < 2100)
    {
      /* Simplify leap year calculations by considering year start
         March 1.  */
      greg_month -= 2;
      if (greg_month < 0)
        {
          greg_month += 12;
          greg_year -= 1;
        }
      int greg_days_since_march_1 =
        /* greg_month  0  1  2  3   4   5   6   7   8   9  10  11
           days        0 31 61 92 122 153 184 214 245 275 306 337  */
        ((greg_month * 153 + 2) / 5)
        + (greg_day - 1);
      int greg_days_this_year = 365 + __isleap (greg_year + 1);
      /* There are 171 days from Sep. 11 to Feb. 28 of the next year,
         or from Sep. 12 to Feb. 29 of the next year (inclusive).  */
      int days_since_year_start = greg_days_since_march_1 + 171;
      int year = greg_year;
      if (days_since_year_start >= greg_days_this_year)
        {
          days_since_year_start -= greg_days_this_year;
          year += 1;
        }
      result->year = year - 8;
      result->month = days_since_year_start / 30; /* in the range 0..12 ! */
      result->day = (days_since_year_start % 30) + 1;
      result->month_names = ethiopian_month_names;
      return 0;
    }
  return -1;
}

static const struct calendar ethiopian_calendar =
{
  gregorian_to_ethiopian,
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

  if (gregorian_to_ethiopian (&cday, greg_year, greg_month, greg_day) == 0)
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
