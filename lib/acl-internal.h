/* Internal implementation of access control lists.

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

#include "acl.h"

#include <stdbool.h>
#include <stdlib.h>

/* All systems define the ACL related API in <sys/acl.h>.  */
#if HAVE_SYS_ACL_H
# include <sys/acl.h>
#endif
#if defined HAVE_ACL && ! defined GETACLCNT && defined ACL_CNT
# define GETACLCNT ACL_CNT
#endif

/* On Linux, additional ACL related API is available in <acl/libacl.h>.  */
#ifdef HAVE_ACL_LIBACL_H
# include <acl/libacl.h>
#endif

#include "error.h"
#include "quote.h"

#include <errno.h>
#ifndef ENOSYS
# define ENOSYS (-1)
#endif
#ifndef ENOTSUP
# define ENOTSUP (-1)
#endif

#include "gettext.h"
#define _(msgid) gettext (msgid)

#ifndef HAVE_FCHMOD
# define HAVE_FCHMOD false
# define fchmod(fd, mode) (-1)
#endif

#ifndef MIN_ACL_ENTRIES
# define MIN_ACL_ENTRIES 4
#endif

/* POSIX 1003.1e (draft 17) */
#ifdef HAVE_ACL_GET_FD
/* Most platforms have a 1-argument acl_get_fd, only OSF/1 has a 2-argument
   macro(!).  */
# if HAVE_ACL_FREE_TEXT /* OSF/1 */
static inline acl_t
rpl_acl_get_fd (int fd)
{
  return acl_get_fd (fd, ACL_TYPE_ACCESS);
}
#  undef acl_get_fd
#  define acl_get_fd rpl_acl_get_fd
# endif
#else
# define HAVE_ACL_GET_FD false
# undef acl_get_fd
# define acl_get_fd(fd) (NULL)
#endif

/* POSIX 1003.1e (draft 17) */
#ifdef HAVE_ACL_SET_FD
/* Most platforms have a 2-argument acl_set_fd, only OSF/1 has a 3-argument
   macro(!).  */
# if HAVE_ACL_FREE_TEXT /* OSF/1 */
static inline int
rpl_acl_set_fd (int fd, acl_t acl)
{
  return acl_set_fd (fd, ACL_TYPE_ACCESS, acl);
}
#  undef acl_set_fd
#  define acl_set_fd rpl_acl_set_fd
# endif
#else
# define HAVE_ACL_SET_FD false
# undef acl_set_fd
# define acl_set_fd(fd, acl) (-1)
#endif

/* POSIX 1003.1e (draft 13) */
#if ! HAVE_ACL_FREE_TEXT
# define acl_free_text(buf) acl_free (buf)
#endif

/* Linux-specific */
#ifndef HAVE_ACL_EXTENDED_FILE
# define HAVE_ACL_EXTENDED_FILE false
# define acl_extended_file(name) (-1)
#endif

/* Linux-specific */
#ifndef HAVE_ACL_FROM_MODE
# define HAVE_ACL_FROM_MODE false
# define acl_from_mode(mode) (NULL)
#endif

/* Set to 1 if a file's mode is implicit by the ACL.
   Set to 0 if a file's mode is stored independently from the ACL.  */
#if HAVE_ACL_COPY_EXT_NATIVE && HAVE_ACL_CREATE_ENTRY_NP /* MacOS X */
# define MODE_INSIDE_ACL 0
#else
# define MODE_INSIDE_ACL 1
#endif

#if defined __APPLE__ && defined __MACH__ /* MacOS X */
# define ACL_NOT_WELL_SUPPORTED(Err) \
   ((Err) == ENOTSUP || (Err) == ENOSYS || (Err) == EINVAL || (Err) == EBUSY || (Err) == ENOENT)
#else
# define ACL_NOT_WELL_SUPPORTED(Err) \
   ((Err) == ENOTSUP || (Err) == ENOSYS || (Err) == EINVAL || (Err) == EBUSY)
#endif

/* Define a replacement for acl_entries if needed.  */
#if USE_ACL && HAVE_ACL_GET_FILE && HAVE_ACL_FREE && !HAVE_ACL_ENTRIES
# define acl_entries rpl_acl_entries
int acl_entries (acl_t);
#endif
