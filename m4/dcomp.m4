# dcomp.m4
# serial 2
dnl Copyright (C) 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# There are three D implementations, see
# <https://en.wikipedia.org/wiki/D_(programming_language)#Implementations>
# <https://wiki.dlang.org/Compilers>
# <https://dub.pm/dub-reference/build_settings/#buildoptions>
# Although each has different possible options, a few options are accepted by
# all of the implementations: -c, -I, -g, -O.  Some essential options, however,
# are different:
#          gdc          ldc2                  dmd
#          ----------   -------------------   ------------
#          -oFILE       -of=FILE, --of=FILE   -of=FILE
#          -lLIBRARY    -L -lLIBRARY          -L=-lLIBRARY
#          -LDIR        -L -LDIR              -L=-LDIR
#          -Wl,OPTION   -L OPTION             -L=OPTION

# Checks for a D implementation.
# Sets DC and DFLAGS (options that can be used with "$DC").
AC_DEFUN([gt_DCOMP],
[
  AC_MSG_CHECKING([for D compiler])
  pushdef([AC_MSG_CHECKING],[:])dnl
  pushdef([AC_CHECKING],[:])dnl
  pushdef([AC_MSG_RESULT],[:])dnl
  AC_ARG_VAR([DC], [D compiler command])
  AC_ARG_VAR([DFLAGS], [D compiler options])
  dnl On OpenBSD, gdc is called 'egdc' and works less well than dmd.
  AC_CHECK_TOOLS([DC], [gdc ldc2 dmd egdc])
  popdef([AC_MSG_RESULT])dnl
  popdef([AC_CHECKING])dnl
  popdef([AC_MSG_CHECKING])dnl
  if test -n "$DC"; then
    ac_result="$DC"
  else
    ac_result="no"
  fi
  AC_MSG_RESULT([$ac_result])
  AC_SUBST([DC])
  if test -z "$DFLAGS" && test -n "$DC"; then
    case `$DC --version | sed -e 's/ .*//' -e 1q` in
      gdc | *-gdc | egdc | *-egdc | LDC*) DFLAGS="-g -O2" ;;
      *)                                  DFLAGS="-g -O" ;;
    esac
  fi
  AC_SUBST([DFLAGS])
])
