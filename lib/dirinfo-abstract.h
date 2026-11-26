/* Abstract interface of directory information per file descriptor.
   Copyright (C) 2006-2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible.  */

#ifndef _DIRINFO_ABSTRACT_H
#define _DIRINFO_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif


/* Close FD, cleaning up any fd to name mapping if fd was visiting a
   directory.  */
extern void _gl_unregister_fd (int fd);

/* Mark NEWFD as a duplicate of OLDFD; useful from dup, dup2, dup3,
   and fcntl.  Both arguments must be valid and distinct file
   descriptors.  Close NEWFD and return -1 if OLDFD is tracking a
   directory, but there is insufficient memory to track the same
   directory in NEWFD; otherwise return NEWFD.  */
extern int _gl_register_dup (int oldfd, int newfd);

/* If FD is currently visiting a directory, then return the name of
   that directory.  Otherwise, return NULL and set errno.  */
extern const char *_gl_directory_name (int fd);


/* An implementation of these functions.  */
struct dirinfo_vtable_t
{
  void (*unregister_fd) (int fd);
  int (*register_dup) (int oldfd, int newfd);
  const char * (*directory_name) (int fd);
};

/* Set the implementation of these functions.
   Use a null VTABLE to withdraw the previously set implementation.  */
extern void _gl_set_dirinfo_impl (const struct dirinfo_vtable_t *vtable);


#ifdef __cplusplus
}
#endif

#endif /* _DIRINFO_ABSTRACT_H */
