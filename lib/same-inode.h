/* Determine whether two stat buffers refer to the same file.

   Copyright (C) 2006, 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SAME_INODE_H
# define SAME_INODE_H 1

/* Perform a tri-state query on whether STAT_BUF_1 and STAT_BUF_2
   represent the same file.  Return 1 for equal, 0 for distinct, and
   -1 for indeterminate (the latter is generally possible only on
   mingw).  Algorithms that use this macro must be prepared to handle
   the indeterminate case without wrong results.  For example, if an
   optimization is possible if two files are the same but unsafe if
   distinct, use SAME_INODE()==1; whereas for an optimization that is
   possible only for distinct files, use !SAME_INODE().  */

# define SAME_INODE(Stat_buf_1, Stat_buf_2) \
   (((Stat_buf_1).st_ino == (Stat_buf_2).st_ino         \
     && (Stat_buf_1).st_dev == (Stat_buf_2).st_dev)     \
    ? 1 - 2 * !(Stat_buf_1).st_ino : 0)

#endif
