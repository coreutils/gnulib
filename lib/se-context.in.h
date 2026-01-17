/* SELinux-related headers.
   Copyright (C) 2007-2026 Free Software Foundation, Inc.

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

/* Written by Jim Meyering, 2007.  */

#ifndef SELINUX_CONTEXT_H
#define SELINUX_CONTEXT_H

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE, _GL_UNNAMED.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <errno.h>

_GL_INLINE_HEADER_BEGIN
#ifndef SE_CONTEXT_INLINE
# define SE_CONTEXT_INLINE _GL_INLINE
#endif

/* _GL_UNNAMED (ID) is the "name" of an unnamed function parameter.  */
#ifndef _GL_UNNAMED
# if ((defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) < 202311 \
      && !defined __cplusplus)
#  define _GL_UNNAMED(id) unnamed_##id _GL_ATTRIBUTE_UNUSED
# else
#  define _GL_UNNAMED(id)
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct selinux_context_t *context_t;
SE_CONTEXT_INLINE context_t
context_new (char const *_GL_UNNAMED (s))
  { errno = ENOTSUP; return (context_t) 0; }
SE_CONTEXT_INLINE char *
context_str (context_t _GL_UNNAMED (con))
  { errno = ENOTSUP; return (char *) 0; }
SE_CONTEXT_INLINE void context_free (context_t _GL_UNNAMED (c)) {}

SE_CONTEXT_INLINE int
context_user_set (context_t _GL_UNNAMED (sc), char const *_GL_UNNAMED (s))
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int
context_role_set (context_t _GL_UNNAMED (sc), char const *_GL_UNNAMED (s))
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int
context_range_set (context_t _GL_UNNAMED (sc), char const *_GL_UNNAMED (s))
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int
context_type_set (context_t _GL_UNNAMED (sc), char const *_GL_UNNAMED (s))
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE char *
context_type_get (context_t _GL_UNNAMED (sc))
  { errno = ENOTSUP; return (char *) 0; }
SE_CONTEXT_INLINE char *
context_range_get (context_t _GL_UNNAMED (sc))
  { errno = ENOTSUP; return (char *) 0; }
SE_CONTEXT_INLINE char *
context_role_get (context_t _GL_UNNAMED (sc))
  { errno = ENOTSUP; return (char *) 0; }
SE_CONTEXT_INLINE char *
context_user_get (context_t _GL_UNNAMED (sc))
  { errno = ENOTSUP; return (char *) 0; }


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif
