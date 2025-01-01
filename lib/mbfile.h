/* Multibyte character I/O: macros for multi-byte encodings.
   Copyright (C) 2001, 2005, 2009-2025 Free Software Foundation, Inc.

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

/* Written by Mitsuru Chinen <mchinen@yamato.ibm.com>
   and Bruno Haible <bruno@clisp.org>.  */

/* The macros in this file implement multi-byte character input from a
   stream.

   mb_file_t
     is the type for multibyte character input stream, usable for variable
     declarations.

   mbf_char_t
     is the type for multibyte character or EOF, usable for variable
     declarations.

   mbf_init (mbf, stream)
     initializes the MB_FILE for reading from stream.

   mbf_getc (mbc, mbf)
     reads the next multibyte character from mbf and stores it in mbc.

   mb_iseof (mbc)
     returns true if mbc represents the EOF value.

   Here are the function prototypes of the macros.

   extern void          mbf_init (mb_file_t mbf, FILE *stream);
   extern void          mbf_getc (mbf_char_t mbc, mb_file_t mbf);
   extern bool          mb_iseof (const mbf_char_t mbc);
 */

#ifndef _MBFILE_H
#define _MBFILE_H 1

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "mbchar.h"

_GL_INLINE_HEADER_BEGIN
#ifndef MBFILE_INLINE
# define MBFILE_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Guarantee two characters of pushback.  */
#define MBFILE_MAX_PUSHBACK 2

struct mbfile_multi {
  FILE *fp;
  bool eof_seen;
  unsigned int pushback_count; /* <= MBFILE_MAX_PUSHBACK */
  mbstate_t state;
  unsigned int bufcount;
  char buf[MBCHAR_BUF_SIZE];
  struct mbchar pushback[MBFILE_MAX_PUSHBACK];
};

