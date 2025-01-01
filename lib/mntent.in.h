/* Provide a more complete mntent.h.

   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

#ifndef _@GUARD_PREFIX@_MNTENT_H

/* The include_next requires a split double-inclusion guard.  */
#@INCLUDE_NEXT@ @NEXT_MNTENT_H@

#ifndef _@GUARD_PREFIX@_MNTENT_H
#define _@GUARD_PREFIX@_MNTENT_H

/* This file uses GNULIB_POSIXCHECK, HAVE_RAW_DECL_*.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_ARG_NONNULL is copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */

#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_HASMNTOPT@ && @HAVE_SETMNTENT@
/* Searches ME->mnt_opts for an option matching OPTION.
   Returns the address of the substring, or null if none found.  */
# if @REPLACE_HASMNTOPT@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef hasmntopt
#   define hasmntopt rpl_hasmntopt
#  endif
_GL_FUNCDECL_RPL (hasmntopt, char *,
                  (const struct mntent *me, const char *option),
                  _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (hasmntopt, char *,
                  (const struct mntent *me, const char *option));
# else
#  if !@HAVE_HASMNTOPT@
_GL_FUNCDECL_SYS (hasmntopt, char *,
                  (const struct mntent *me, const char *option),
                  _GL_ARG_NONNULL ((1, 2)));
#  endif
_GL_CXXALIAS_SYS (hasmntopt, char *,
                  (const struct mntent *me, const char *option));
# endif
# if __GLIBC__ >= 2
_GL_CXXALIASWARN (hasmntopt);
# endif
#elif defined GNULIB_POSIXCHECK
# undef hasmntopt
# if HAVE_RAW_DECL_HASMNTOPT
_GL_WARN_ON_USE (hasmntopt, "hasmntopt is unportable - "
                 "use gnulib module hasmntopt for portability");
# endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* _@GUARD_PREFIX@_MNTENT_H */
#endif /* _@GUARD_PREFIX@_MNTENT_H */
