/* SELinux-related headers.
   Copyright (C) 2007-2021 Free Software Foundation, Inc.

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

/* Written by Jim Meyering, 2007.  */

#ifndef SELINUX_CONTEXT_H
# define SELINUX_CONTEXT_H

# include <errno.h>

#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
#endif
_GL_INLINE_HEADER_BEGIN
#ifndef SE_CONTEXT_INLINE
# define SE_CONTEXT_INLINE _GL_INLINE
#endif

/* The definition of _GL_UNUSED_PARAMETER is copied here.  */

typedef int context_t;
SE_CONTEXT_INLINE context_t context_new (char const *s _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return 0; }
SE_CONTEXT_INLINE char *context_str (context_t con _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return (void *) 0; }
SE_CONTEXT_INLINE void context_free (context_t c _GL_UNUSED_PARAMETER) {}

SE_CONTEXT_INLINE int context_user_set (context_t sc _GL_UNUSED_PARAMETER,
                                        char const *s _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int context_role_set (context_t sc _GL_UNUSED_PARAMETER,
                                        char const *s _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int context_range_set (context_t sc _GL_UNUSED_PARAMETER,
                                         char const *s _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE int context_type_set (context_t sc _GL_UNUSED_PARAMETER,
                                        char const *s _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return -1; }
SE_CONTEXT_INLINE char *context_type_get (context_t sc _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return (void *) 0; }
SE_CONTEXT_INLINE char *context_range_get (context_t sc _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return (void *) 0; }
SE_CONTEXT_INLINE char *context_role_get (context_t sc _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return (void *) 0; }
SE_CONTEXT_INLINE char *context_user_get (context_t sc _GL_UNUSED_PARAMETER)
  { errno = ENOTSUP; return (void *) 0; }

_GL_INLINE_HEADER_END

#endif
