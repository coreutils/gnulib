#serial 9

dnl From Jim Meyering.
dnl A wrapper around AC_FUNC_MKTIME.

AC_DEFUN([jm_FUNC_MKTIME],
[AC_REQUIRE([AC_FUNC_MKTIME])dnl

 if test $ac_cv_func_working_mktime = no; then
   AC_DEFINE(mktime, rpl_mktime,
    [Define to rpl_mktime if the replacement function should be used.])
 fi
])
