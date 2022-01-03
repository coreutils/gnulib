/* Construct a regular expression from a literal string.
   Copyright (C) 1995, 2010-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2010.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _REGEX_QUOTE_H
#define _REGEX_QUOTE_H

#include <stdbool.h>
#include <stdlib.h>


/* Specifies a quotation task for converting a fixed string to a regular
   expression pattern.  */
struct regex_quote_spec
{
  /* True if the regular expression pattern consists of multibyte characters
     (in the encoding given by the LC_CTYPE category of the locale),
     false if it consists of single bytes or UTF-8 characters.  */
  unsigned int /*bool*/ multibyte : 1;
  /* True if the regular expression pattern shall match only entire lines.  */
  unsigned int /*bool*/ anchored : 1;
  /* Set of characters that need to be escaped (all ASCII), as a
     NUL-terminated string.  */
  char special[30 + 1];
};


/* Creates a quotation task that produces a POSIX regular expression, that is,
   a pattern that can be compiled with regcomp().
   CFLAGS can be 0 or REG_EXTENDED.
   If it is 0, the result is a Basic Regular Expression (BRE)
   <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03>.
   If it is REG_EXTENDED, the result is an Extended Regular Expression (ERE)
   <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_04>.
   If ANCHORED is false, the regular expression will match substrings of lines.
   If ANCHORED is true, it will match only complete lines,  */
extern struct regex_quote_spec
       regex_quote_spec_posix (int cflags, bool anchored);

/* Creates a quotation task that produces a regular expression that can be
   compiled with the GNU API function re_compile_pattern().
   SYNTAX describes the syntax of the regular expression (such as
   RE_SYNTAX_POSIX_BASIC, RE_SYNTAX_POSIX_EXTENDED, RE_SYNTAX_EMACS, all
   defined in <regex.h>).  It must be the same value as 're_syntax_options'
   at the moment of the re_compile_pattern() call.
   If ANCHORED is false, the regular expression will match substrings of lines.
   If ANCHORED is true, it will match only complete lines,  */
extern struct regex_quote_spec
       regex_quote_spec_gnu (unsigned long /*reg_syntax_t*/ syntax, bool anchored);

/* Creates a quotation task that produces a PCRE regular expression, that is,
   a pattern that can be compiled with pcre_compile().
   OPTIONS is the same value as the second argument passed to pcre_compile().
   If ANCHORED is false, the regular expression will match substrings of lines.
   If ANCHORED is true, it will match only complete lines,  */
extern struct regex_quote_spec
       regex_quote_spec_pcre (int options, bool anchored);


/* Returns the number of bytes needed for the quoted string.  */
extern size_t
       regex_quote_length (const char *string, const struct regex_quote_spec *spec);

/* Copies the quoted string to p and returns the incremented p.
   There must be room for regex_quote_length (string, spec) + 1 bytes at p.  */
extern char *
       regex_quote_copy (char *restrict p,
                         const char *string, const struct regex_quote_spec *spec);

/* Returns the freshly allocated quoted string.  */
extern char *
       regex_quote (const char *string, const struct regex_quote_spec *spec)
       _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE
       _GL_ATTRIBUTE_RETURNS_NONNULL;


#endif /* _REGEX_QUOTE_H */
