/* Replacement <selinux/label.h> for platforms that lack it.
   Copyright 2020-2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#ifndef SELINUX_LABEL_H
#define SELINUX_LABEL_H

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE, _GL_UNNAMED.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <selinux/selinux.h>
#include <errno.h>

_GL_INLINE_HEADER_BEGIN
#ifndef SE_LABEL_INLINE
# define SE_LABEL_INLINE _GL_INLINE
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

#define SELABEL_CTX_FILE 0

struct selabel_handle;

SE_LABEL_INLINE int
selabel_lookup (struct selabel_handle *_GL_UNNAMED (hnd),
                char **_GL_UNNAMED (context),
                char const *_GL_UNNAMED (key),
                int _GL_UNNAMED (type))
{ errno = ENOTSUP; return -1; }

SE_LABEL_INLINE int
selabel_lookup_raw (struct selabel_handle *_GL_UNNAMED (hnd),
                    char **_GL_UNNAMED (context),
                    char const *_GL_UNNAMED (key),
                    int _GL_UNNAMED (type))
{ errno = ENOTSUP; return -1; }

SE_LABEL_INLINE struct selabel_handle *
selabel_open (int _GL_UNNAMED (backend),
              struct selinux_opt *_GL_UNNAMED (options),
              unsigned _GL_UNNAMED (nopt))
{ errno = ENOTSUP; return 0; }

SE_LABEL_INLINE void
selabel_close (struct selabel_handle *_GL_UNNAMED (hnd))
{ errno = ENOTSUP; }

_GL_INLINE_HEADER_END

#endif
