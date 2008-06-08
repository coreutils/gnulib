/* copy-acl.c - copy access control list from one file to another file

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


/* Copy access control lists from one file to another. If SOURCE_DESC is
   a valid file descriptor, use file descriptor operations, else use
   filename based operations on SRC_NAME. Likewise for DEST_DESC and
   DST_NAME.
   If access control lists are not available, fchmod the target file to
   MODE.  Also sets the non-permission bits of the destination file
   (S_ISUID, S_ISGID, S_ISVTX) to those from MODE if any are set.
   Return 0 if successful.
   Return -2 and set errno for an error relating to the source file.
   Return -1 and set errno for an error relating to the destination file.  */

static int
qcopy_acl (const char *src_name, int source_desc, const char *dst_name,
	   int dest_desc, mode_t mode)
{
#if USE_ACL && HAVE_ACL_GET_FILE
  /* POSIX 1003.1e (draft 17 -- abandoned) specific version.  */
  /* Linux, FreeBSD, MacOS X, IRIX, Tru64 */
# if MODE_INSIDE_ACL
  /* Linux, FreeBSD, IRIX, Tru64 */

  acl_t acl;
  int ret;

  if (HAVE_ACL_GET_FD && source_desc != -1)
    acl = acl_get_fd (source_desc);
  else
    acl = acl_get_file (src_name, ACL_TYPE_ACCESS);
  if (acl == NULL)
    {
      if (ACL_NOT_WELL_SUPPORTED (errno))
	return qset_acl (dst_name, dest_desc, mode);
      else
        return -2;
    }

  if (HAVE_ACL_SET_FD && dest_desc != -1)
    ret = acl_set_fd (dest_desc, acl);
  else
    ret = acl_set_file (dst_name, ACL_TYPE_ACCESS, acl);
  if (ret != 0)
    {
      int saved_errno = errno;

      if (ACL_NOT_WELL_SUPPORTED (errno) && !acl_access_nontrivial (acl))
        {
	  acl_free (acl);
	  return chmod_or_fchmod (dst_name, dest_desc, mode);
	}
      else
	{
	  acl_free (acl);
	  chmod_or_fchmod (dst_name, dest_desc, mode);
	  errno = saved_errno;
	  return -1;
	}
    }
  else
    acl_free (acl);

  if (mode & (S_ISUID | S_ISGID | S_ISVTX))
    {
      /* We did not call chmod so far, and either the mode and the ACL are
	 separate or special bits are to be set which don't fit into ACLs.  */

      if (chmod_or_fchmod (dst_name, dest_desc, mode) != 0)
	return -1;
    }

  if (S_ISDIR (mode))
    {
      acl = acl_get_file (src_name, ACL_TYPE_DEFAULT);
      if (acl == NULL)
	return -2;

      if (acl_set_file (dst_name, ACL_TYPE_DEFAULT, acl))
	{
	  int saved_errno = errno;

	  acl_free (acl);
	  errno = saved_errno;
	  return -1;
	}
      else
        acl_free (acl);
    }
  return 0;

# else /* !MODE_INSIDE_ACL */
  /* MacOS X */

#  if !HAVE_ACL_TYPE_EXTENDED
#   error Must have ACL_TYPE_EXTENDED
#  endif

  /* On MacOS X,  acl_get_file (name, ACL_TYPE_ACCESS)
     and          acl_get_file (name, ACL_TYPE_DEFAULT)
     always return NULL / EINVAL.  You have to use
		  acl_get_file (name, ACL_TYPE_EXTENDED)
     or           acl_get_fd (open (name, ...))
     to retrieve an ACL.
     On the other hand,
		  acl_set_file (name, ACL_TYPE_ACCESS, acl)
     and          acl_set_file (name, ACL_TYPE_DEFAULT, acl)
     have the same effect as
		  acl_set_file (name, ACL_TYPE_EXTENDED, acl):
     Each of these calls sets the file's ACL.  */

  acl_t acl;
  int ret;

  if (HAVE_ACL_GET_FD && source_desc != -1)
    acl = acl_get_fd (source_desc);
  else
    acl = acl_get_file (src_name, ACL_TYPE_EXTENDED);
  if (acl == NULL)
    {
      if (ACL_NOT_WELL_SUPPORTED (errno))
	return qset_acl (dst_name, dest_desc, mode);
      else
        return -2;
    }

  if (HAVE_ACL_SET_FD && dest_desc != -1)
    ret = acl_set_fd (dest_desc, acl);
  else
    ret = acl_set_file (dst_name, ACL_TYPE_EXTENDED, acl);
  if (ret != 0)
    {
      int saved_errno = errno;

      if (ACL_NOT_WELL_SUPPORTED (errno) && !(acl_entries (acl) > 0))
        {
	  acl_free (acl);
	  return chmod_or_fchmod (dst_name, dest_desc, mode);
	}
      else
	{
	  acl_free (acl);
	  chmod_or_fchmod (dst_name, dest_desc, mode);
	  errno = saved_errno;
	  return -1;
	}
    }
  else
    acl_free (acl);

  /* Since !MODE_INSIDE_ACL, we have to call chmod explicitly.  */
  return chmod_or_fchmod (dst_name, dest_desc, mode);

# endif

#elif USE_ACL && defined ACL_NO_TRIVIAL
  /* Solaris 10 NFSv4 ACLs.  */

  int ret;
  acl_t *aclp = NULL;
  ret = (source_desc < 0
	 ? acl_get (src_name, ACL_NO_TRIVIAL, &aclp)
	 : facl_get (source_desc, ACL_NO_TRIVIAL, &aclp));
  if (ret != 0 && errno != ENOSYS)
    return -2;

  ret = qset_acl (dst_name, dest_desc, mode);
  if (ret != 0)
    return -1;

  if (aclp)
    {
      ret = (dest_desc < 0
	     ? acl_set (dst_name, aclp)
	     : facl_set (dest_desc, aclp));
      if (ret != 0)
	{
	  int saved_errno = errno;

	  acl_free (aclp);
	  errno = saved_errno;
	  return -1;
	}
      acl_free (aclp);
    }

  return 0;

#else

  return qset_acl (dst_name, dest_desc, mode);

#endif
}


/* Copy access control lists from one file to another. If SOURCE_DESC is
   a valid file descriptor, use file descriptor operations, else use
   filename based operations on SRC_NAME. Likewise for DEST_DESC and
   DST_NAME.
   If access control lists are not available, fchmod the target file to
   MODE.  Also sets the non-permission bits of the destination file
   (S_ISUID, S_ISGID, S_ISVTX) to those from MODE if any are set.
   Return 0 if successful, otherwise output a diagnostic and return -1.  */

int
copy_acl (const char *src_name, int source_desc, const char *dst_name,
	  int dest_desc, mode_t mode)
{
  int ret = qcopy_acl (src_name, source_desc, dst_name, dest_desc, mode);
  switch (ret)
    {
    case -2:
      error (0, errno, "%s", quote (src_name));
      return -1;

    case -1:
      error (0, errno, _("preserving permissions for %s"), quote (dst_name));
      return -1;

    default:
      return 0;
    }
}
