/* Invoke dirent-like functions, but avoid some glitches.

   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

/* Written by Eric Blake.  */

/* This file uses _GL_ATTRIBUTE_DEALLOC.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

DIR *opendir_safer (const char *name)
  _GL_ATTRIBUTE_DEALLOC (closedir, 1);

#ifdef __cplusplus
}
#endif
