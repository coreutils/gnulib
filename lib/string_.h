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
#if ! @HAVE_DECL_MEMMEM@
extern void *memmem (void const *__haystack, size_t __haystack_len,
		     void const *__needle, size_t __needle_len);
#endif

/* Copy N bytes of SRC to DEST, return pointer to bytes after the
   last written byte.  */
#if ! @HAVE_MEMPCPY@
extern void *mempcpy (void *restrict __dest, void const *restrict __src,
		      size_t __n);
#endif

/* Search backwards through a block for a byte (specified as an int).  */
#if ! @HAVE_DECL_MEMRCHR@
extern void *memrchr (void const *, int, size_t);
#endif

/* Copy SRC to DST, returning the address of the terminating '\0' in DST.  */
#if ! @HAVE_STPCPY@
extern char *stpcpy (char *restrict __dst, char const *restrict __src);
#endif

/* Copy no more than N bytes of SRC to DST, returning a pointer past the
   last non-NUL byte written into DST.  */
#if ! @HAVE_STPNCPY@
# define stpncpy gnu_stpncpy
extern char *stpncpy (char *restrict __dst, char const *restrict __src,
		      size_t __n);
#endif

/* Compare strings S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than, equal to or greater
   than S2.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!
   No known system has a strcasecmp() function that works correctly in
   multibyte locales.  Therefore use our version always, if the
   strcase module is available.  */
#if @REPLACE_STRCASECMP@
# define strcasecmp rpl_strcasecmp
extern int strcasecmp (char const *__s1, char const *__s2);
#endif

/* Compare no more than N bytes of strings S1 and S2, ignoring case,
   returning less than, equal to or greater than zero if S1 is
   lexicographically less than, equal to or greater than S2.
   Note: This function cannot work correctly in multibyte locales.  */
#if ! @HAVE_DECL_STRNCASECMP@
extern int strncasecmp (char const *__s1, char const *__s2, size_t __n);
#endif

/* Find the first occurrence of C in S or the final NUL byte.  */
#if ! @HAVE_STRCHRNUL@
extern char *strchrnul (char const *__s, int __c_in);
#endif

/* Duplicate S, returning an identical malloc'd string.  */
#if ! @HAVE_DECL_STRDUP@ && ! defined strdup
extern char *strdup (char const *__s);
#endif

/* Return a newly allocated copy of at most N bytes of STRING.  */
#if ! @HAVE_STRNDUP@
# undef strndup
# define strndup rpl_strndup
# if ! @HAVE_DECL_STRNDUP@
extern char *strndup (char const *__string, size_t __n);
# endif
#endif

/* Find the length (number of bytes) of STRING, but scan at most
   MAXLEN bytes.  If no '\0' terminator is found in that many bytes,
   return MAXLEN.  */
#if ! @HAVE_DECL_STRNLEN@
extern size_t strnlen (char const *__string, size_t __maxlen);
#endif

/* Find the first occurrence in S of any character in ACCEPT.  */
#if ! @HAVE_STRPBRK@
extern char *strpbrk (char const *__s, char const *__accept);
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
#if ! @HAVE_STRSEP@
extern char *strsep (char **restrict __stringp, char const *restrict __delim);
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK.
   No known system has a strstr() function that works correctly in
   multibyte locales.  Therefore use our version always, if the strstr
   module is available.  */
#if @REPLACE_STRSTR@
# undef strstr
# define strstr rpl_strstr
extern char *strstr (char const *__haystack, char const *__needle);
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive
   comparison.
   Note: This function may, in multibyte locales, return success even if
   strlen (haystack) < strlen (needle) !  */
#if @REPLACE_STRCASESTR@
# undef strcasestr
# define strcasestr rpl_strcasestr
extern char *strcasestr (const char *haystack, const char *needle);
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
#if ! @HAVE_DECL_STRTOK_R@
extern char *strtok_r (char *restrict __s, char const *restrict __sep,
		       char **restrict __lasts);
#endif

#ifdef __cplusplus
}
#endif

#endif
