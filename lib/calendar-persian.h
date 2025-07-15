/* Support for the Persian solar Hijri calendar (used in Iran).
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

/* Reference: https://en.wikipedia.org/wiki/Solar_Hijri_calendar
   More info regarding the leap years: Some online date converters and also
   Emacs (cal-persia.el) get the leap years wrong: they pretend that year 1403
   is a non-leap year and that 1404 is a leap year. The correct info, based on
   https://fa.wikipedia.org/wiki/%DA%AF%D8%A7%D9%87%E2%80%8C%D8%B4%D9%85%D8%A7%D8%B1%DB%8C_%D8%B1%D8%B3%D9%85%DB%8C_%D8%A7%DB%8C%D8%B1%D8%A7%D9%86
   and https://github.com/movahhedi/persian-leap ,
   is that the following years are leap years:
     1276, 1280, 1284, 1288, 1292, 1296, 1300, 1304,
     1309, 1313, 1317, 1321, 1325, 1329, 1333, 1337,
     1342, 1346, 1350, 1354, 1358, 1362, 1366, 1370,
     1375, 1379, 1383, 1387, 1391, 1395, 1399, 1403,
     1408, 1412, 1416, 1420, 1424, 1428, 1432, 1436,
     1441, 1445, 1449, 1453, 1457, 1461, 1465, 1469,
     1474, 1478, 1482, 1486, 1490, 1494, 1498.
   This is consistent with the table in
   https://en.wikipedia.org/wiki/Solar_Hijri_calendar#Comparison_with_Gregorian_calendar
 */

static const struct calendar_month_name persian_month_names[12] =
{
  /* Farvardin */   { "فروردین", "فروردین" },
  /* Ordibehesht */ { "اردیبهشت", "اردیبهشت" },
  /* Khordad */     { "خرداد", "خرداد" },
  /* Tir */         { "تیر", "تیر" },
  /* Mordad */      { "مرداد", "مرداد" },
  /* Shahrivar */   { "شهریور", "شهریور" },
  /* Mehr */        { "مهر", "مهر" },
  /* Aban */        { "آبان", "آبان" },
  /* Azar */        { "آذر", "آذر" },
  /* Dey */         { "دی", "دی" },
  /* Bahman */      { "بهمن", "بهمن" },
  /* Esfand */      { "اسفند", "اسفند" },
};

static int
gregorian_to_persian (struct calendar_date *result,
                      int greg_year, int greg_month, int greg_day)
{
  if ((greg_year > 1925 && greg_year < 1975)
      || (greg_year > 1978 && greg_year < 2100))
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
      int greg_days_since_1900_march_1 =
        (greg_year - 1900) * 365
        + ((greg_year - 1900) / 4)
        - ((greg_year - 1900) / 100)
        + ((greg_year - 1600) / 400)
        + greg_days_since_march_1;
      /* The Hijri calendar currently uses 33-year cycles of 12053 days each
         (8 leap years and 25 non-leap years).
         For our purposes, let's define the start of such a cycle as the
         beginning of the year that follows the leap year that follows
         the 4 non-leap years:
         1931-03-22, 1964-03-21, 1997-03-21, 2030-03-21, 2063-03-21, 2096-03-20,
         ...  */
      int cycle33_number = (greg_days_since_1900_march_1 + 710) / 12053;
      int days_since_cycle33_start = (greg_days_since_1900_march_1 + 710) % 12053;
      /* In such a 33-year cycle, the days after a leap year end are at days
         0, 1461, 2922, 4383, 5844, 7305, 8766, 10227;
         these are the multiples of 1461.
         We call the period that starts with 3 or 4 non-leap years and ends
         with a leap year a "leap cycle".  Thus these 8 days are the beginning
         of the 8 leap cycles in the 33-year cycle.  */
      int leap_years_since_cycle33_start = days_since_cycle33_start / 1461;
      if (leap_years_since_cycle33_start > 7)
        leap_years_since_cycle33_start = 7;
      int is_last_day_of_leap_year =
        (((days_since_cycle33_start + 1) % 1461) == 0
         && days_since_cycle33_start <= 10227)
        || (days_since_cycle33_start == 12053 - 1);
      int days_since_leapcycle_start =
        days_since_cycle33_start - leap_years_since_cycle33_start * 1461;
      int full_years_since_leapcycle_start =
        days_since_leapcycle_start / 365 - is_last_day_of_leap_year;
      int full_years_since_cycle33_start =
        leap_years_since_cycle33_start * 4 + full_years_since_leapcycle_start;
      int year = 1277 + cycle33_number * 33 + full_years_since_cycle33_start;
      int days_since_year_start =
        days_since_leapcycle_start - full_years_since_leapcycle_start * 365;
      int month;
      int days_since_month_start;
      if (days_since_year_start < 186)
        {
          month = days_since_year_start / 31;
          days_since_month_start = days_since_year_start % 31;
        }
      else
        {
          month = (days_since_year_start - 6) / 30;
          days_since_month_start = (days_since_year_start - 6) % 30;
        }
      result->year = year;
      result->month = month;
      result->day = days_since_month_start + 1;
      result->month_names = persian_month_names;
      return 0;
    }
  return -1;
}

static const struct calendar persian_calendar =
{
  gregorian_to_persian,
  "%OY/%Om/%Od",
  0xDBB0                /* The alternate digits are U+06F0..U+06F9.  */
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

  if (gregorian_to_persian (&cday, greg_year, greg_month, greg_day) == 0)
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
