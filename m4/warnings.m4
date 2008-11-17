# warnings.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Simon Josefsson

# gl_AS_VAR_IF(VAR, VALUE, [IF-MATCH], [IF-NOT-MATCH])
# ----------------------------------------------------
# Provide the functionality of AS_VAR_IF if Autoconf does not have it.
m4_ifdef([AS_VAR_IF],
[m4_copy([AS_VAR_IF], [gl_AS_VAR_IF])],
[m4_define([gl_AS_VAR_IF],
[AS_IF([test x"AS_VAR_GET([$1])" = x""$2], [$3], [$4])])])

# gl_AS_VAR_APPEND(VAR, VALUE)
# ----------------------------
# Provide the functionality of AS_VAR_APPEND if Autoconf does not have it.
m4_ifdef([AS_VAR_APPEND],
[m4_copy([AS_VAR_APPEND], [gl_AS_VAR_APPEND])],
[m4_define([gl_AS_VAR_APPEND],
[AS_VAR_SET([$1], [AS_VAR_GET([$1])$2])])])

# gl_WARN_ADD(PARAMETER, [VARIABLE = WARN_CFLAGS])
# ------------------------------------------------
# Adds parameter to WARN_CFLAGS if the compiler supports it.  For example,
# gl_WARN_ADD([-Wparentheses]).
AC_DEFUN([gl_WARN_ADD],
[AS_VAR_PUSHDEF([gl_Warn], [gl_cv_warn_$1])dnl
AC_CACHE_CHECK([whether compiler handles $1], [gl_Warn], [
  save_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="${CPPFLAGS} $1"
  AC_PREPROC_IFELSE([AC_LANG_PROGRAM([])],
                    [AS_VAR_SET([gl_Warn], [yes])],
		    [AS_VAR_SET([gl_Warn], [no])])
  CPPFLAGS="$save_CPPFLAGS"
])
AS_VAR_PUSHDEF([gl_Flags], m4_if([$2], [], [[WARN_CFLAGS]], [[$2]]))dnl
gl_AS_VAR_IF([gl_Warn], [yes], [gl_AS_VAR_APPEND([gl_Flags], [" $1"])])
AS_VAR_POPDEF([gl_Flags])dnl
AS_VAR_POPDEF([gl_Warn])dnl
m4_ifval([$2], [AS_LITERAL_IF([$2], [AC_SUBST([$2])], [])])dnl
])

# gl_WARN_SUPPORTED(VARIABLE)
# ----------------------
# Add all supported warning parameters to variable VARIABLE.
AC_DEFUN([gl_WARN_SUPPORTED],
[
 FOO=
 # List of all supported warning parameters according to GCC 4.3.2 manual.
 for w in \
   -Wall \
   -W \
   -Wformat-y2k \
   -Wformat-nonliteral \
   -Wformat-security \
   -Winit-self \
   -Wmissing-include-dirs \
   -Wswitch-default \
   -Wswitch-enum \
   -Wunused \
   -Wunknown-pragmas \
   -Wstrict-aliasing \
   -Wstrict-overflow \
   -Wsystem-headers \
   -Wfloat-equal \
   -Wtraditional \
   -Wtraditional-conversion \
   -Wdeclaration-after-statement \
   -Wundef \
   -Wshadow \
   -Wunsafe-loop-optimizations \
   -Wpointer-arith \
   -Wbad-function-cast \
   -Wc++-compat \
   -Wcast-qual \
   -Wcast-align \
   -Wwrite-strings \
   -Wconversion \
   -Wsign-conversion \
   -Wlogical-op \
   -Waggregate-return \
   -Wstrict-prototypes \
   -Wold-style-definition \
   -Wmissing-prototypes \
   -Wmissing-declarations \
   -Wmissing-noreturn \
   -Wmissing-format-attribute \
   -Wpacked \
   -Wpadded \
   -Wredundant-decls \
   -Wnested-externs \
   -Wunreachable-code \
   -Winline \
   -Winvalid-pch \
   -Wlong-long \
   -Wvla \
   -Wvolatile-register-var \
   -Wdisabled-optimization \
   -Wstack-protector \
   -Woverlength-strings \
  ; do
    FOO="$FOO $w"
  done
  $1=$FOO
])

# gl_WARN_COMPLEMENT(OUTVAR, LISTVAR, REMOVEVAR)
# ----------------------
# Copy LISTVAR to OUTVAR except for the entries in REMOVEVAR.
# Elements separated by whitespace.  In set logic terms, the function
# does OUTVAR = LISTVAR \ REMOVEVAR.
AC_DEFUN([gl_WARN_COMPLEMENT],
[
  gl_warn_set=
  set x $2; shift
  for gl_warn_item
  do
    case " $3 " in
      *" $gl_warn_item "*)
        ;;
      *)
        gl_warn_set="$gl_warn_set $gl_warn_item"
        ;;
    esac
  done
  $1=$gl_warn_set
])
