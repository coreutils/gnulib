/* Case-insensitive searching in a string.
   Copyright (C) 2005-2007 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2005.

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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include <ctype.h>
#include <stddef.h>  /* for NULL, in case a nonstandard string.h lacks it */

#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

/* Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive
   comparison.
   Note: This function may, in multibyte locales, return success even if
   strlen (haystack) < strlen (needle) !  */
char *
strcasestr (const char *haystack, const char *needle)
{
  if (*needle != '\0')
    {
      /* Speed up the following searches of needle by caching its first
	 character.  */
      unsigned char b = TOLOWER ((unsigned char) *needle);

      needle++;
      for (;; haystack++)
	{
	  if (*haystack == '\0')
	    /* No match.  */
	    return NULL;
	  if (TOLOWER ((unsigned char) *haystack) == b)
	    /* The first character matches.  */
	    {
	      const char *rhaystack = haystack + 1;
	      const char *rneedle = needle;

	      for (;; rhaystack++, rneedle++)
		{
		  if (*rneedle == '\0')
		    /* Found a match.  */
		    return (char *) haystack;
		  if (*rhaystack == '\0')
		    /* No match.  */
		    return NULL;
		  if (TOLOWER ((unsigned char) *rhaystack)
		      != TOLOWER ((unsigned char) *rneedle))
		    /* Nothing in this round.  */
		    break;
		}
	    }
	}
    }
  else
    return (char *) haystack;
}
