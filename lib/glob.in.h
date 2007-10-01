/* glob.h -- Find a path matching a pattern.

   Copyright (C) 2005-2007 Free Software Foundation, Inc.

   Written by Derek Price <derek@ximbiot.com> & Paul Eggert <eggert@CS.UCLA.EDU>

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

#ifndef _GL_GLOB_H
#define _GL_GLOB_H

#if @HAVE_SYS_CDEFS_H@
# include <sys/cdefs.h>
#endif

#include <stddef.h>

/* On some systems, such as AIX 5.1, <sys/stat.h> does a "#define stat stat64".
   Make sure this definition is seen before glob-libc.h defines types that
   rely on 'struct stat'.  */
#include <sys/stat.h>

#ifndef __BEGIN_DECLS
# define __BEGIN_DECLS
# define __END_DECLS
#endif
#ifndef __THROW
# define __THROW
#endif

#ifndef __size_t
# define __size_t	size_t
#endif
#ifndef __USE_GNU
# define __USE_GNU    1
#endif


#define glob rpl_glob
#define globfree rpl_globfree
#define glob_pattern_p rpl_glob_pattern_p

#define __GLOB_GNULIB 1

/* Now the standard GNU C Library header should work.  */
#include "glob-libc.h"

#endif /* _GL_GLOB_H */
