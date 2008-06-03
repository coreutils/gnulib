/* Return the number of entries in an ACL.

   Copyright (C) 2002-2003, 2005-2008 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Written by Paul Eggert and Andreas Gruenbacher.  */

#include <config.h>

#include "acl-internal.h"

/* This file assumes POSIX-draft like ACLs
   (Linux, FreeBSD, MacOS X, IRIX, Tru64).  */

/* Return the number of entries in ACL.  */

int
acl_entries (acl_t acl)
{
  int count = 0;

  if (acl != NULL)
    {
#if HAVE_ACL_FIRST_ENTRY /* Linux, FreeBSD, MacOS X */
      acl_entry_t ace;
      int at_end;

      for (at_end = acl_get_entry (acl, ACL_FIRST_ENTRY, &ace);
	   !at_end;
	   at_end = acl_get_entry (acl, ACL_NEXT_ENTRY, &ace))
	count++;
#else /* IRIX, Tru64 */
# if HAVE_ACL_TO_SHORT_TEXT /* IRIX */
      /* Don't use acl_get_entry: it is undocumented.  */
      count = acl->acl_cnt;
# endif
# if HAVE_ACL_FREE_TEXT /* Tru64 */
      /* Don't use acl_get_entry: it takes only one argument and does not
	 work.  */
      count = acl->acl_num;
# endif
#endif
    }

  return count;
}
