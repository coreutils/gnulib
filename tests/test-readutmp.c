/* Test of readutmp module.
   Copyright (C) 2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#if HAVE_UTMPX_H || HAVE_UTMP_H

# include "readutmp.h"

# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# include "xalloc.h"

# define ELEMENT STRUCT_UTMP
# define COMPARE(entry1, entry2) \
   _GL_CMP (UT_TIME_MEMBER (entry1), UT_TIME_MEMBER (entry2))
# define STATIC static
# include "array-mergesort.h"

# include "macros.h"

int
main (int argc, char *argv[])
{
  STRUCT_UTMP *entries;
  size_t num_entries;

  if (read_utmp (UTMP_FILE, &num_entries, &entries, 0) < 0)
    {
      fprintf (stderr, "Skipping test: cannot open %s\n", UTMP_FILE);
      return 77;
    }

  printf ("    Time (GMT)           User          PID     Term Exit Boot User Process\n");
  printf ("------------------- --------------- ---------- ---- ---- ---- ------------\n");

  if (num_entries > 0)
    {
      /* Sort the entries according to increasing UT_TIME_MEMBER (entry).
         Use a stable sort algorithm.  */
      merge_sort_inplace (entries, num_entries,
                          XNMALLOC (num_entries, STRUCT_UTMP));

      size_t i;
      for (i = 0; i < num_entries; i++)
        {
          const STRUCT_UTMP *entry = &entries[i];

          char *user = extract_trimmed_name (entry);
          long pid = UT_PID (entry);
          int termination = UT_EXIT_E_TERMINATION (entry);
          int exit = UT_EXIT_E_EXIT (entry);

          time_t tim = UT_TIME_MEMBER (entry);
          struct tm *gmt = gmtime (&tim);
          char timbuf[100];
          if (gmt == NULL
              || strftime (timbuf, sizeof (timbuf), "%Y-%m-%d %H:%M:%S", gmt)
                 == 0)
            strcpy (timbuf, "---");

          printf ("%s %-15s %10ld %3d  %3d   %c       %c\n",
                  timbuf,
                  user,
                  pid,
                  termination,
                  exit,
                  UT_TYPE_BOOT_TIME (entry) ? 'X' : ' ',
                  UT_TYPE_USER_PROCESS (entry) ? 'X' : ' ');
        }

      /* If the first time is more than 5 years in the past or the last time
         is more than a week in the future, the time_t members are wrong.  */
      time_t first = UT_TIME_MEMBER (&entries[0]);
      time_t last = UT_TIME_MEMBER (&entries[num_entries - 1]);
      time_t now = time (NULL);
      ASSERT (first >= now - 157680000);
      ASSERT (last <= now + 604800);
    }

  return 0;
}

#else

# include <stdio.h>

int
main ()
{
  fprintf (stderr, "Skipping test: neither <utmpx.h> nor <utmp.h> is available\n");
  return 77;
}

#endif
