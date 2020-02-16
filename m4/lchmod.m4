#serial 5

dnl Copyright (C) 2005-2006, 2008-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Paul Eggert.
dnl Provide a replacement for lchmod on hosts that lack a working version.

AC_DEFUN([gl_FUNC_LCHMOD],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])

  dnl Persuade glibc <sys/stat.h> to declare lchmod().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  AC_CHECK_FUNCS_ONCE([fchmodat lchmod lstat])
  if test "$ac_cv_func_lchmod" = no; then
    HAVE_LCHMOD=0
  else
    AC_CACHE_CHECK([whether lchmod works on non-symlinks],
      [gl_cv_func_lchmod_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [
              AC_INCLUDES_DEFAULT[
              #ifndef S_IRUSR
               #define S_IRUSR 0400
              #endif
              #ifndef S_IWUSR
               #define S_IWUSR 0200
              #endif
              #ifndef S_IRWXU
               #define S_IRWXU 0700
              #endif
              #ifndef S_IRWXG
               #define S_IRWXG 0070
              #endif
              #ifndef S_IRWXO
               #define S_IRWXO 0007
              #endif
            ]],
            [[
              int permissive = S_IRWXU | S_IRWXG | S_IRWXO;
              int desired = S_IRUSR | S_IWUSR;
              static char const f[] = "conftest.lchmod";
              struct stat st;
              if (creat (f, permissive) < 0)
                return 1;
              if (lchmod (f, desired) != 0)
                return 1;
              if (stat (f, &st) != 0)
                return 1;
              return ! ((st.st_mode & permissive) == desired);
            ]])],
         [gl_cv_func_lchmod_works=yes],
         [gl_cv_func_lchmod_works=no],
         [case "$host_os" in
            dnl Guess no on Linux with glibc, yes otherwise.
            linux-gnu*) gl_cv_func_lchmod_works="guessing no" ;;
            *)          gl_cv_func_lchmod_works="$gl_cross_guess_normal" ;;
          esac
         ])
       rm -f conftest.lchmod])
    case $gl_cv_func_lchmod_works in
      *yes) ;;
      *) REPLACE_LCHMOD=1;;
    esac
  fi
])
