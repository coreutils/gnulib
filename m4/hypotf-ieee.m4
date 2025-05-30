# hypotf-ieee.m4
# serial 1
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl This macro is in a separate file (not in hypotf.m4 and not inlined in the
dnl module description), so that gl_FUNC_HYPOTF can test whether 'aclocal' has
dnl found uses of this macro.

AC_DEFUN([gl_FUNC_HYPOTF_IEEE],
[
  m4_divert_text([INIT_PREPARE], [gl_hypotf_required=ieee])
  AC_REQUIRE([gl_FUNC_HYPOTF])
])
