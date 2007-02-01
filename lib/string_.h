/* A GNU-like <string.h>.

   Copyright (C) 1995-1996, 2001-2007 Free Software Foundation, Inc.

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

#ifndef _GL_STRING_H
#define _GL_STRING_H

#include @ABSOLUTE_STRING_H@

#ifdef __cplusplus
extern "C" {
#endif

/* Return the first occurrence of NEEDLE in HAYSTACK.  */
#if @GNULIB_MEMMEM@
# if ! @HAVE_DECL_MEMMEM@
extern void *memmem (void const *__haystack, size_t __haystack_len,
		     void const *__needle, size_t __needle_len);
# endif
#else
# undef memmem
# define memmem memmem_is_unportable__use_gnulib_module_memmem_for_portability
#endif

/* Copy N bytes of SRC to DEST, return pointer to bytes after the
   last written byte.  */
#if @GNULIB_MEMPCPY@
# if ! @HAVE_MEMPCPY@
extern void *mempcpy (void *restrict __dest, void const *restrict __src,
		      size_t __n);
# endif
#else
# undef mempcpy
# define mempcpy mempcpy_is_unportable__use_gnulib_module_mempcpy_for_portability
#endif

/* Search backwards through a block for a byte (specified as an int).  */
#if @GNULIB_MEMRCHR@
# if ! @HAVE_DECL_MEMRCHR@
extern void *memrchr (void const *, int, size_t);
# endif
#else
# undef memrchr
# define memrchr memrchr_is_unportable__use_gnulib_module_memrchr_for_portability
#endif

/* Copy SRC to DST, returning the address of the terminating '\0' in DST.  */
#if @GNULIB_STPCPY@
# if ! @HAVE_STPCPY@
extern char *stpcpy (char *restrict __dst, char const *restrict __src);
# endif
#else
# undef stpcpy
# define stpcpy stpcpy_is_unportable__use_gnulib_module_stpcpy_for_portability
#endif

/* Copy no more than N bytes of SRC to DST, returning a pointer past the
   last non-NUL byte written into DST.  */
#if @GNULIB_STPNCPY@
# if ! @HAVE_STPNCPY@
#  define stpncpy gnu_stpncpy
extern char *stpncpy (char *restrict __dst, char const *restrict __src,
		      size_t __n);
# endif
#else
# undef stpncpy
# define stpncpy stpncpy_is_unportable__use_gnulib_module_stpncpy_for_portability
#endif

/* Compare strings S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than, equal to or greater
   than S2.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!
   No known system has a strcasecmp() function that works correctly in
   multibyte locales.  Therefore use our version always, if the
   strcase module is available.  */
#if @GNULIB_STRCASE@
# if @REPLACE_STRCASECMP@
#  define strcasecmp rpl_strcasecmp
extern int strcasecmp (char const *__s1, char const *__s2);
# endif
#else
# undef strcasecmp
# define strcasecmp strcasecmp_is_unportable__use_gnulib_module_strcase_for_portability
#endif

/* Compare no more than N bytes of strings S1 and S2, ignoring case,
   returning less than, equal to or greater than zero if S1 is
   lexicographically less than, equal to or greater than S2.
   Note: This function cannot work correctly in multibyte locales.  */
#if @GNULIB_STRCASE@
# if ! @HAVE_DECL_STRNCASECMP@
extern int strncasecmp (char const *__s1, char const *__s2, size_t __n);
# endif
#else
# undef strncasecmp
# define strncasecmp strncasecmp_is_unportable__use_gnulib_module_strcase_for_portability
#endif

/* Find the first occurrence of C in S or the final NUL byte.  */
#if @GNULIB_STRCHRNUL@
# if ! @HAVE_STRCHRNUL@
extern char *strchrnul (char const *__s, int __c_in);
# endif
#else
# undef strchrnul
# define strchrnul strchrnul_is_unportable__use_gnulib_module_strchrnul_for_portability
#endif

/* Duplicate S, returning an identical malloc'd string.  */
#if @GNULIB_STRDUP@
# if ! @HAVE_DECL_STRDUP@ && ! defined strdup
extern char *strdup (char const *__s);
# endif
#else
# undef strdup
# define strdup strdup_is_unportable__use_gnulib_module_strdup_for_portability
#endif

