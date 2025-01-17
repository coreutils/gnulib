/* Iterating through multibyte strings: macros for multi-byte encodings.
   Copyright (C) 2001, 2005, 2007, 2009-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>.  */

/* The macros in this file implement forward iteration through a
   multi-byte string, without knowing its length a-priori.

   With these macros, an iteration loop that looks like

      char *iter;
      for (iter = buf; *iter != '\0'; iter++)
        {
          do_something (*iter);
        }

   becomes

      mbui_iterator_t iter;
      for (mbui_init (iter, buf); mbui_avail (iter); mbui_advance (iter))
        {
          do_something (mbui_cur_ptr (iter), mb_len (mbui_cur (iter)));
        }

   The benefit of these macros over plain use of mbrtowc is:
   - Handling of invalid multibyte sequences is possible without
     making the code more complicated, while still preserving the
     invalid multibyte sequences.

   Compared to mbiter.h, the macros here don't need to know the string's
   length a-priori.  The downside is that at each step, the look-ahead
   that guards against overrunning the terminating '\0' is more expensive.
   The mbui_* macros are therefore suitable when there is a high probability
   that only the first few multibyte characters need to be inspected.
   Whereas the mbi_* macros are better if usually the iteration runs
   through the entire string.

   mbui_iterator_t
     is a type usable for variable declarations.

   mbui_init (iter, startptr)
     initializes the iterator, starting at startptr.

   mbui_avail (iter)
     returns true if there are more multibyte characters available before
     the end of string is reached. In this case, mbui_cur (iter) is
     initialized to the next multibyte character.

   mbui_advance (iter)
     advances the iterator by one multibyte character.

   mbui_cur (iter)
     returns the current multibyte character, of type mbchar_t.  All the
     macros defined in mbchar.h can be used on it.

   mbui_cur_ptr (iter)
     return a pointer to the beginning of the current multibyte character.

   mbui_reloc (iter, ptrdiff)
     relocates iterator when the string is moved by ptrdiff bytes.

   mbui_copy (&destiter, &srciter)
     copies srciter to destiter.

   Here are the function prototypes of the macros.

   extern void          mbui_init (mbui_iterator_t iter, const char *startptr);
   extern bool          mbui_avail (mbui_iterator_t iter);
   extern void          mbui_advance (mbui_iterator_t iter);
   extern mbchar_t      mbui_cur (mbui_iterator_t iter);
   extern const char *  mbui_cur_ptr (mbui_iterator_t iter);
   extern void          mbui_reloc (mbui_iterator_t iter, ptrdiff_t ptrdiff);
   extern void          mbui_copy (mbui_iterator_t *new, const mbui_iterator_t *old);
 */

#ifndef _MBUITER_H
#define _MBUITER_H 1

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE,
   _GL_ATTRIBUTE_ALWAYS_INLINE.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <wchar.h>

#include "mbchar.h"
#include "strnlen1.h"

_GL_INLINE_HEADER_BEGIN
#ifndef MBUITER_INLINE
# define MBUITER_INLINE _GL_INLINE _GL_ATTRIBUTE_ALWAYS_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


struct mbuiter_multi
{
  #if !GNULIB_MBRTOC32_REGULAR
  bool in_shift;        /* true if next byte may not be interpreted as ASCII */
                        /* If GNULIB_MBRTOC32_REGULAR, it is always false,
                           so optimize it away.  */
  #endif
  mbstate_t state;      /* if in_shift: current shift state */
                        /* If GNULIB_MBRTOC32_REGULAR, it is in an initial state
                           before and after every mbuiter_multi_next invocation.
                         */
  bool next_done;       /* true if mbui_avail has already filled the following */
  unsigned int cur_max; /* A cache of MB_CUR_MAX.  */
  struct mbchar cur;    /* the current character:
        const char *cur.ptr          pointer to current character
        The following are only valid after mbui_avail.
        size_t cur.bytes             number of bytes of current character
        bool cur.wc_valid            true if wc is a valid 32-bit wide character
        char32_t cur.wc              if wc_valid: the current character
        */
};

