# sys_msg_h.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Written by Bruno Haible.

AC_DEFUN_ONCE([gl_SYS_MSG_H],
[
  AC_CHECK_HEADERS_ONCE([sys/msg.h])
  dnl For now, we provide a <sys/msg.h> wrapper only if the system already has
  dnl a <sys/msg.h>.
  if test $ac_cv_header_sys_msg_h = yes; then
    GL_GENERATE_SYS_MSG_H=true
    gl_CHECK_NEXT_HEADERS([sys/msg.h])
  else
    GL_GENERATE_SYS_MSG_H=false
  fi
])
