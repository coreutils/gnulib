/* Convert multibyte character and return next 16-bit wide character.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <stdlib.h>
#include <wchar.h>

/* We must find room for a two-bytes char16_t in an mbstate_t, without
   interfering with the existing use of the mbstate_t in mbrtoc32.  */
static_assert (sizeof (mbstate_t) >= 4);

#if GNULIB_defined_mbstate_t                              /* AIX */
/* mbstate_t has at least 4 bytes.  They are used as coded in
   gnulib/lib/mbrtowc.c.  */
# define SET_EXTRA_STATE(ps, c16) \
    (((char *)(ps))[0] = 8, \
     ((char *)(ps))[1] = (unsigned char) ((c16) >> 8), \
     ((char *)(ps))[2] = (unsigned char) ((c16) & 0xff))
# define GET_EXTRA_STATE(ps) \
    (((char *)(ps))[0] == 8 \
     ? ((unsigned char) ((char *)(ps))[1] << 8) | (unsigned char) ((char *)(ps))[2] \
     : 0)
# define RESET_EXTRA_STATE(ps) \
    (((char *)(ps))[0] = 0)
#elif __GLIBC__ >= 2
/* mbstate_t is defined in <bits/types/__mbstate_t.h>.
   For more details, see glibc/iconv/skeleton.c.  */
# define SET_EXTRA_STATE(ps, c16) \
    ((ps)->__count |= ((unsigned int) (c16) << 16))
# define GET_EXTRA_STATE(ps) \
    (((unsigned int) (ps)->__count) >> 16)
# define RESET_EXTRA_STATE(ps) \
    ((ps)->__count &= 0xffff)
#elif (defined __APPLE__ && defined __MACH__) || defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __minix
/* macOS, FreeBSD, NetBSD, OpenBSD, Minix */
/* On macOS, mbstate_t is defined in <machine/_types.h>.
   It is an opaque aligned 128-byte struct, of which at most the first
   12 bytes are used.
   For more details, see the __mbsinit implementations in
   Libc-<version>/locale/FreeBSD/
   {ascii,none,euc,mskanji,big5,gb2312,gbk,gb18030,utf8,utf2}.c.  */
/* On FreeBSD, mbstate_t is defined in src/sys/sys/_types.h.
   It is an opaque aligned 128-byte struct, of which at most the first
   12 bytes are used.
   For more details, see the __mbsinit implementations in
   src/lib/libc/locale/
   {ascii,none,euc,mskanji,big5,gb2312,gbk,gb18030,utf8}.c.  */
/* On NetBSD, mbstate_t is defined in src/sys/sys/ansi.h.
   It is an opaque aligned 128-byte struct, of which at most the first
   28 bytes are used.
   For more details, see the *State types in
   src/lib/libc/citrus/modules/citrus_*.c
   (ignoring citrus_{hz,iso2022,utf7,viqr,zw}.c, since these implement
   stateful encodings, not usable as locale encodings).  */
/* On OpenBSD, mbstate_t is defined in src/sys/sys/_types.h.
   It is an opaque aligned 128-byte struct, of which at most the first
   12 bytes are used.
   For more details, see src/lib/libc/citrus/citrus_*.c.  */
/* Minix has borrowed its mbstate_t type and mbrtowc implementation from the
   BSDs.  */
# define SET_EXTRA_STATE(ps, c16) \
    (((unsigned short *)(ps))[16] = (c16))
# define GET_EXTRA_STATE(ps) \
    (((unsigned short *)(ps))[16])
# define RESET_EXTRA_STATE(ps) \
    (((unsigned short *)(ps))[16] = 0)
#elif defined __sun                                       /* Solaris */
/* On Solaris, mbstate_t is defined in <wchar_impl.h>.
   It is an opaque aligned 24-byte or 32-byte struct, of which at most the first
   20 or 28 bytes are used.
   For more details on OpenSolaris derivatives, see the *State types in
   illumos-gate/usr/src/lib/libc/port/locale/
   {none,euc,mskanji,big5,gb2312,gbk,gb18030,utf8}.c.  */
# define SET_EXTRA_STATE(ps, c16) \
    (((unsigned short *)(ps))[10] = (c16))
# define GET_EXTRA_STATE(ps) \
    (((unsigned short *)(ps))[10])
# define RESET_EXTRA_STATE(ps) \
    (((unsigned short *)(ps))[10] = 0)
#elif defined __CYGWIN__
/* On Cygwin, mbstate_t is defined in <sys/_types.h>.
   For more details, see newlib/libc/stdlib/mbtowc_r.c and
   winsup/cygwin/strfuncs.cc.  */
