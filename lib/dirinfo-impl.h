/* Implementation of directory information per file descriptor.
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

#ifndef _DIRINFO_IMPL_H
#define _DIRINFO_IMPL_H

/* This file uses _GL_ATTRIBUTE_NONNULL.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Mark FD as visiting FILENAME.  FD must be non-negative, and refer
   to an open file descriptor.  If REPLACE_OPEN_DIRECTORY is non-zero,
   this should only be called if FD is visiting a directory.  Close FD
   and return -1 with errno set if there is insufficient memory to track
   the directory name; otherwise return FD.  */
extern int _gl_register_fd (int fd, const char *filename)
                           _GL_ATTRIBUTE_NONNULL ((2));


#ifdef __cplusplus
}
#endif

#endif /* _DIRINFO_IMPL_H */
