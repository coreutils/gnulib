/* Replacement <selinux/selinux.h> for platforms that lack it.
   Copyright (C) 2008-2026 Free Software Foundation, Inc.

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

#if @USE_SELINUX_SELINUX_H@

#@INCLUDE_NEXT@ @NEXT_SELINUX_SELINUX_H@

#else
# if !defined _@GUARD_PREFIX@_SELINUX_SELINUX_H
#  define _@GUARD_PREFIX@_SELINUX_SELINUX_H

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE, _GL_UNNAMED.  */
#  if !_GL_CONFIG_H_INCLUDED
#   error "Please include config.h first."
#  endif

#  include <sys/types.h> /* for mode_t */
#  include <errno.h>

_GL_INLINE_HEADER_BEGIN
#  ifndef SE_SELINUX_INLINE
#   define SE_SELINUX_INLINE _GL_INLINE
#  endif

/* _GL_UNNAMED (ID) is the "name" of an unnamed function parameter.  */
#  ifndef _GL_UNNAMED
#   if ((defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) < 202311 \
        && !defined __cplusplus)
#    define _GL_UNNAMED(id) unnamed_##id _GL_ATTRIBUTE_UNUSED
#   else
#    define _GL_UNNAMED(id)
#   endif
#  endif

#  if !GNULIB_defined_security_types

typedef unsigned short security_class_t;
struct selinux_opt;
#   define is_selinux_enabled() 0

SE_SELINUX_INLINE int
getcon (char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
getcon_raw (char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE void
freecon (char *_GL_UNNAMED (con)) {}

SE_SELINUX_INLINE int
getfscreatecon (char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
getfscreatecon_raw (char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setfscreatecon (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setfscreatecon_raw (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
matchpathcon (char const *_GL_UNNAMED (file), mode_t _GL_UNNAMED (m),
              char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
getfilecon (char const *_GL_UNNAMED (file), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
getfilecon_raw (char const *_GL_UNNAMED (file), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
lgetfilecon (char const *_GL_UNNAMED (file), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
lgetfilecon_raw (char const *_GL_UNNAMED (file), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
fgetfilecon (int _GL_UNNAMED (fd), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
fgetfilecon_raw (int _GL_UNNAMED (fd), char **_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setfilecon (char const *_GL_UNNAMED (file), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setfilecon_raw (char const *_GL_UNNAMED (file), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
lsetfilecon (char const *_GL_UNNAMED (file), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
lsetfilecon_raw (char const *_GL_UNNAMED (file), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
fsetfilecon (int _GL_UNNAMED (fd), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
fsetfilecon_raw (int _GL_UNNAMED (fd), char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }

SE_SELINUX_INLINE int
security_check_context (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
security_check_context_raw (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setexeccon (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
setexeccon_raw (char const *_GL_UNNAMED (con))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
security_compute_create (char const *_GL_UNNAMED (scon),
                         char const *_GL_UNNAMED (tcon),
                         security_class_t _GL_UNNAMED (tclass),
                         char **_GL_UNNAMED (newcon))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE int
security_compute_create_raw (char const *_GL_UNNAMED (scon),
                             char const *_GL_UNNAMED (tcon),
                             security_class_t _GL_UNNAMED (tclass),
                             char **_GL_UNNAMED (newcon))
  { errno = ENOTSUP; return -1; }
SE_SELINUX_INLINE security_class_t
string_to_security_class (char const *_GL_UNNAMED (name))
  { errno = ENOTSUP; return 0; }
SE_SELINUX_INLINE int
matchpathcon_init_prefix (char const *_GL_UNNAMED (path),
                          char const *_GL_UNNAMED (prefix))
  { errno = ENOTSUP; return -1; }

#   define GNULIB_defined_security_types 1
#  endif

_GL_INLINE_HEADER_END

# endif /* _@GUARD_PREFIX@_SELINUX_SELINUX_H */
#endif
