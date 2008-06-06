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
  int ret;

#if USE_ACL && HAVE_ACL_GET_FILE
  /* POSIX 1003.1e (draft 17 -- abandoned) specific version.  */
  /* Linux, FreeBSD, MacOS X, IRIX, Tru64 */

  acl_t acl;
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

      if (ACL_NOT_WELL_SUPPORTED (errno))
        {
	  int nontrivial = acl_access_nontrivial (acl);

	  acl_free (acl);

	  if (!nontrivial)
	    {
	      if (chmod_or_fchmod (dst_name, dest_desc, mode) != 0)
		saved_errno = errno;
	      else
		return 0;
	    }
	  else
	    chmod_or_fchmod (dst_name, dest_desc, mode);
	}
      else
	{
	  acl_free (acl);
	  chmod_or_fchmod (dst_name, dest_desc, mode);
	}
      errno = saved_errno;
      return -1;
    }
  else
    acl_free (acl);

  if (!MODE_INSIDE_ACL || (mode & (S_ISUID | S_ISGID | S_ISVTX)))
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

#elif USE_ACL && defined ACL_NO_TRIVIAL
  /* Solaris 10 NFSv4 ACLs.  */

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

  ret = qset_acl (dst_name, dest_desc, mode);
  return ret;

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
