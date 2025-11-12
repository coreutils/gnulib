/* Determination whether a file is local or remote.
   Copyright (C) 2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _FILE_REMOTE_H
#define _FILE_REMOTE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Determines whether a file (or directory) is remote, that is, whether
   a file system operation on the file, such as mkdir() or open(), can
   take an unlimited amount of time, due to network issues.
   Returns
     - 0, if it is local,
     - 1, if it is remote,
     - -1 with errno set, upon error (e.g. if the file is not accessible).  */
extern int file_is_remote (const char *file);

#ifdef __cplusplus
}
#endif

#endif /* _FILE_REMOTE_H */
