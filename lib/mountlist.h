/* mountlist.h -- declarations for list of mounted file systems

   Copyright (C) 1991-1992, 1998, 2000-2005, 2009-2025 Free Software
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

#ifndef MOUNTLIST_H_
#define MOUNTLIST_H_

/* This file uses _GL_ATTRIBUTE_MALLOC.  */
#if !_GL_CONFIG_H_INCLUDED
# error "Please include config.h first."
#endif

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


/* A mount table entry. */
struct mount_entry
{
  char *me_devname;             /* Device node name, including "/dev/". */
  char *me_mountdir;            /* Mount point directory name. */
  char *me_mntroot;             /* Directory on filesystem of device used */
                                /* as root for the (bind) mount. */
  char *me_type;                /* "nfs", "4.2", etc. */
  dev_t me_dev;                 /* Device number of me_mountdir. */
  unsigned int me_dummy : 1;    /* Nonzero for dummy file systems. */
  unsigned int me_remote : 1;   /* Nonzero for remote file systems. */
  unsigned int me_type_malloced : 1; /* Nonzero if me_type was malloced. */
  struct mount_entry *me_next;
};

/* Return a list of the currently mounted file systems, or NULL on error.
   Add each entry to the tail of the list so that they stay in order.
   If NEED_FS_TYPE is true, ensure that the file system type fields in
   the returned list are valid.  Otherwise, they might not be.  */
struct mount_entry *read_file_system_list (bool need_fs_type)
  _GL_ATTRIBUTE_MALLOC;

void free_mount_entry (struct mount_entry *entry);


#ifdef __cplusplus
}
#endif

#endif
