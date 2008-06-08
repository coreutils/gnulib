/* set-mode-acl.c - set access control list equivalent to a mode

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

/* If DESC is a valid file descriptor use fchmod to change the
   file's mode to MODE on systems that have fchown. On systems
   that don't have fchown and if DESC is invalid, use chown on
   NAME instead.
   Return 0 if successful.  Return -1 and set errno upon failure.  */

int
chmod_or_fchmod (const char *name, int desc, mode_t mode)
{
  if (HAVE_FCHMOD && desc != -1)
    return fchmod (desc, mode);
  else
    return chmod (name, mode);
}

/* Set the access control lists of a file. If DESC is a valid file
   descriptor, use file descriptor operations where available, else use
   filename based operations on NAME.  If access control lists are not
   available, fchmod the target file to MODE.  Also sets the
   non-permission bits of the destination file (S_ISUID, S_ISGID, S_ISVTX)
   to those from MODE if any are set.
   Return 0 if successful.  Return -1 and set errno upon failure.  */

int
qset_acl (char const *name, int desc, mode_t mode)
{
#if USE_ACL
# if HAVE_ACL_GET_FILE
  /* POSIX 1003.1e draft 17 (abandoned) specific version.  */
  /* Linux, FreeBSD, MacOS X, IRIX, Tru64 */
#  if MODE_INSIDE_ACL
  /* Linux, FreeBSD, IRIX, Tru64 */

  /* We must also have acl_from_text and acl_delete_def_file.
     (acl_delete_def_file could be emulated with acl_init followed
      by acl_set_file, but acl_set_file with an empty acl is
      unspecified.)  */

#   ifndef HAVE_ACL_FROM_TEXT
#    error Must have acl_from_text (see POSIX 1003.1e draft 17).
#   endif
#   ifndef HAVE_ACL_DELETE_DEF_FILE
#    error Must have acl_delete_def_file (see POSIX 1003.1e draft 17).
#   endif

  acl_t acl;
  int ret;

  if (HAVE_ACL_FROM_MODE) /* Linux */
    {
      acl = acl_from_mode (mode);
      if (!acl)
	return -1;
    }
  else /* FreeBSD, IRIX, Tru64 */
    {
      /* If we were to create the ACL using the functions acl_init(),
	 acl_create_entry(), acl_set_tag_type(), acl_set_qualifier(),
	 acl_get_permset(), acl_clear_perm[s](), acl_add_perm(), we
	 would need to create a qualifier.  I don't know how to do this.
	 So create it using acl_from_text().  */

#   if HAVE_ACL_FREE_TEXT /* Tru64 */
      char acl_text[] = "u::---,g::---,o::---,";
#   else /* FreeBSD, IRIX */
      char acl_text[] = "u::---,g::---,o::---";
#   endif

      if (mode & S_IRUSR) acl_text[ 3] = 'r';
      if (mode & S_IWUSR) acl_text[ 4] = 'w';
      if (mode & S_IXUSR) acl_text[ 5] = 'x';
      if (mode & S_IRGRP) acl_text[10] = 'r';
      if (mode & S_IWGRP) acl_text[11] = 'w';
      if (mode & S_IXGRP) acl_text[12] = 'x';
      if (mode & S_IROTH) acl_text[17] = 'r';
      if (mode & S_IWOTH) acl_text[18] = 'w';
      if (mode & S_IXOTH) acl_text[19] = 'x';

      acl = acl_from_text (acl_text);
      if (!acl)
	return -1;
    }
  if (HAVE_ACL_SET_FD && desc != -1)
    ret = acl_set_fd (desc, acl);
  else
    ret = acl_set_file (name, ACL_TYPE_ACCESS, acl);
  if (ret != 0)
    {
      int saved_errno = errno;
      acl_free (acl);

      if (ACL_NOT_WELL_SUPPORTED (errno))
	return chmod_or_fchmod (name, desc, mode);
      else
	{
	  errno = saved_errno;
	  return -1;
	}
    }
  else
    acl_free (acl);

  if (S_ISDIR (mode) && acl_delete_def_file (name))
    return -1;

  if (mode & (S_ISUID | S_ISGID | S_ISVTX))
    {
      /* We did not call chmod so far, so the special bits have not yet
         been set.  */
      return chmod_or_fchmod (name, desc, mode);
    }
  return 0;

#  else /* !MODE_INSIDE_ACL */
  /* MacOS X */

#   if !HAVE_ACL_TYPE_EXTENDED
#    error Must have ACL_TYPE_EXTENDED
#   endif

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

  /* Remove the ACL if the file has ACLs.  */
  if (HAVE_ACL_GET_FD && desc != -1)
    acl = acl_get_fd (desc);
  else
    acl = acl_get_file (name, ACL_TYPE_EXTENDED);
  if (acl)
    {
      acl_free (acl);

      acl = acl_init (0);
      if (acl)
	{
	  if (HAVE_ACL_SET_FD && desc != -1)
	    ret = acl_set_fd (desc, acl);
	  else
	    ret = acl_set_file (name, ACL_TYPE_EXTENDED, acl);
	  if (ret != 0)
	    {
	      int saved_errno = errno;

	      acl_free (acl);

	      if (ACL_NOT_WELL_SUPPORTED (saved_errno))
		return chmod_or_fchmod (name, desc, mode);
	      else
		{
		  errno = saved_errno;
		  return -1;
		}
	    }
	  acl_free (acl);
	}
    }

  /* Since !MODE_INSIDE_ACL, we have to call chmod explicitly.  */
  return chmod_or_fchmod (name, desc, mode);
#  endif

# elif defined ACL_NO_TRIVIAL
  /* Solaris 10, with NFSv4 ACLs.  */
  acl_t *aclp;
  char acl_text[] = "user::---,group::---,mask:---,other:---";

  if (mode & S_IRUSR) acl_text[ 6] = 'r';
  if (mode & S_IWUSR) acl_text[ 7] = 'w';
  if (mode & S_IXUSR) acl_text[ 8] = 'x';
  if (mode & S_IRGRP) acl_text[17] = acl_text[26] = 'r';
  if (mode & S_IWGRP) acl_text[18] = acl_text[27] = 'w';
  if (mode & S_IXGRP) acl_text[19] = acl_text[28] = 'x';
  if (mode & S_IROTH) acl_text[36] = 'r';
  if (mode & S_IWOTH) acl_text[37] = 'w';
  if (mode & S_IXOTH) acl_text[38] = 'x';

  if (acl_fromtext (acl_text, &aclp) != 0)
    {
      errno = ENOMEM;
      return -1;
    }
  else
    {
      int ret = (desc < 0 ? acl_set (name, aclp) : facl_set (desc, aclp));
      int saved_errno = errno;
      acl_free (aclp);
      if (ret == 0 || saved_errno != ENOSYS)
	{
	  errno = saved_errno;
	  return ret;
	}
    }

  return chmod_or_fchmod (name, desc, mode);

# else /* Unknown flavor of ACLs */
  return chmod_or_fchmod (name, desc, mode);
# endif
#else /* !USE_ACL */
  return chmod_or_fchmod (name, desc, mode);
#endif
}

/* As with qset_acl, but also output a diagnostic on failure.  */

int
set_acl (char const *name, int desc, mode_t mode)
{
  int r = qset_acl (name, desc, mode);
  if (r != 0)
    error (0, errno, _("setting permissions for %s"), quote (name));
  return r;
}
