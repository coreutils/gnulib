/* Determine whether we can write any file.

   Copyright (C) 2007 Free Software Foundation, Inc.

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
   Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Paul Eggert.  */

#include <config.h>

#include "write-any-file.h"

#if HAVE_PRIV_H
# include <priv.h>
#endif
#include <unistd.h>

/* Return true if we know that we can write any file, including
   writing directories.  */

bool
can_write_any_file (void)
{
  static bool initialized;
  static bool can;

  if (! initialized)
    {
#if defined PRIV_EFFECTIVE && defined PRIV_FILE_DAC_WRITE
      priv_set_t *pset = priv_allocset ();
      if (pset)
	{
	  can =
	    (getppriv (PRIV_EFFECTIVE, pset) == 0
	     && priv_ismember (pset, PRIV_FILE_DAC_WRITE));
	  priv_freeset (pset);
	}
      else
#else
      /* In traditional Unix, only root can unlink directories.  */
      can = (geteuid () == 0);
#endif
      initialized = true;
    }

  return can;
}
