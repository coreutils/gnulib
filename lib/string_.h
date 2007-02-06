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


/* GL_LINK_WARNING("literal string") arranges to emit the literal string as
   a linker warning on most glibc systems.
   We use a linker warning rather than a preprocessor warning, because
   #warning cannot be used inside macros.  */
#ifndef GL_LINK_WARNING
  /* This works on platforms with GNU ld and ELF object format.
     Testing __GLIBC__ is sufficient for asserting that GNU ld is in use.
     Testing __ELF__ guarantees the ELF object format.
     Testing __GNUC__ is necessary for the compound expression syntax.  */
# if defined __GLIBC__ && defined __ELF__ && defined __GNUC__
#  define GL_LINK_WARNING(message) \
     GL_LINK_WARNING1 (__FILE__, __LINE__, message)
#  define GL_LINK_WARNING1(file, line, message) \
     GL_LINK_WARNING2 (file, line, message)  /* macroexpand file and line */
#  define GL_LINK_WARNING2(file, line, message) \
     GL_LINK_WARNING3 (file ":" #line ": warning: " message)
#  define GL_LINK_WARNING3(message) \
     ({ static const char warning[sizeof (message)]		\
          __attribute__ ((__unused__,				\
                          __section__ (".gnu.warning"),		\
                          __aligned__ (1)))			\
          = message "\n";					\
        (void)0;						\
     })
# else
#  define GL_LINK_WARNING(message) ((void) 0)
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Return the first occurrence of NEEDLE in HAYSTACK.  */
#if @GNULIB_MEMMEM@
# if ! @HAVE_DECL_MEMMEM@
extern void *memmem (void const *__haystack, size_t __haystack_len,
		     void const *__needle, size_t __needle_len);
# endif
#elif defined GNULIB_POSIXCHECK
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
#elif defined GNULIB_POSIXCHECK
# undef mempcpy
# define mempcpy mempcpy_is_unportable__use_gnulib_module_mempcpy_for_portability
#endif

/* Search backwards through a block for a byte (specified as an int).  */
#if @GNULIB_MEMRCHR@
# if ! @HAVE_DECL_MEMRCHR@
extern void *memrchr (void const *, int, size_t);
# endif
#elif defined GNULIB_POSIXCHECK
# undef memrchr
# define memrchr memrchr_is_unportable__use_gnulib_module_memrchr_for_portability
#endif

/* Copy SRC to DST, returning the address of the terminating '\0' in DST.  */
#if @GNULIB_STPCPY@
# if ! @HAVE_STPCPY@
extern char *stpcpy (char *restrict __dst, char const *restrict __src);
# endif
#elif defined GNULIB_POSIXCHECK
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
#elif defined GNULIB_POSIXCHECK
# undef stpncpy
# define stpncpy stpncpy_is_unportable__use_gnulib_module_stpncpy_for_portability
#endif

/* Compare strings S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than, equal to or greater
   than S2.
   Note: This function does not work in multibyte locales.  */
#if ! @HAVE_STRCASECMP@
extern int strcasecmp (char const *s1, char const *s2);
#endif
#if defined GNULIB_POSIXCHECK
/* strcasecmp() does not work with multibyte strings:
   POSIX says that it operates on "strings", and "string" in POSIX is defined
   as a sequence of bytes, not of characters.   */
# undef strcasecmp
# define strcasecmp(a,b) \
    (GL_LINK_WARNING ("strcasecmp cannot work correctly on character strings in multibyte locales - use mbscasecmp if you care about internationalization, or use c_strcasecmp (from gnulib module c-strcase) if you want a locale independent function"), \
     strcasecmp (a, b))
#endif

/* Compare no more than N bytes of strings S1 and S2, ignoring case,
   returning less than, equal to or greater than zero if S1 is
   lexicographically less than, equal to or greater than S2.
   Note: This function cannot work correctly in multibyte locales.  */
#if ! @HAVE_DECL_STRNCASECMP@
extern int strncasecmp (char const *s1, char const *s2, size_t n);
#endif
#if defined GNULIB_POSIXCHECK
/* strncasecmp() does not work with multibyte strings:
   POSIX says that it operates on "strings", and "string" in POSIX is defined
   as a sequence of bytes, not of characters.  */
# undef strncasecmp
# define strncasecmp(a,b) \
    (GL_LINK_WARNING ("strncasecmp cannot work correctly on character strings in multibyte locales - don't use it if you care about internationalization; use c_strncasecmp (from gnulib module c-strcase) if you want a locale independent function"), \
     strncasecmp (a, b))
#endif

#if defined GNULIB_POSIXCHECK
/* strchr() does not work with multibyte strings if the locale encoding is
   GB18030 and the character to be searched is a digit.  */
# undef strchr
# define strchr(s,c) \
    (GL_LINK_WARNING ("strchr cannot work correctly on character strings in some multibyte locales - use mbschr if you care about internationalization"), \
     strchr (s, c))
#endif

/* Find the first occurrence of C in S or the final NUL byte.  */
#if @GNULIB_STRCHRNUL@
# if ! @HAVE_STRCHRNUL@
extern char *strchrnul (char const *__s, int __c_in);
# endif
#elif defined GNULIB_POSIXCHECK
# undef strchrnul
# define strchrnul strchrnul_is_unportable__use_gnulib_module_strchrnul_for_portability
#endif

/* Duplicate S, returning an identical malloc'd string.  */
#if @GNULIB_STRDUP@
# if ! @HAVE_DECL_STRDUP@ && ! defined strdup
extern char *strdup (char const *__s);
# endif
#elif defined GNULIB_POSIXCHECK
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
#elif defined GNULIB_POSIXCHECK
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
#elif defined GNULIB_POSIXCHECK
# undef strnlen
# define strnlen strnlen_is_unportable__use_gnulib_module_strnlen_for_portability
#endif

#if defined GNULIB_POSIXCHECK
/* strcspn() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it does not work with multibyte strings if the
   locale encoding is GB18030 and one of the characters to be searched is a
   digit.  */
# undef strcspn
# define strcspn(s,a) \
    (GL_LINK_WARNING ("strcspn cannot work correctly on character strings in multibyte locales - use mbscspn if you care about internationalization"), \
     strcspn (s, a))
#endif

/* Find the first occurrence in S of any character in ACCEPT.  */
#if @GNULIB_STRPBRK@
# if ! @HAVE_STRPBRK@
extern char *strpbrk (char const *__s, char const *__accept);
# endif
# if defined GNULIB_POSIXCHECK
/* strpbrk() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it does not work with multibyte strings if the
   locale encoding is GB18030 and one of the characters to be searched is a
   digit.  */
#  undef strpbrk
#  define strpbrk(s,a) \
     (GL_LINK_WARNING ("strpbrk cannot work correctly on character strings in multibyte locales - use mbspbrk if you care about internationalization"), \
      strpbrk (s, a))
# endif
#elif defined GNULIB_POSIXCHECK
# undef strpbrk
# define strpbrk strpbrk_is_unportable__use_gnulib_module_strpbrk_for_portability
#endif

#if defined GNULIB_POSIXCHECK
/* strspn() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it cannot work with multibyte strings.  */
# undef strspn
# define strspn(s,a) \
    (GL_LINK_WARNING ("strspn cannot work correctly on character strings in multibyte locales - use mbsspn if you care about internationalization"), \
     strspn (s, a))
#endif

#if defined GNULIB_POSIXCHECK
/* strrchr() does not work with multibyte strings if the locale encoding is
   GB18030 and the character to be searched is a digit.  */
# undef strrchr
# define strrchr(s,c) \
    (GL_LINK_WARNING ("strrchr cannot work correctly on character strings in some multibyte locales - use mbsrchr if you care about internationalization"), \
     strrchr (s, c))
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
# if defined GNULIB_POSIXCHECK
#  undef strsep
#  define strsep(s,d) \
     (GL_LINK_WARNING ("strsep cannot work correctly on character strings in multibyte locales - use mbssep if you care about internationalization"), \
      strsep (s, d))
# endif
#elif defined GNULIB_POSIXCHECK
# undef strsep
# define strsep strsep_is_unportable__use_gnulib_module_strsep_for_portability
#endif

#if defined GNULIB_POSIXCHECK
/* strstr() does not work with multibyte strings if the locale encoding is
   different from UTF-8:
   POSIX says that it operates on "strings", and "string" in POSIX is defined
   as a sequence of bytes, not of characters.  */
# undef strstr
# define strstr(a,b) \
    (GL_LINK_WARNING ("strstr cannot work correctly on character strings in most multibyte locales - use mbsstr if you care about internationalization"), \
     strstr (a, b))
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive
   comparison.  */
#if ! @HAVE_STRCASESTR@
extern char *strcasestr (const char *haystack, const char *needle);
#endif
#if defined GNULIB_POSIXCHECK
/* strcasestr() does not work with multibyte strings:
   It is a glibc extension, and glibc implements it only for unibyte
   locales.  */
# undef strcasestr
# define strcasestr(a,b) \
    (GL_LINK_WARNING ("strcasestr does work correctly on character strings in multibyte locales - use mbscasestr if you care about internationalization, or use c-strcasestr if you want a locale independent function"), \
     strcasestr (a, b))
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
extern char *strtok_r (char *restrict s, char const *restrict delim,
		       char **restrict save_ptr);
# endif
# if defined GNULIB_POSIXCHECK
#  undef strtok_r
#  define strtok_r(s,d,p) \
     (GL_LINK_WARNING ("strtok_r cannot work correctly on character strings in multibyte locales - use mbstok_r if you care about internationalization"), \
      strtok_r (s, d, p))
# endif
#elif defined GNULIB_POSIXCHECK
# undef strtok_r
# define strtok_r strtok_r_is_unportable__use_gnulib_module_strtok_r_for_portability
#endif


/* The following functions are not specified by POSIX.  They are gnulib
   extensions.  */

#if @GNULIB_MBSCHR@
/* Locate the first single-byte character C in the character string STRING,
   and return a pointer to it.  Return NULL if C is not found in STRING.
   Unlike strchr(), this function works correctly in multibyte locales with
   encodings such as GB18030.  */
# define mbschr rpl_mbschr /* avoid collision with HP-UX function */
extern char * mbschr (const char *string, int c);
#endif

#if @GNULIB_MBSRCHR@
/* Locate the last single-byte character C in the character string STRING,
   and return a pointer to it.  Return NULL if C is not found in STRING.
   Unlike strrchr(), this function works correctly in multibyte locales with
   encodings such as GB18030.  */
# define mbsrchr rpl_mbsrchr /* avoid collision with HP-UX function */
extern char * mbsrchr (const char *string, int c);
#endif

#if @GNULIB_MBSSTR@
/* Find the first occurrence of the character string NEEDLE in the character
   string HAYSTACK.  Return NULL if NEEDLE is not found in HAYSTACK.
   Unlike strstr(), this function works correctly in multibyte locales with
   encodings different from UTF-8.  */
extern char * mbsstr (const char *haystack, const char *needle);
#endif

#if @GNULIB_MBSCASECMP@
/* Compare the character strings S1 and S2, ignoring case, returning less than,
   equal to or greater than zero if S1 is lexicographically less than, equal to
   or greater than S2.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!
   Unlike strcasecmp(), this function works correctly in multibyte locales.  */
extern int mbscasecmp (const char *s1, const char *s2);
#endif

#if @GNULIB_MBSCASESTR@
/* Find the first occurrence of the character string NEEDLE in the character
   string HAYSTACK, using case-insensitive comparison.
   Note: This function may, in multibyte locales, return success even if
   strlen (haystack) < strlen (needle) !
   Unlike strcasestr(), this function works correctly in multibyte locales.  */
extern char * mbscasestr (const char *haystack, const char *needle);
#endif

#if @GNULIB_MBSCSPN@
/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.
   Unlike strcspn(), this function works correctly in multibyte locales.  */
extern size_t mbscspn (const char *string, const char *accept);
#endif

#if @GNULIB_MBSPBRK@
/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the pointer to it, or NULL if none
   exists.
   Unlike strpbrk(), this function works correctly in multibyte locales.  */
# define mbspbrk rpl_mbspbrk /* avoid collision with HP-UX function */
extern char * mbspbrk (const char *string, const char *accept);
#endif

#if @GNULIB_MBSSPN@
/* Find the first occurrence in the character string STRING of any character
   not in the character string REJECT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.
   Unlike strspn(), this function works correctly in multibyte locales.  */
extern size_t mbsspn (const char *string, const char *reject);
#endif

#if @GNULIB_MBSSEP@
/* Search the next delimiter (multibyte character listed in the character
   string DELIM) starting at the character string *STRINGP.
   If one is found, overwrite it with a NUL, and advance *STRINGP to point
   to the next multibyte character after it.  Otherwise, set *STRINGP to NULL.
   If *STRINGP was already NULL, nothing happens.
   Return the old value of *STRINGP.

   This is a variant of mbstok_r() that supports empty fields.

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.

   See also mbstok_r().  */
extern char * mbssep (char **stringp, const char *delim);
#endif

#if @GNULIB_MBSTOK_R@
/* Parse the character string STRING into tokens separated by characters in
   the character string DELIM.
   If STRING is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
	char s[] = "-abc-=-def";
	char *sp;
	x = mbstok_r(s, "-", &sp);	// x = "abc", sp = "=-def"
	x = mbstok_r(NULL, "-=", &sp);	// x = "def", sp = NULL
	x = mbstok_r(NULL, "=", &sp);	// x = NULL
		// s = "abc\0-def\0"

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.

   See also mbssep().  */
extern char * mbstok_r (char *string, const char *delim, char **save_ptr);
#endif


#ifdef __cplusplus
}
#endif

#endif