/* Return a newly allocated copy of at most N bytes of STRING.  */
#if @GNULIB_STRNDUP@
# if ! @HAVE_STRNDUP@
#  undef strndup
#  define strndup rpl_strndup
#  if ! @HAVE_DECL_STRNDUP@
extern char *strndup (char const *__string, size_t __n);
#  endif
# endif
#else
# undef strndup
# define strndup strndup_is_unportable__use_gnulib_module_strndup_for_portability
#endif

/* Find the length (number of bytes) of STRING, but scan at most
   MAXLEN bytes.  If no '\0' terminator is found in that many bytes,
   return MAXLEN.  */
#if @GNULIB_STRNLEN@
# if ! @HAVE_DECL_STRNLEN@
extern size_t strnlen (char const *__string, size_t __maxlen);
# endif
#else
# undef strnlen
# define strnlen strnlen_is_unportable__use_gnulib_module_strnlen_for_portability
#endif

/* Find the first occurrence in S of any character in ACCEPT.  */
#if @GNULIB_STRPBRK@
# if ! @HAVE_STRPBRK@
extern char *strpbrk (char const *__s, char const *__accept);
# endif
#else
# undef strpbrk
# define strpbrk strpbrk_is_unportable__use_gnulib_module_strpbrk_for_portability
#endif

/* Search the next delimiter (char listed in DELIM) starting at *STRINGP.
   If one is found, overwrite it with a NUL, and advance *STRINGP
   to point to the next char after it.  Otherwise, set *STRINGP to NULL.
   If *STRINGP was already NULL, nothing happens.
   Return the old value of *STRINGP.

   This is a variant of strtok() that is multithread-safe and supports
   empty fields.

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.
   Caveat: It doesn't work with multibyte strings unless all of the delimiter
           characters are ASCII characters < 0x30.

   See also strtok_r().  */
#if @GNULIB_STRSEP@
# if ! @HAVE_STRSEP@
extern char *strsep (char **restrict __stringp, char const *restrict __delim);
# endif
#else
# undef strsep
# define strsep strsep_is_unportable__use_gnulib_module_strsep_for_portability
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK.
   No known system has a strstr() function that works correctly in
   multibyte locales.  Therefore use our version always, if the strstr
   module is available.  */
#if @GNULIB_STRSTR@
# if @REPLACE_STRSTR@
#  undef strstr
#  define strstr rpl_strstr
extern char *strstr (char const *__haystack, char const *__needle);
# endif
#else
# undef strstr
# define strstr strstr_is_unportable__use_gnulib_module_strstr_for_portability
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive
   comparison.
   Note: This function may, in multibyte locales, return success even if
   strlen (haystack) < strlen (needle) !  */
#if @GNULIB_STRCASESTR@
# if @REPLACE_STRCASESTR@
#  undef strcasestr
#  define strcasestr rpl_strcasestr
extern char *strcasestr (const char *haystack, const char *needle);
# endif
#else
# undef strcasestr
# define strcasestr strcasestr_is_unportable__use_gnulib_module_strcasestr_for_portability
#endif

/* Parse S into tokens separated by characters in DELIM.
   If S is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
	char s[] = "-abc-=-def";
	char *sp;
	x = strtok_r(s, "-", &sp);	// x = "abc", sp = "=-def"
	x = strtok_r(NULL, "-=", &sp);	// x = "def", sp = NULL
	x = strtok_r(NULL, "=", &sp);	// x = NULL
		// s = "abc\0-def\0"

   This is a variant of strtok() that is multithread-safe.

   For the POSIX documentation for this function, see:
   http://www.opengroup.org/susv3xsh/strtok.html

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.
   Caveat: It doesn't work with multibyte strings unless all of the delimiter
           characters are ASCII characters < 0x30.

   See also strsep().  */
#if @GNULIB_STRTOK_R@
# if ! @HAVE_DECL_STRTOK_R@
extern char *strtok_r (char *restrict __s, char const *restrict __sep,
		       char **restrict __lasts);
# endif
#else
# undef strtok_r
# define strtok_r strtok_r_is_unportable__use_gnulib_module_strtok_r_for_portability
#endif

#ifdef __cplusplus
}
#endif

#endif
