/* Test whether a file has a nontrivial access control list.

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

#include "acl.h"

#include "acl-internal.h"


#if USE_ACL && HAVE_ACL_GET_FILE

# if HAVE_ACL_TYPE_EXTENDED /* MacOS X */

/* ACL is an ACL, from a file, stored as type ACL_TYPE_EXTENDED.
   Return 1 if the given ACL is non-trivial.
   Return 0 if it is trivial.  */
int
acl_extended_nontrivial (acl_t acl)
{
  /* acl is non-trivial if it is non-empty.  */
  return (acl_entries (acl) > 0);
}

# else /* Linux, FreeBSD, IRIX, Tru64 */

/* ACL is an ACL, from a file, stored as type ACL_TYPE_ACCESS.
   Return 1 if the given ACL is non-trivial.
   Return 0 if it is trivial, i.e. equivalent to a simple stat() mode.
   Return -1 and set errno upon failure to determine it.  */
int
acl_access_nontrivial (acl_t acl)
{
  /* acl is non-trivial if it has some entries other than for "user::",
     "group::", and "other::".  Normally these three should be present
     at least, allowing us to write
	return (3 < acl_entries (acl));
     but the following code is more robust.  */
#  if HAVE_ACL_FIRST_ENTRY /* Linux, FreeBSD */

  acl_entry_t ace;
  int at_end;

  for (at_end = acl_get_entry (acl, ACL_FIRST_ENTRY, &ace);
       !at_end;
       at_end = acl_get_entry (acl, ACL_NEXT_ENTRY, &ace))
    {
      acl_tag_t tag;
      if (acl_get_tag_type (ace, &tag) < 0)
	return -1;
      if (!(tag == ACL_USER_OBJ || tag == ACL_GROUP_OBJ || tag == ACL_OTHER))
	return 1;
    }
  return 0;

#  else /* IRIX, Tru64 */
#   if HAVE_ACL_TO_SHORT_TEXT /* IRIX */
  /* Don't use acl_get_entry: it is undocumented.  */

  int count = acl->acl_cnt;
  int i;

  for (i = 0; i < count; i++)
    {
      acl_entry_t ace = &acl->acl_entry[i];
      acl_tag_t tag = ace->ae_tag;

      if (!(tag == ACL_USER_OBJ || tag == ACL_GROUP_OBJ
	    || tag == ACL_OTHER_OBJ))
	return 1;
    }
  return 0;

#   endif
#   if HAVE_ACL_FREE_TEXT /* Tru64 */
  /* Don't use acl_get_entry: it takes only one argument and does not work.  */

  int count = acl->acl_num;
  acl_entry_t ace;

  for (ace = acl->acl_first; count > 0; ace = ace->next, count--)
    {
      acl_tag_t tag;
      acl_perm_t perm;

      tag = ace->entry->acl_type;
      if (!(tag == ACL_USER_OBJ || tag == ACL_GROUP_OBJ || tag == ACL_OTHER))
	return 1;

      perm = ace->entry->acl_perm;
      /* On Tru64, perm can also contain non-standard bits such as
	 PERM_INSERT, PERM_DELETE, PERM_MODIFY, PERM_LOOKUP, ... */
      if ((perm & ~(ACL_READ | ACL_WRITE | ACL_EXECUTE)) != 0)
	return 1;
    }
  return 0;

#   endif
#  endif
}

# endif

#endif


/* Return 1 if NAME has a nontrivial access control list, 0 if NAME
   only has no or a base access control list, and -1 (setting errno)
   on error.  SB must be set to the stat buffer of FILE.  */

int
file_has_acl (char const *name, struct stat const *sb)
{
#if USE_ACL
  if (! S_ISLNK (sb->st_mode))
    {
# if HAVE_ACL_GET_FILE

      /* POSIX 1003.1e (draft 17 -- abandoned) specific version.  */
      /* Linux, FreeBSD, MacOS X, IRIX, Tru64 */
      int ret;

      if (HAVE_ACL_EXTENDED_FILE) /* Linux */
	{
	  /* On Linux, acl_extended_file is an optimized function: It only
	     makes two calls to getxattr(), one for ACL_TYPE_ACCESS, one for
	     ACL_TYPE_DEFAULT.  */
	  ret = acl_extended_file (name);
	}
      else /* FreeBSD, MacOS X, IRIX, Tru64 */
	{
#  if HAVE_ACL_TYPE_EXTENDED /* MacOS X */
	  /* On MacOS X, acl_get_file (name, ACL_TYPE_ACCESS)
	     and acl_get_file (name, ACL_TYPE_DEFAULT)
	     always return NULL / EINVAL.  There is no point in making
	     these two useless calls.  The real ACL is retrieved through
	     acl_get_file (name, ACL_TYPE_EXTENDED).  */
	  acl_t acl = acl_get_file (name, ACL_TYPE_EXTENDED);
	  if (acl)
	    {
	      ret = acl_extended_nontrivial (acl);
	      acl_free (acl);
	    }
	  else
	    ret = -1;
#  else /* FreeBSD, IRIX, Tru64 */
	  acl_t acl = acl_get_file (name, ACL_TYPE_ACCESS);
	  if (acl)
	    {
	      int saved_errno;

	      ret = acl_access_nontrivial (acl);
	      saved_errno = errno;
	      acl_free (acl);
	      errno = saved_errno;
#   if HAVE_ACL_FREE_TEXT /* Tru64 */
	      /* On OSF/1, acl_get_file (name, ACL_TYPE_DEFAULT) always
		 returns NULL with errno not set.  There is no point in
		 making this call.  */
#   else /* FreeBSD, IRIX */
	      /* On Linux, FreeBSD, IRIX, acl_get_file (name, ACL_TYPE_ACCESS)
		 and acl_get_file (name, ACL_TYPE_DEFAULT) on a directory
		 either both succeed or both fail; it depends on the
		 filesystem.  Therefore there is no point in making the second
		 call if the first one already failed.  */
	      if (ret == 0 && S_ISDIR (sb->st_mode))
		{
		  acl = acl_get_file (name, ACL_TYPE_DEFAULT);
		  if (acl)
		    {
		      ret = (0 < acl_entries (acl));
		      acl_free (acl);
		    }
		  else
		    ret = -1;
		}
#   endif
	    }
	  else
	    ret = -1;
#  endif
	}
      if (ret < 0)
	return ACL_NOT_WELL_SUPPORTED (errno) ? 0 : -1;
      return ret;

# elif HAVE_ACL && defined GETACLCNT /* Solaris, Cygwin, not HP-UX */

#  if HAVE_ACL_TRIVIAL

      /* Solaris 10, which also has NFSv4 and ZFS style ACLs.  */
      return acl_trivial (name);

#  else /* Solaris, Cygwin, general case */

      /* Solaris 2.5 through Solaris 10, Cygwin, and contemporaneous versions
	 of Unixware.  The acl() call returns the access and default ACL both
	 at once.  */
      int n = acl (name, GETACLCNT, 0, NULL);
      return n < 0 ? (errno == ENOSYS ? 0 : -1) : (MIN_ACL_ENTRIES < n);

#  endif

# endif
    }
#endif

  /* FIXME: Add support for AIX.  Please see Samba's
     source/lib/sysacls.c file for fix-related ideas.  */

  return 0;
}
