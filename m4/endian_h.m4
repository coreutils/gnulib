# endian_h.m4
# serial 1
dnl Copyright 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl A placeholder for <endian.h>, for platforms that have issues.

AC_DEFUN_ONCE([gl_ENDIAN_H],
[
  AC_REQUIRE([gl_BIGENDIAN])

  AC_CHECK_HEADERS_ONCE([endian.h])
  if test $ac_cv_header_endian_h = yes; then
    AC_CACHE_CHECK([if endian.h conforms to POSIX],
      [gl_cv_header_working_endian_h],
      [gl_cv_header_working_endian_h=no
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
[[
#include <endian.h>
]],
[[
#if LITTLE_ENDIAN == BIG_ENDIAN
# error "Endian macros not unique."
#endif
#if BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN
# error "Byte order not defined."
#endif

/* Check for uint16_t, uint32_t, uint64_t along with
   byte order conversion functions that accept floating-point
   arguments.  */

/* Big endian to host.  */
uint16_t value16_1 = be16toh (0.0);
uint32_t value32_1 = be32toh (0.0);
uint64_t value64_1 = be64toh (0.0);

/* Host to big endian.  */
uint16_t value16_2 = htobe16 (0.0);
uint32_t value32_2 = htobe32 (0.0);
uint64_t value64_2 = htobe64 (0.0);

/* Little endian to host.  */
uint16_t value16_3 = le16toh (0.0);
uint32_t value32_3 = le32toh (0.0);
uint64_t value64_3 = le64toh (0.0);

/* Host to little endian.  */
uint16_t value16_4 = htole16 (0.0);
uint32_t value32_4 = htole32 (0.0);
uint64_t value64_4 = htole64 (0.0);

/* Make sure the variables get used.  */
return !(value16_1 + value32_1 + value64_1
         + value16_2 + value32_2 + value64_2
         + value16_3 + value32_3 + value64_3
         + value16_4 + value32_4 + value64_4);
]])],
         [gl_cv_header_working_endian_h=yes],
         [gl_cv_header_working_endian_h=no])
      ])
  fi
  if test $gl_cv_header_working_endian_h = yes; then
    GL_GENERATE_ENDIAN_H=false
  else
    GL_GENERATE_ENDIAN_H=true
  fi
])
