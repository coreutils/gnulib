/* Substitute for and wrapper around <malloc.h>.
   Copyright (C) 2020-2023 Free Software Foundation, Inc.

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

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#if defined _GL_ALREADY_INCLUDING_MALLOC_H
/* Special invocation convention:
   - On Android we have a sequence of nested includes
       <malloc.h> -> <stdio.h> -> <sys/stat.h> -> <time.h> -> <sys/time.h> ->
       <sys/select.h> -> <signal.h> -> <pthread.h> -> <stdlib.h> -> "malloc.h"
     In this situation, in C++ mode, the declaration of memalign might be used
     before it actually occurs.  */

#@INCLUDE_NEXT_AS_FIRST_DIRECTIVE@ @NEXT_AS_FIRST_DIRECTIVE_MALLOC_H@

#else
/* Normal invocation convention.  */

#ifndef _@GUARD_PREFIX@_MALLOC_H

#define _GL_ALREADY_INCLUDING_MALLOC_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_MALLOC_H@
# @INCLUDE_NEXT_AS_FIRST_DIRECTIVE@ @NEXT_AS_FIRST_DIRECTIVE_MALLOC_H@
#endif

#undef _GL_ALREADY_INCLUDING_MALLOC_H

#ifndef _@GUARD_PREFIX@_MALLOC_H
#define _@GUARD_PREFIX@_MALLOC_H

/* Solaris declares memalign() in <stdlib.h>, not in <malloc.h>.  */
#if defined __sun || defined __hpux
# include <stdlib.h>
#endif

/* Get size_t.  */
#include <stddef.h>

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Make _GL_ATTRIBUTE_DEALLOC_FREE work, even though <stdlib.h> may not have
   been included yet.  */
#if @GNULIB_FREE_POSIX@
# if (@REPLACE_FREE@ && !defined free \
      && !(defined __cplusplus && defined GNULIB_NAMESPACE))
/* We can't do '#define free rpl_free' here.  */
#  if defined __cplusplus && (__GLIBC__ + (__GLIBC_MINOR__ >= 14) > 2)
_GL_EXTERN_C void rpl_free (void *) throw ();
#  else
_GL_EXTERN_C void rpl_free (void *);
#  endif
#  undef _GL_ATTRIBUTE_DEALLOC_FREE
#  define _GL_ATTRIBUTE_DEALLOC_FREE _GL_ATTRIBUTE_DEALLOC (rpl_free, 1)
# else
#  if defined _MSC_VER && !defined free
_GL_EXTERN_C
#   if defined _DLL
     __declspec (dllimport)
#   endif
     void __cdecl free (void *);
#  else
#   if defined __cplusplus && (__GLIBC__ + (__GLIBC_MINOR__ >= 14) > 2)
_GL_EXTERN_C void free (void *) throw ();
#   else
_GL_EXTERN_C void free (void *);
#   endif
#  endif
# endif
#else
# if defined _MSC_VER && !defined free
_GL_EXTERN_C
#   if defined _DLL
     __declspec (dllimport)
#   endif
     void __cdecl free (void *);
# else
#  if defined __cplusplus && (__GLIBC__ + (__GLIBC_MINOR__ >= 14) > 2)
_GL_EXTERN_C void free (void *) throw ();
#  else
_GL_EXTERN_C void free (void *);
#  endif
# endif
#endif


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
#endif
