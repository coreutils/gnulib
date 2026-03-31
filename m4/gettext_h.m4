# gettext_h.m4
# serial 2
dnl Copyright (C) 2025-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_GETTEXT_H],
[
  AC_SUBST([LIBINTL])
  AC_SUBST([LTLIBINTL])
  AC_CACHE_CHECK([for LC_MESSAGES value], [gl_cv_val_LC_MESSAGES],
    [AC_COMPUTE_INT([gl_cv_val_LC_MESSAGES],
       [LC_MESSAGES],
       [#include <locale.h>
       ],
       [# The value Gnulib uses when <locale.h> does not define LC_MESSAGES.
        gl_cv_LC_MESSAGES=1729])])
  AC_DEFINE_UNQUOTED([_GL_LC_MESSAGES], [$gl_cv_val_LC_MESSAGES],
    [The value of LC_MESSAGES, or 1729 on hosts that lack it.])
  AH_BOTTOM([
/* The text domainname for Gnulib messages.  Ordinarily this is "gnulib",
   but packages that do their own translations of Gnulib can use something
   different by defining GNULIB_TEXT_DOMAIN in their config.h file.  */
#ifndef GNULIB_TEXT_DOMAIN
# define GNULIB_TEXT_DOMAIN/**/"gnulib"
#endif
])
])
