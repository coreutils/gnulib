/* Ordered map data type implemented by a binary tree.
   Copyright (C) 2006, 2009-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2018.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _GL_RBTREE_OMAP_H
#define _GL_RBTREE_OMAP_H

#include "gl_omap.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct gl_omap_implementation gl_rbtree_omap_implementation;
#define GL_RBTREE_OMAP &gl_rbtree_omap_implementation

#ifdef __cplusplus
}
#endif

#endif /* _GL_RBTREE_OMAP_H */
