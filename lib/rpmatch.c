/* Determine whether string value is affirmation or negative response
   according to current locale's data.

   Copyright (C) 1996, 1998, 2000, 2002, 2003, 2006, 2008 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stddef.h>
#include <stdlib.h>

#if ENABLE_NLS
# include <sys/types.h>
# include <limits.h>
# include <regex.h>
# include "gettext.h"
# define _(msgid) gettext (msgid)

static int
try (const char *response, const char *pattern, const int match,
     const int nomatch, const char **lastp, regex_t *re)
{
  if (pattern != *lastp)
    {
      /* The pattern has changed.  */
      if (*lastp)
	{
	  /* Free the old compiled pattern.  */
	  regfree (re);
	  *lastp = NULL;
	}
      /* Compile the pattern and cache it for future runs.  */
      if (regcomp (re, pattern, REG_EXTENDED) != 0)
	return -1;
      *lastp = pattern;
    }

  /* See if the regular expression matches RESPONSE.  */
  return regexec (re, response, 0, NULL, 0) == 0 ? match : nomatch;
}
#endif


int
rpmatch (const char *response)
{
#if ENABLE_NLS
  /* Match against one of the response patterns, compiling the pattern
     first if necessary.  */

  /* We get the patterns from a PO file.  It would be possible to use
     nl_langinfo (YESEXPR) instead of _("^[yY]"), and nl_langinfo (NOEXPR)
     instead of _("^[nN]"), if we could assume that the system's locale
     support is good.  But this is not the case e.g. on Cygwin.  The
     localizations of gnulib.pot are of better quality in general.
     Also, if we used locale info from non-free systems that don't have a
     'localedef' command, we would deprive the users of the freedom to
     localize this pattern for their preferred language.  */

  /* We cache the response patterns and compiled regexps here.  */
  static const char *yesexpr, *noexpr;
  static regex_t yesre, nore;
  int result;

  return ((result = try (response, _("^[yY]"), 1, 0,
			 &yesexpr, &yesre))
	  ? result
	  : try (response, _("^[nN]"), 0, -1, &noexpr, &nore));
#else
  /* Test against "^[yY]" and "^[nN]", hardcoded to avoid requiring regex */
  return (*response == 'y' || *response == 'Y' ? 1
	  : *response == 'n' || *response == 'N' ? 0 : -1);
#endif
}
