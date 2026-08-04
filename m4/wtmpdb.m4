# wtmpdb.m4
# serial 1
dnl Copyright (C) 2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Sets WTMPDB_CHOICE to 'yes' or 'no', depending on the preferred use of
# wtmpdb APIs.
AC_DEFUN([gl_WTMPDB_CHOICE],
[
  AC_MSG_CHECKING([whether to use wtmpdb APIs])
  AC_ARG_WITH([wtmpdb],
    [  --with-wtmpdb           use wtmpdb APIs],
    [WTMPDB_CHOICE="$withval"],
    [WTMPDB_CHOICE=no])
  AC_MSG_RESULT([$WTMPDB_CHOICE])
  AC_SUBST([WTMPDB_CHOICE])
])

# Pre-built package name for the libwtmpdb library:
# - On Debian and Debian-based systems: libwtmpdb-dev,
# - On Red Hat distributions: --.
# - Other: https://repology.org/project/wtmpdb/versions