MBFILE_INLINE void
mbfile_multi_getc (struct mbchar *mbc, struct mbfile_multi *mbf)
{
  unsigned int new_bufcount;
  size_t bytes;

  /* Return character pushed back, if there is one.  */
  if (mbf->pushback_count > 0)
    {
      mb_copy (mbc, &mbf->pushback[mbf->pushback_count - 1]);
      mbf->pushback_count--;
      return;
    }

  /* If EOF has already been seen, don't use getc.  This matters if
     mbf->fp is connected to an interactive tty.  */
  if (mbf->eof_seen)
    goto eof;

  new_bufcount = mbf->bufcount;

  /* If mbf->state is not in an initial state, some more 32-bit wide character
     may be hiding in the state.  We need to call mbrtoc32 again.  */
  #if GNULIB_MBRTOC32_REGULAR
  assert (mbsinit (&mbf->state));
  #else
  if (mbsinit (&mbf->state))
  #endif
    {
      /* Before using mbrtoc32, we need at least one byte.  */
      if (new_bufcount == 0)
        {
          int c = getc (mbf->fp);
          if (c == EOF)
            {
              mbf->eof_seen = true;
              goto eof;
            }
          mbf->buf[0] = (unsigned char) c;
          new_bufcount++;
        }

      /* Handle most ASCII characters quickly, without calling mbrtoc32().  */
      if (new_bufcount == 1 && is_basic (mbf->buf[0]))
        {
          /* These characters are part of the POSIX portable character set.
             For most of them, namely those in the ISO C basic character set,
             ISO C 99 guarantees that their wide character code is identical to
             their char code.  For the few other ones, this is the case as well,
             in all locale encodings that are in use.  The 32-bit wide character
             code is the same as well.  */
          mbc->wc = mbc->buf[0] = mbf->buf[0];
          mbc->wc_valid = true;
          mbc->ptr = &mbc->buf[0];
          mbc->bytes = 1;
          mbf->bufcount = 0;
          return;
        }
    }

  /* Use mbrtoc32 on an increasing number of bytes.  Read only as many bytes
     from mbf->fp as needed.  This is needed to give reasonable interactive
     behaviour when mbf->fp is connected to an interactive tty.  */
  for (;;)
    {
      /* Feed the bytes one by one into mbrtoc32.  */
      bytes = mbrtoc32 (&mbc->wc, &mbf->buf[mbf->bufcount], new_bufcount - mbf->bufcount, &mbf->state);

      if (bytes == (size_t) -1)
        {
          /* An invalid multibyte sequence was encountered.  */
          mbf->bufcount = new_bufcount;
          /* Return a single byte.  */
          bytes = 1;
          mbc->wc_valid = false;
          /* Allow the next invocation to continue from a sane state.  */
          mbszero (&mbf->state);
          break;
        }
      else if (bytes == (size_t) -2)
        {
          /* An incomplete multibyte character.  */
          mbf->bufcount = new_bufcount;
          if (mbf->bufcount == MBCHAR_BUF_SIZE)
            {
              /* An overlong incomplete multibyte sequence was encountered.  */
              /* Return a single byte.  */
              bytes = 1;
              mbc->wc_valid = false;
              break;
            }
          else
            {
              /* Read one more byte and retry mbrtoc32.  */
              int c = getc (mbf->fp);
              if (c == EOF)
                {
                  /* An incomplete multibyte character at the end.  */
                  mbf->eof_seen = true;
                  bytes = new_bufcount;
                  mbc->wc_valid = false;
                  break;
                }
              mbf->buf[new_bufcount] = (unsigned char) c;
              new_bufcount++;
            }
        }
      else
        {
          #if !GNULIB_MBRTOC32_REGULAR
          if (bytes == (size_t) -3)
            {
              /* The previous multibyte sequence produced an additional 32-bit
                 wide character.  */
              mbf->bufcount = new_bufcount;
              bytes = 0;
            }
          else
          #endif
            {
              bytes = mbf->bufcount + bytes;
              mbf->bufcount = new_bufcount;
              if (bytes == 0)
                {
                  /* A null 32-bit wide character was encountered.  */
                  bytes = 1;
                  assert (mbf->buf[0] == '\0');
                  assert (mbc->wc == 0);
                }
            }
          mbc->wc_valid = true;
          break;
        }
    }

  /* Return the multibyte sequence mbf->buf[0..bytes-1].  */
  mbc->ptr = &mbc->buf[0];
  memcpy (&mbc->buf[0], &mbf->buf[0], bytes);
  mbc->bytes = bytes;

  mbf->bufcount -= bytes;
  if (mbf->bufcount > 0)
    {
      /* It's not worth calling memmove() for so few bytes.  */
      unsigned int count = mbf->bufcount;
      char *p = &mbf->buf[0];

      do
        {
          *p = *(p + bytes);
          p++;
        }
      while (--count > 0);
    }
  return;

eof:
  /* An mbchar_t with bytes == 0 is used to indicate EOF.  */
  mbc->ptr = NULL;
  mbc->bytes = 0;
  mbc->wc_valid = false;
  return;
}

MBFILE_INLINE void
mbfile_multi_ungetc (const struct mbchar *mbc, struct mbfile_multi *mbf)
{
  if (mbf->pushback_count == MBFILE_MAX_PUSHBACK)
    abort ();
  mb_copy (&mbf->pushback[mbf->pushback_count], mbc);
  mbf->pushback_count++;
}

typedef struct mbfile_multi mb_file_t;

typedef mbchar_t mbf_char_t;

#define mbf_init(mbf, stream)                                           \
  ((mbf).fp = (stream),                                                 \
   (mbf).eof_seen = false,                                              \
   (mbf).pushback_count = 0,                                            \
   mbszero (&(mbf).state),                                              \
   (mbf).bufcount = 0)

#define mbf_getc(mbc, mbf) mbfile_multi_getc (&(mbc), &(mbf))

#define mbf_ungetc(mbc, mbf) mbfile_multi_ungetc (&(mbc), &(mbf))

#define mb_iseof(mbc) ((mbc).bytes == 0)


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _MBFILE_H */
