/* relpath - print the relative path
   Copyright (C) 2012 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Pádraig Brady.  */

#ifndef _RELPATH_H
# define _RELPATH_H

/* Output the relative representation if possible.
   If BUF is non NULL, write to that buffer rather than to stdout.
   Return true iff success.  */

bool
relpath (const char *can_fname, const char *can_reldir, char *buf, size_t len);

/* Return FROM represented as relative to the dir of TARGET.  The
   result is malloced, or NULL upon error (described by errno).  */

char *
convert_abs_rel (const char *from, const char *target);

#endif