# define SET_EXTRA_STATE(ps, c16) \
    ((ps)->__count = 8, \
     (ps)->__value.__wch = (c16))
# define GET_EXTRA_STATE(ps) \
    ((ps)->__count == 8 ? (ps)->__value.__wch : 0)
# define RESET_EXTRA_STATE(ps) \
    ((ps)->__count = 0)
#elif defined _WIN32 && !defined __CYGWIN__               /* Native Windows.  */
/* MSVC defines 'mbstate_t' as an aligned 8-byte struct.
   On mingw, 'mbstate_t' is sometimes defined as 'int', sometimes defined
   as an aligned 8-byte struct, of which the first 4 bytes matter.  */
# define SET_EXTRA_STATE(ps, c16) \
    (((char *)(ps))[3] = 4, \
     ((unsigned short *)(ps))[0] = (c16))
# define GET_EXTRA_STATE(ps) \
    (((char *)(ps))[3] == 4 \
     ? ((unsigned short *)(ps))[0] \
     : 0)
# define RESET_EXTRA_STATE(ps) \
    (((char *)(ps))[3] = 0, \
     ((unsigned short *)(ps))[0] = 0)
#elif defined __ANDROID__                                 /* Android */
/* Android defines 'mbstate_t' in <bits/mbstate_t.h>.
   It is an opaque 4-byte or 8-byte struct.
   For more details, see
   bionic/libc/private/bionic_mbstate.h
   bionic/libc/bionic/mbrtoc32.cpp
   bionic/libc/bionic/mbrtoc16.cpp
 */
# define SET_EXTRA_STATE(ps, c16) \
    (((char *)(ps))[3] = 4, \
     ((char *)(ps))[0] = (unsigned char) ((c16) & 0xff), \
     ((char *)(ps))[1] = (unsigned char) ((c16) >> 8))
# define GET_EXTRA_STATE(ps) \
    (((char *)(ps))[3] == 4 \
     ? ((unsigned char) ((char *)(ps))[1] << 8) | (unsigned char) ((char *)(ps))[0] \
     : 0)
# define RESET_EXTRA_STATE(ps) \
    (((char *)(ps))[0] = ((char *)(ps))[1] = ((char *)(ps))[2] = ((char *)(ps))[3] = 0)
#else
/* This is just a wild guess, for other platforms.  It likely causes unit test
   failures.  */
# define SET_EXTRA_STATE(ps, c16) \
    (((char *)(ps))[1] = (unsigned char) ((c16) >> 8), \
     ((char *)(ps))[2] = (unsigned char) ((c16) & 0xff))
# define GET_EXTRA_STATE(ps) \
    (((unsigned char) ((char *)(ps))[1] << 8) | (unsigned char) ((char *)(ps))[2])
# define RESET_EXTRA_STATE(ps) \
    (((char *)(ps))[1] = ((char *)(ps))[2] = 0)
#endif

static mbstate_t internal_state;

size_t
mbrtoc16 (char16_t *pwc, const char *s, size_t n, mbstate_t *ps)
#undef mbrtoc16
{
  /* It's simpler to handle the case s == NULL upfront, than to worry about
     this case later, before every test of pwc and n.  */
  if (s == NULL)
    {
      pwc = NULL;
      s = "";
      n = 1;
    }

  if (ps == NULL)
    ps = &internal_state;

  if (GET_EXTRA_STATE (ps) == 0)
    {
      if (n == 0)
        return (size_t) -2;

      char32_t c32;
      size_t ret = mbrtoc32 (&c32, s, n, ps);
      if (ret == (size_t)(-1) || ret == (size_t)(-2))
        ;
      else if (ret == (size_t)(-3))
        {
          /* When mbrtoc32 returns several char32_t values for a single
             multibyte character, they are all in the Unicode BMP range.  */
          if (c32 >= 0x10000)
            abort ();
          if (pwc != NULL)
            *pwc = c32;
        }
      else if (c32 < 0x10000)
        {
          if (pwc != NULL)
            *pwc = c32;
        }
      else
        {
          if (c32 >= 0x110000)
            abort ();
          /* Decompose a Unicode character into a high surrogate and a low
             surrogate.  */
          char16_t surr1 = 0xd800 + ((c32 - 0x10000) >> 10);
          char16_t surr2 = 0xdc00 + ((c32 - 0x10000) & 0x3ff);
          if (pwc != NULL)
            *pwc = surr1;
          SET_EXTRA_STATE (ps, surr2);
        }
      return ret;
    }
  else
    {
      if (pwc != NULL)
        *pwc = GET_EXTRA_STATE (ps);
      RESET_EXTRA_STATE (ps);
      return (size_t)(-3);
    }
}
