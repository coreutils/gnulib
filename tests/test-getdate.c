/* Test of getdate() function.
   Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Simon Josefsson <simon@josefsson.org>, 2008.  */

#include <config.h>

#include "getdate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "progname.h"
#include "macros.h"

#ifdef DEBUG
#define LOG(str, now, res)                                              \
  printf ("string `%s' diff %d %d\n",                   \
          str, res.tv_sec - now.tv_sec, res.tv_nsec - now.tv_nsec);
#else
#define LOG(str, now, res) (void) 0
#endif

static const char* const day_table[] =
{
  "SUNDAY",
  "MONDAY",
  "TUESDAY",
  "WEDNESDAY",
  "THURSDAY",
  "FRIDAY",
  "SATURDAY",
  NULL
};

int
main (int argc _GL_UNUSED, char **argv)
{
  struct timespec result;
  struct timespec result2;
  struct timespec now;
  const char *p;
  int i;

  set_program_name (argv[0]);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "now";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec == result.tv_sec && now.tv_nsec == result.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "tomorrow";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec + 24 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "yesterday";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec - 24 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "4 hours";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec + 4 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  /* test if timezone is not being ignored for day offset */
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 +24 hours";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +1 day";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  /* test if several time zones formats are handled same way */
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+14:00";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+14";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  p = "UTC+1400";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC-14:00";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC-14";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  p = "UTC-1400";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+0:15";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+0015";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC-1:30";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC-130";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);


  /* TZ out of range should cause get_date failure */
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+25:00";
  ASSERT (!get_date (&result, p, &now));

        /* Check for several invalid countable dayshifts */
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+4:00 +40 yesterday";
  ASSERT (!get_date (&result, p, &now));
  p = "UTC+4:00 next yesterday";
  ASSERT (!get_date (&result, p, &now));
  p = "UTC+4:00 tomorrow ago";
  ASSERT (!get_date (&result, p, &now));
  p = "UTC+4:00 40 now ago";
  ASSERT (!get_date (&result, p, &now));
  p = "UTC+4:00 last tomorrow";
  ASSERT (!get_date (&result, p, &now));
  p = "UTC+4:00 -4 today";
  ASSERT (!get_date (&result, p, &now));

  /* And check correct usage of dayshifts */
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 tomorrow";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +1 day";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 yesterday";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 1 day ago";
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  now.tv_sec = 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 now";
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +0 minutes"; /* silly, but simple "UTC+400" is different*/
  ASSERT (get_date (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  /* Check that some "next Monday", "last Wednesday", etc. are correct.  */
  setenv ("TZ", "UTC0", 1);
  for (i = 0; day_table[i]; i++)
    {
      unsigned int thur2 = 7 * 24 * 3600; /* 2nd thursday */
      char tmp[32];
      sprintf (tmp, "NEXT %s", day_table[i]);
      now.tv_sec = thur2 + 4711;
      now.tv_nsec = 1267;
      ASSERT (get_date (&result, tmp, &now));
      LOG (tmp, now, result);
      ASSERT (result.tv_nsec == 0);
      ASSERT (result.tv_sec == thur2 + (i == 4 ? 7 : (i + 3) % 7) * 24 * 3600);

      sprintf (tmp, "LAST %s", day_table[i]);
      now.tv_sec = thur2 + 4711;
      now.tv_nsec = 1267;
      ASSERT (get_date (&result, tmp, &now));
      LOG (tmp, now, result);
      ASSERT (result.tv_nsec == 0);
      ASSERT (result.tv_sec == thur2 + ((i + 3) % 7 - 7) * 24 * 3600);
    }

  p = "THURSDAY UTC+00";  /* The epoch was on Thursday.  */
  now.tv_sec = 0;
  now.tv_nsec = 0;
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (result.tv_sec == now.tv_sec
          && result.tv_nsec == now.tv_nsec);

  p = "FRIDAY UTC+00";
  now.tv_sec = 0;
  now.tv_nsec = 0;
  ASSERT (get_date (&result, p, &now));
  LOG (p, now, result);
  ASSERT (result.tv_sec == 24 * 3600
          && result.tv_nsec == now.tv_nsec);

  return 0;
}
