/* A GNU-like <stdlib.h>.

   Copyright (C) 1995, 2001-2002, 2006-2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#if defined __need_malloc_and_calloc
/* Special invocation convention inside glibc header files.  */
#include @ABSOLUTE_STDLIB_H@
#else
/* Normal invocation convention.  */
#ifndef _GL_STDLIB_H
#define _GL_STDLIB_H

#include @ABSOLUTE_STDLIB_H@


/* The definition of GL_LINK_WARNING is copied here.  */


/* Some systems do not define EXIT_*, despite otherwise supporting C89.  */
#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif


#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_MKDTEMP@
# if !@HAVE_MKDTEMP@
/* Create a unique temporary directory from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the directory name unique.
   Returns TEMPLATE, or a null pointer if it cannot get a unique name.
   The directory is created mode 700.  */
extern char * mkdtemp (char *template);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mkdtemp
# define mkdtemp(t) \
    (GL_LINK_WARNING ("mkdtemp is unportable - "\
                      "use gnulib module mkdtemp for portability"), \
     mkdtemp (t))
#endif

#if @GNULIB_MKSTEMP@
# if @REPLACE_MKSTEMP@
/* Create a unique temporary file from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the file name unique.
   The file is then created, ensuring it didn't exist before.
   The file is created read-write (mask at least 0600 & ~umask), but it may be
   world-readable and world-writable (mask 0666 & ~umask), depending on the
   implementation.
   Returns the open file descriptor if successful, otherwise -1 and errno
   set.  */
#  define mkstemp rpl_mkstemp
extern int mkstemp (char *template);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mkstemp
# define mkstemp(t) \
    (GL_LINK_WARNING ("mkstemp is unportable - "\
                      "use gnulib module mkstemp for portability"), \
     mkstemp (t))
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_STDLIB_H */
#endif
