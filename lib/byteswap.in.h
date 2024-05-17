/* byteswap.h - Byte swapping
   Copyright (C) 2005, 2007, 2009-2024 Free Software Foundation, Inc.
   Written by Oskar Liljeblad <oskar@osk.mine.nu>, 2005.

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

#ifndef _GL_BYTESWAP_H
#define _GL_BYTESWAP_H 1

/* This file uses _GL_INLINE.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdint.h>

_GL_INLINE_HEADER_BEGIN
#ifndef _GL_BYTESWAP_INLINE
# define _GL_BYTESWAP_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Given an unsigned 16-bit argument X, return the value corresponding to
   X with reversed byte order.  */
_GL_BYTESWAP_INLINE uint16_t
bswap_16 (uint16_t x)
{
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))            \
     || (defined __has_builtin && __has_builtin (__builtin_bswap16))
  return __builtin_bswap16 (x);
#else
  return (((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)));
#endif
}

/* Given an unsigned 32-bit argument X, return the value corresponding to
   X with reversed byte order.  */
_GL_BYTESWAP_INLINE uint32_t
bswap_32 (uint32_t x)
{
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))            \
     || (defined __has_builtin && __has_builtin (__builtin_bswap32))
  return __builtin_bswap32 (x);
#else
  return ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)
          | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24));
#endif
}

/* Given an unsigned 64-bit argument X, return the value corresponding to
   X with reversed byte order.  */
_GL_BYTESWAP_INLINE uint64_t
bswap_64 (uint64_t x)
{
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))            \
     || (defined __has_builtin && __has_builtin (__builtin_bswap64))
  return __builtin_bswap64 (x);
#else
  return ((((x) & 0xff00000000000000ull) >> 56)
          | (((x) & 0x00ff000000000000ull) >> 40)
          | (((x) & 0x0000ff0000000000ull) >> 24)
          | (((x) & 0x000000ff00000000ull) >> 8)
          | (((x) & 0x00000000ff000000ull) << 8)
          | (((x) & 0x0000000000ff0000ull) << 24)
          | (((x) & 0x000000000000ff00ull) << 40)
          | (((x) & 0x00000000000000ffull) << 56));
#endif
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _GL_BYTESWAP_H */
