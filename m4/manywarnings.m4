# manywarnings.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Simon Josefsson

# gl_MANYWARN_COMPLEMENT(OUTVAR, LISTVAR, REMOVEVAR)
# --------------------------------------------------
# Copy LISTVAR to OUTVAR except for the entries in REMOVEVAR.
# Elements separated by whitespace.  In set logic terms, the function
# does OUTVAR = LISTVAR \ REMOVEVAR.
AC_DEFUN([gl_MANYWARN_COMPLEMENT],
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

# gl_MANYWARN_ALL_GCC(VARIABLE)
# -----------------------------
# Add all documented GCC (currently as per version 4.3.2) warning
# parameters to variable VARIABLE.  Note that you need to test them
# using gl_WARN_ADD if you want to make sure your gcc understands it.
AC_DEFUN([gl_MANYWARN_ALL_GCC],
[
 gl_manywarn_set=
 for gl_manywarn_item in \
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
    gl_manywarn_set="$gl_manywarn_set $gl_manywarn_item"
  done
 # The following are not documented in the manual but are included in
 # output from gcc --help=warnings.
 for gl_manywarn_item in \
   -Wattributes \
   -Wcoverage-mismatch \
   -Wmultichar \
   -Wunused-macros \
  ; do
    gl_manywarn_set="$gl_manywarn_set $gl_manywarn_item"
  done
  $1=$gl_manywarn_set
])
