/* Substitute for and wrapper around <malloc.h>.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

#ifndef _@GUARD_PREFIX@_MALLOC_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_MALLOC_H@
# @INCLUDE_NEXT@ @NEXT_MALLOC_H@
#endif

#ifndef _@GUARD_PREFIX@_MALLOC_H
#define _@GUARD_PREFIX@_MALLOC_H

/* Solaris declares memalign() in <stdlib.h>, not in <malloc.h>.
   Also get size_t and free().  */
#include <stdlib.h>

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Declare overridden functions.  */

#if @GNULIB_MEMALIGN@
# if @REPLACE_MEMALIGN@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef memalign
#   define memalign rpl_memalign
#  endif
_GL_FUNCDECL_RPL (memalign, void *,
                  (size_t alignment, size_t size)
                  _GL_ATTRIBUTE_DEALLOC_FREE);
_GL_CXXALIAS_RPL (memalign, void *, (size_t alignment, size_t size));
# else
#  if @HAVE_MEMALIGN@
#   if __GNUC__ >= 11
/* For -Wmismatched-dealloc: Associate memalign with free or rpl_free.  */
_GL_FUNCDECL_SYS (memalign, void *,
                  (size_t alignment, size_t size)
                  _GL_ATTRIBUTE_DEALLOC_FREE);
#   endif
_GL_CXXALIAS_SYS (memalign, void *, (size_t alignment, size_t size));
#  endif
# endif
# if @HAVE_MEMALIGN@
_GL_CXXALIASWARN (memalign);
# endif
#else
# if __GNUC__ >= 11 && !defined memalign
/* For -Wmismatched-dealloc: Associate memalign with free or rpl_free.  */
_GL_FUNCDECL_SYS (memalign, void *,
                  (size_t alignment, size_t size)
                  _GL_ATTRIBUTE_DEALLOC_FREE);
# endif
# if defined GNULIB_POSIXCHECK
#  undef memalign
#  if HAVE_RAW_DECL_MEMALIGN
_GL_WARN_ON_USE (memalign, "memalign is not portable - "
                 "use gnulib module memalign for portability");
#  endif
# endif
#endif


#endif /* _@GUARD_PREFIX@_MALLOC_H */
#endif /* _@GUARD_PREFIX@_MALLOC_H */
