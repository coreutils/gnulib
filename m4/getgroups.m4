#serial 6

dnl From Jim Meyering.
dnl
dnl Invoking code should check $GETGROUPS_LIB something like this:
dnl  jm_FUNC_GETGROUPS
dnl  test -n "$GETGROUPS_LIB" && LIBS="$GETGROUPS_LIB $LIBS"
dnl

AC_DEFUN([jm_FUNC_GETGROUPS],
[
  AC_REQUIRE([AC_FUNC_GETGROUPS])
  AC_SUBST([GETGROUPS_LIB])
  if test $ac_cv_func_getgroups_works = no; then
    AC_LIBOBJ(getgroups)
    AC_DEFINE(getgroups, rpl_getgroups,
      [Define as rpl_getgroups if getgroups doesn't work right.])
    gl_PREREQ_GETGROUPS
  fi
])

# Prerequisites of lib/getgroups.c.
AC_DEFUN([gl_PREREQ_GETGROUPS],
[
  AC_REQUIRE([AC_TYPE_GETGROUPS])
])
