/* Quote string descriptors for output.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#ifndef _STRING_DESC_QUOTEARG_H
#define _STRING_DESC_QUOTEARG_H 1

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE, _GL_ATTRIBUTE_MALLOC,
   _GL_ATTRIBUTE_NONNULL, _GL_ATTRIBUTE_RETURNS_NONNULL.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include "string-desc.h"
#include "quotearg.h"


_GL_INLINE_HEADER_BEGIN
#ifndef GL_STRING_DESC_QUOTEARG_INLINE
# define GL_STRING_DESC_QUOTEARG_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Place into buffer BUFFER (of size BUFFERSIZE) a quoted version of
   argument ARG, using O to control quoting.
   If O is null, use the default.
   Terminate the output with a null character, and return the written
   size of the output, not counting the terminating null.
   If BUFFERSIZE is too small to store the output string, return the
   value that would have been returned had BUFFERSIZE been large enough.
   On output, BUFFER might contain embedded null bytes if the style of O
   does not use backslash escapes and the flags of O do not request
   elision of null bytes.  */
#if 0
extern size_t sd_quotearg_buffer (char *restrict buffer,
                                  size_t buffersize,
                                  string_desc_t arg,
                                  struct quoting_options const *o);
#endif

/* Like sd_quotearg_buffer, except return the result in a newly
   allocated buffer and store its length, excluding the terminating null
   byte, in *SIZE.  It is the caller's responsibility to free the result.
   The result might contain embedded null bytes if the style of O does
   not use backslash escapes and the flags of O do not request elision
   of null bytes.  */
#if 0
extern char *sd_quotearg_alloc (string_desc_t arg,
                                size_t *size,
                                struct quoting_options const *o)
  _GL_ATTRIBUTE_NONNULL ((2))
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE
  _GL_ATTRIBUTE_RETURNS_NONNULL;
#endif

/* Use storage slot N to return a quoted version of the string ARG.
   Use the default quoting options.
   The returned value points to static storage that can be
   reused by the next call to this function with the same value of N.
   N must be nonnegative.  */
#if 0
extern char *sd_quotearg_n (int n, string_desc_t arg);
#endif

/* Equivalent to sd_quotearg_n (0, ARG).  */
#if 0
extern char *sd_quotearg (string_desc_t arg);
#endif

/* Use style S and storage slot N to return a quoted version of the string ARG.
   This is like sd_quotearg_n (N, ARG), except that it uses S
   with no other options to specify the quoting method.  */
#if 0
extern char *sd_quotearg_n_style (int n, enum quoting_style s,
                                  string_desc_t arg);
#endif

/* Equivalent to sd_quotearg_n_style (0, S, ARG).  */
#if 0
extern char *sd_quotearg_style (enum quoting_style s,
                                string_desc_t arg);
#endif

/* Like sd_quotearg (ARG), except also quote any instances of CH.
   See set_char_quoting for a description of acceptable CH values.  */
#if 0
extern char *sd_quotearg_char (string_desc_t arg, char ch);
#endif

/* Equivalent to sd_quotearg_char (ARG, ':').  */
#if 0
extern char *sd_quotearg_colon (string_desc_t arg);
#endif

/* Like sd_quotearg_n_style (N, S, ARG) but with S as
   custom_quoting_style with left quote as LEFT_QUOTE and right quote
   as RIGHT_QUOTE.  See set_custom_quoting for a description of acceptable
   LEFT_QUOTE and RIGHT_QUOTE values.  */
#if 0
extern char *sd_quotearg_n_custom (int n,
                                   char const *left_quote,
                                   char const *right_quote,
                                   string_desc_t arg);
#endif

/* Equivalent to
   sd_quotearg_n_custom (0, LEFT_QUOTE, RIGHT_QUOTE, ARG).  */
#if 0
extern char *sd_quotearg_custom (char const *left_quote,
                                 char const *right_quote,
                                 string_desc_t arg);
#endif


/* ==== Inline function definitions ==== */

GL_STRING_DESC_QUOTEARG_INLINE size_t
sd_quotearg_buffer (char *restrict buffer, size_t buffersize,
                    string_desc_t arg,
                    struct quoting_options const *o)
{
  return quotearg_buffer (buffer, buffersize,
                          sd_data (arg), sd_length (arg),
                          o);
}

GL_STRING_DESC_QUOTEARG_INLINE
_GL_ATTRIBUTE_NONNULL ((2))
_GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE
_GL_ATTRIBUTE_RETURNS_NONNULL
char *
sd_quotearg_alloc (string_desc_t arg,
                   size_t *size,
                   struct quoting_options const *o)
{
  return quotearg_alloc_mem (sd_data (arg), sd_length (arg),
                             size,
                             o);
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_n (int n, string_desc_t arg)
{
  return quotearg_n_mem (n, sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg (string_desc_t arg)
{
  return quotearg_mem (sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_n_style (int n, enum quoting_style s, string_desc_t arg)
{
  return quotearg_n_style_mem (n, s,
                               sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_style (enum quoting_style s, string_desc_t arg)
{
  return quotearg_style_mem (s,
                             sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_char (string_desc_t arg, char ch)
{
  return quotearg_char_mem (sd_data (arg), sd_length (arg),
                            ch);
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_colon (string_desc_t arg)
{
  return quotearg_colon_mem (sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_n_custom (int n,
                      char const *left_quote, char const *right_quote,
                      string_desc_t arg)
{
  return quotearg_n_custom_mem (n, left_quote, right_quote,
                                sd_data (arg), sd_length (arg));
}

GL_STRING_DESC_QUOTEARG_INLINE char *
sd_quotearg_custom (char const *left_quote, char const *right_quote,
                    string_desc_t arg)
{
  return quotearg_custom_mem (left_quote, right_quote,
                              sd_data (arg), sd_length (arg));
}


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END


#endif /* _STRING_DESC_QUOTEARG_H */
