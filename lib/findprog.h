/* Locating a program in PATH.
   Copyright (C) 2001-2003, 2009-2019 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */


#ifdef __cplusplus
extern "C" {
#endif


/* Looks up a program in the PATH.
   Attempts to determine the pathname that would be called by execlp/execvp
   of PROGNAME.  If successful, it returns a pathname containing a slash
   (either absolute or relative to the current directory).  Otherwise, it
   returns PROGNAME unmodified.
   Because of the latter case, callers should use execlp/execvp, not
   execl/execv on the returned pathname.
   The returned string is freshly malloc()ed if it is != PROGNAME.  */
extern const char *find_in_path (const char *progname);

/* Looks up a program in the given PATH-like string.
   The PATH argument consists of a list of directories, separated by ':' or
   (on native Windows) by ';'.  An empty PATH element designates the current
   directory.  A null PATH is equivalent to an empty PATH, that is, to the
   singleton list that contains only the current directory.
   Determines the pathname that would be called by execlp/execvp of PROGNAME.
   - If successful, it returns a pathname containing a slash (either absolute
     or relative to the current directory).  The returned string can be used
     with either execl/execv or execlp/execvp.  It is freshly malloc()ed if it
     is != PROGNAME.
   - Otherwise, it returns NULL.  */
extern const char *find_in_given_path (const char *progname, const char *path);


#ifdef __cplusplus
}
#endif
