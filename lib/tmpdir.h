/* Determine a temporary directory.
   Copyright (C) 2001-2002, 2009-2025 Free Software Foundation, Inc.

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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Path search algorithm, for tmpnam, tmpfile, etc.  If DIR is
   non-null and exists, uses it; otherwise uses the first of $TMPDIR,
   P_tmpdir, /tmp that exists.  Copies into TMPL a template suitable
   for use with mk[s]temp.  Will fail (-1) if DIR is non-null and
   doesn't exist, none of the searched dirs exists, or there's not
   enough space in TMPL. */
extern int path_search (char *tmpl, size_t tmpl_len, const char *dir, const char *pfx, bool try_tmpdir);


#ifdef __cplusplus
}
#endif