MBUITER_INLINE void
mbuiter_multi_next (struct mbuiter_multi *iter)
{
  if (iter->next_done)
    return;
  #if !GNULIB_MBRTOC32_REGULAR
  if (iter->in_shift)
    goto with_shift;
  #endif
  /* Handle most ASCII characters quickly, without calling mbrtowc().  */
  if (is_basic (*iter->cur.ptr))
    {
      /* These characters are part of the POSIX portable character set.
         For most of them, namely those in the ISO C basic character set,
         ISO C 99 guarantees that their wide character code is identical to
         their char code.  For the few other ones, this is the case as well,
         in all locale encodings that are in use.  The 32-bit wide character
         code is the same as well.  */
      iter->cur.bytes = 1;
      iter->cur.wc = *iter->cur.ptr;
      iter->cur.wc_valid = true;
    }
  else
    {
      assert (mbsinit (&iter->state));
      #if !GNULIB_MBRTOC32_REGULAR
      iter->in_shift = true;
    with_shift:
      #endif
      iter->cur.bytes = mbrtoc32 (&iter->cur.wc, iter->cur.ptr,
                                  strnlen1 (iter->cur.ptr, iter->cur_max),
                                  &iter->state);
      if (iter->cur.bytes == (size_t) -1)
        {
          /* An invalid multibyte sequence was encountered.  */
          iter->cur.bytes = 1;
          iter->cur.wc_valid = false;
          /* Allow the next invocation to continue from a sane state.  */
          #if !GNULIB_MBRTOC32_REGULAR
          iter->in_shift = false;
          #endif
          mbszero (&iter->state);
        }
      else if (iter->cur.bytes == (size_t) -2)
        {
          /* An incomplete multibyte character at the end.  */
          iter->cur.bytes = strlen (iter->cur.ptr);
          iter->cur.wc_valid = false;
          /* Whether to set iter->in_shift = false and reset iter->state
             or not is not important; the string end is reached anyway.  */
        }
      else
        {
          if (iter->cur.bytes == 0)
            {
              /* A null wide character was encountered.  */
              iter->cur.bytes = 1;
              assert (*iter->cur.ptr == '\0');
              assert (iter->cur.wc == 0);
            }
          #if !GNULIB_MBRTOC32_REGULAR
          else if (iter->cur.bytes == (size_t) -3)
            /* The previous multibyte sequence produced an additional 32-bit
               wide character.  */
            iter->cur.bytes = 0;
          #endif
          iter->cur.wc_valid = true;

          /* When in an initial state, we can go back treating ASCII
             characters more quickly.  */
          #if !GNULIB_MBRTOC32_REGULAR
          if (mbsinit (&iter->state))
            iter->in_shift = false;
          #endif
        }
    }
  iter->next_done = true;
}

MBUITER_INLINE void
mbuiter_multi_reloc (struct mbuiter_multi *iter, ptrdiff_t ptrdiff)
{
  iter->cur.ptr += ptrdiff;
}

MBUITER_INLINE void
mbuiter_multi_copy (struct mbuiter_multi *new_iter, const struct mbuiter_multi *old_iter)
{
  #if !GNULIB_MBRTOC32_REGULAR
  if ((new_iter->in_shift = old_iter->in_shift))
    memcpy (&new_iter->state, &old_iter->state, sizeof (mbstate_t));
  else
  #endif
    mbszero (&new_iter->state);
  new_iter->next_done = old_iter->next_done;
  new_iter->cur_max = old_iter->cur_max;
  mb_copy (&new_iter->cur, &old_iter->cur);
}

/* Iteration macros.  */
typedef struct mbuiter_multi mbui_iterator_t;
#if !GNULIB_MBRTOC32_REGULAR
#define mbui_init(iter, startptr) \
  ((iter).cur.ptr = (startptr), \
   (iter).in_shift = false, mbszero (&(iter).state), \
   (iter).next_done = false, \
   (iter).cur_max = MB_CUR_MAX)
#else
/* Optimized: no in_shift.  */
#define mbui_init(iter, startptr) \
  ((iter).cur.ptr = (startptr), \
   mbszero (&(iter).state), \
   (iter).next_done = false, \
   (iter).cur_max = MB_CUR_MAX)
#endif
#define mbui_avail(iter) \
  (mbuiter_multi_next (&(iter)), !mb_isnul ((iter).cur))
#define mbui_advance(iter) \
  ((iter).cur.ptr += (iter).cur.bytes, (iter).next_done = false)

/* Access to the current character.  */
#define mbui_cur(iter) (iter).cur
#define mbui_cur_ptr(iter) (iter).cur.ptr

/* Relocation.  */
#define mbui_reloc(iter, ptrdiff) mbuiter_multi_reloc (&iter, ptrdiff)

/* Copying an iterator.  */
#define mbui_copy mbuiter_multi_copy


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _MBUITER_H */
