/* kwset.h - header declaring the keyword set library.
   Copyright (C) 1989, 1998, 2005, 2007, 2009-2025 Free Software Foundation,
   Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written August 1989 by Mike Haertel.  */

#ifndef _KWSET_H
#define _KWSET_H

/* This file uses _GL_ATTRIBUTE_PURE.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stddef.h>

#include "arg-nonnull.h"
#include "idx.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kwsmatch
{
  idx_t index;  /* Index number of matching keyword.  */
  idx_t offset; /* Offset of match.  */
  idx_t size;   /* Length of match.  */
};

struct kwset;
typedef struct kwset *kwset_t;

extern kwset_t kwsalloc (char const *);
extern void kwsincr (kwset_t, char const *, idx_t);
extern idx_t kwswords (kwset_t) _GL_ATTRIBUTE_PURE;
extern void kwsprep (kwset_t);
extern ptrdiff_t kwsexec (kwset_t, char const *, idx_t,
                          struct kwsmatch *, bool)
  _GL_ARG_NONNULL ((4));
extern void kwsfree (kwset_t);

#ifdef __cplusplus
}
#endif

#endif /* _KWSET_H */
