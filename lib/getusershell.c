/* getusershell.c -- Return names of valid user shells.

   Copyright (C) 1991, 1997, 2000-2001, 2003-2006, 2008-2025 Free Software
   Foundation, Inc.

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

/* Written by David MacKenzie <djm@gnu.ai.mit.edu> */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#ifndef SHELLS_FILE
# ifndef __DJGPP__
/* File containing a list of nonrestricted shells, one per line. */
#  define SHELLS_FILE "/etc/shells"
# else
/* This is a horrible kludge.  Isn't there a better way?  */
#  define SHELLS_FILE "/dev/env/DJDIR/etc/shells"
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "filename.h"
#include "xalloc.h"

#if GNULIB_FOPEN_SAFER
# include "stdio--.h"
#endif

#if ! defined ADDITIONAL_DEFAULT_SHELLS && defined __MSDOS__
# define ADDITIONAL_DEFAULT_SHELLS \
  "c:/dos/command.com", "c:/windows/command.com", "c:/command.com",
#else
# define ADDITIONAL_DEFAULT_SHELLS /* empty */
#endif

/* List of shells to use if the shells file is missing. */
static char const* const default_shells[] =
{
  ADDITIONAL_DEFAULT_SHELLS
  "/bin/sh", "/bin/csh", "/usr/bin/sh", "/usr/bin/csh", NULL
};

/* Index of the next shell in 'default_shells' to return.
   0 means we are not using 'default_shells'. */
static size_t default_index = 0;

/* Input stream from the shells file. */
static FILE *shellstream = NULL;

/* Line of input from the shells file. */
static char *line = NULL;

/* Number of bytes allocated for 'line'. */
static size_t line_size = 0;

/* Return an entry from the shells file, ignoring comment lines.
   If the file doesn't exist, use the list in DEFAULT_SHELLS (above).
   In any case, the returned string is in memory allocated through malloc.
   Return NULL if there are no more entries.  */

char *
getusershell (void)
{
  if (default_index > 0)
    {
      if (default_shells[default_index])
        /* Not at the end of the list yet.  */
        return xstrdup (default_shells[default_index++]);
      return NULL;
    }

  if (shellstream == NULL)
    {
      shellstream = fopen (SHELLS_FILE, "r");
      if (shellstream == NULL)
        {
          /* No shells file.  Use the default list.  */
          default_index = 1;
          return xstrdup (default_shells[0]);
        }
    }

  for (;;)
    {
      ssize_t nread = getline (&line, &line_size, shellstream);

      /* End of file.  */
      if (nread == -1)
        return NULL;
      /* Skip empty lines. */
      else if (nread > 1)
        {
          char *start = line;
          char *comment = strchr (start, '#');
          char *end;

          if (comment != NULL)
            {
              /* Trim the comment mark.  */
              *comment = '\0';
              end = comment;
            }
          else
            {
              /* Trim the newline.  */
              end = start + nread;
              if (end[-1] == '\n')
                *--end = '\0';
            }

          /* Skip leading whitespace.  */
          while (start < end && isspace ((unsigned char) start[0]))
            start++;
          /* Trim trailing whitespace.  */
          while (start < end && isspace ((unsigned char) end[-1]))
            *--end = '\0';

          /* Only return absolute file names. */
          if (start < end && IS_ABSOLUTE_FILE_NAME (start))
            return start;
        }
    }
}

/* Rewind the shells file. */

void
setusershell (void)
{
  default_index = 0;
  if (shellstream)
    rewind (shellstream);
}

/* Close the shells file. */

void
endusershell (void)
{
  if (shellstream)
    {
      fclose (shellstream);
      shellstream = NULL;
    }
}

#ifdef TEST
int
main (void)
{
  char *s;

  while (s = getusershell ())
    puts (s);
  exit (0);
}
#endif
