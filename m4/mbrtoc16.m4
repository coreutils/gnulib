# mbrtoc16.m4
# serial 4
dnl Copyright (C) 2014-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_MBRTOC16],
[
  AC_REQUIRE([gl_UCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  dnl Determine REPLACE_MBSTATE_T, from which GNULIB_defined_mbstate_t is
  dnl determined.  It describes how our overridden mbrtowc is implemented.
  dnl We then implement mbrtoc16 accordingly.
  AC_REQUIRE([gl_MBSTATE_T_BROKEN])

  AC_REQUIRE([gl_TYPE_CHAR16_T])
  AC_REQUIRE([gl_MBRTOC16_SANITYCHECK])

  AC_REQUIRE([gl_CHECK_FUNC_MBRTOC16])
  if test $gl_cv_func_mbrtoc16 = no; then
    HAVE_MBRTOC16=0
  else
    if test $GNULIBHEADERS_OVERRIDE_CHAR16_T = 1 || test $REPLACE_MBSTATE_T = 1; then
      REPLACE_MBRTOC16=1
    else
      gl_MBRTOC16_NULL_DESTINATION
      gl_MBRTOC16_RETVAL
      gl_MBRTOC16_EMPTY_INPUT
      gl_MBRTOC16_C_LOCALE
      case "$gl_cv_func_mbrtoc16_null_destination" in
        *yes) ;;
        *) REPLACE_MBRTOC16=1 ;;
      esac
      case "$gl_cv_func_mbrtoc16_retval" in
        *yes) ;;
        *) REPLACE_MBRTOC16=1 ;;
      esac
      case "$gl_cv_func_mbrtoc16_empty_input" in
        *yes) ;;
        *) REPLACE_MBRTOC16=1 ;;
      esac
      case "$gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ" in
        *yes) ;;
        *) REPLACE_MBRTOC16=1 ;;
      esac
    fi
    if test $HAVE_WORKING_MBRTOC16 = 0; then
      REPLACE_MBRTOC16=1
    fi
  fi
])

AC_DEFUN([gl_CHECK_FUNC_MBRTOC16],
[
  dnl Cf. gl_CHECK_FUNCS_ANDROID
  AC_CHECK_DECL([mbrtoc16], , ,
    [[#ifdef __HAIKU__
       #include <stdint.h>
      #endif
      #include <uchar.h>
    ]])
  if test $ac_cv_have_decl_mbrtoc16 = yes; then
    AC_CHECK_FUNCS([mbrtoc16])
    gl_cv_func_mbrtoc16="$ac_cv_func_mbrtoc16"
  else
    gl_cv_func_mbrtoc16=no
  fi
])

AC_DEFUN([gl_MBRTOC16_NULL_DESTINATION],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_TYPE_CHAR16_T])
  AC_REQUIRE([gt_LOCALE_EN_UTF8])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether mbrtoc16 supports a NULL destination],
    [gl_cv_func_mbrtoc16_null_destination],
    [
      dnl Initial guess, used when cross-compiling or when no suitable locale
      dnl is present.
changequote(,)dnl
      case "$host_os" in
          # Guess no on glibc systems, on OpenBSD, and on Android.
        *-gnu* | gnu* | openbsd* | *-android*)
          gl_cv_func_mbrtoc16_null_destination="guessing no" ;;
          # Guess yes otherwise.
        *)
          gl_cv_func_mbrtoc16_null_destination="guessing yes" ;;
      esac
changequote([,])dnl
      if test "$LOCALE_EN_UTF8" != none; then
        AC_RUN_IFELSE(
          [AC_LANG_SOURCE([[
             #include <locale.h>
             #include <string.h>
             #ifdef __HAIKU__
              #include <stdint.h>
             #endif
             #include <uchar.h>
             int
             main (void)
             {
               if (setlocale (LC_ALL, "$LOCALE_EN_UTF8") == NULL
                 return 1;
               mbstate_t state;
               size_t ret;
               char input[] = "\360\237\230\213"; /* U+1F60B */
               memset (&state, '\0', sizeof (mbstate_t));
               ret = mbrtoc16 (NULL, input, 4, &state);
               if (ret != 4)
                 return 2;
               ret = mbrtoc16 (NULL, input + 4, 0, &state);
               if (ret != (size_t)(-3))
                 return 3;
               return 0;
             }]])],
          [gl_cv_func_mbrtoc16_null_destination=yes],
          [gl_cv_func_mbrtoc16_null_destination=no],
          [:])
      fi
    ])
])

dnl Test whether mbrtoc16, when parsing the end of a multibyte character,
dnl correctly returns the number of bytes that were needed to complete the
dnl character (not the total number of bytes of the multibyte character).
dnl Also test whether mbrtoc16 returns a byte count when it has stored a
dnl high surrogate and (size_t) -3 when it has stored a low surrogate.
dnl Result is gl_cv_func_mbrtoc16_retval.

AC_DEFUN([gl_MBRTOC16_RETVAL],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CACHE_CHECK([whether mbrtoc16 has a correct return value],
    [gl_cv_func_mbrtoc16_retval],
    [
      dnl Initial guess, used when cross-compiling or when no suitable locale
      dnl is present.
changequote(,)dnl
      case "$host_os" in
                           # Guess no on Android.
        linux*-android*)   gl_cv_func_mbrtoc16_retval="guessing no" ;;
                           # Guess no on native Windows.
        mingw* | windows*) gl_cv_func_mbrtoc16_retval="guessing no" ;;
                           # Guess yes otherwise.
        *)                 gl_cv_func_mbrtoc16_retval="guessing yes" ;;
      esac
changequote([,])dnl
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <locale.h>
#include <string.h>
#include <wchar.h>
#ifdef __HAIKU__
 #include <stdint.h>
#endif
#include <uchar.h>
int main ()
{
  int result = 0;
  int found_some_locale = 0;
  /* This fails on Android.  */
  if (setlocale (LC_ALL, "en_US.UTF-8") != NULL)
    {
      char input[] = "\360\237\230\213"; /* U+1F60B */
      mbstate_t state;
      char16_t wc;

      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtoc16 (&wc, input, 4, &state) != 4
          || mbrtoc16 (&wc, input + 4, 0, &state) != (size_t)(-3))
        result |= 1;
      found_some_locale = 1;
    }
  /* This fails on native Windows.  */
  if (setlocale (LC_ALL, "Japanese_Japan.932") != NULL)
    {
      char input[] = "<\223\372\226\173\214\352>"; /* "<日本語>" */
      mbstate_t state;
      char16_t wc;

      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtoc16 (&wc, input + 3, 1, &state) == (size_t)(-2))
        {
          input[3] = '\0';
          if (mbrtoc16 (&wc, input + 4, 4, &state) != 1)
            result |= 2;
        }
      found_some_locale = 1;
    }
  if (setlocale (LC_ALL, "Chinese_Taiwan.950") != NULL)
    {
      char input[] = "<\244\351\245\273\273\171>"; /* "<日本語>" */
      mbstate_t state;
      char16_t wc;

      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtoc16 (&wc, input + 3, 1, &state) == (size_t)(-2))
        {
          input[3] = '\0';
          if (mbrtoc16 (&wc, input + 4, 4, &state) != 1)
            result |= 4;
        }
      found_some_locale = 1;
    }
  if (setlocale (LC_ALL, "Chinese_China.936") != NULL)
    {
      char input[] = "<\310\325\261\276\325\132>"; /* "<日本語>" */
      mbstate_t state;
      char16_t wc;

      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtoc16 (&wc, input + 3, 1, &state) == (size_t)(-2))
        {
          input[3] = '\0';
          if (mbrtoc16 (&wc, input + 4, 4, &state) != 1)
            result |= 8;
        }
      found_some_locale = 1;
    }
  return (found_some_locale ? result : 77);
}]])],
        [gl_cv_func_mbrtoc16_retval=yes],
        [if test $? != 77; then
           gl_cv_func_mbrtoc16_retval=no
         fi
        ],
        [:])
    ])
])

dnl Test whether mbrtoc16 returns the correct value on empty input.

AC_DEFUN([gl_MBRTOC16_EMPTY_INPUT],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether mbrtoc16 works on empty input],
    [gl_cv_func_mbrtoc16_empty_input],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
           #ifdef __HAIKU__
            #include <stdint.h>
           #endif
           #include <uchar.h>
           static char16_t wc;
           static mbstate_t mbs;
           int
           main (void)
           {
             return mbrtoc16 (&wc, "", 0, &mbs) != (size_t) -2;
           }]])],
        [gl_cv_func_mbrtoc16_empty_input=yes],
        [gl_cv_func_mbrtoc16_empty_input=no],
        [case "$host_os" in
                            # Guess no on glibc systems.
           *-gnu* | gnu*)   gl_cv_func_mbrtoc16_empty_input="guessing no" ;;
                            # Guess no on Android.
           linux*-android*) gl_cv_func_mbrtoc16_empty_input="guessing no" ;;
           *)               gl_cv_func_mbrtoc16_empty_input="guessing yes" ;;
         esac
        ])
    ])
])

dnl <https://pubs.opengroup.org/onlinepubs/9699919799/functions/mbrtowc.html>
dnl POSIX:2018 says regarding mbrtowc: "In the POSIX locale an [EILSEQ] error
dnl cannot occur since all byte values are valid characters."  It is reasonable
dnl to expect mbrtoc16 to behave in the same way.

AC_DEFUN([gl_MBRTOC16_C_LOCALE],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether the C locale is free of encoding errors],
    [gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <limits.h>
            #include <locale.h>
            #ifdef __HAIKU__
             #include <stdint.h>
            #endif
            #include <uchar.h>
          ]], [[
            int i;
            char *locale = setlocale (LC_ALL, "C");
            if (! locale)
              return 2;
            for (i = CHAR_MIN; i <= CHAR_MAX; i++)
              {
                char c = i;
                char16_t wc;
                mbstate_t mbs = { 0, };
                size_t ss = mbrtoc16 (&wc, &c, 1, &mbs);
                if (1 < ss)
                  return 3;
              }
            return 0;
          ]])],
       [gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ=yes],
       [gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ=no],
       [case "$host_os" in
                             # Guess yes on native Windows.
          mingw* | windows*) gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ="guessing yes" ;;
          *)                 gl_cv_func_mbrtoc16_C_locale_sans_EILSEQ="$gl_cross_guess_normal" ;;
        esac
       ])
    ])
])

dnl Test whether mbrtoc16 works not worse than mbrtowc.
dnl Result is HAVE_WORKING_MBRTOC16.

AC_DEFUN([gl_MBRTOC16_SANITYCHECK],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_TYPE_CHAR16_T])
  AC_REQUIRE([gl_CHECK_FUNC_MBRTOC16])
  AC_REQUIRE([gt_LOCALE_FR])
  AC_REQUIRE([gt_LOCALE_ZH_CN])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  if test $GNULIBHEADERS_OVERRIDE_CHAR16_T = 1 || test $gl_cv_func_mbrtoc16 = no; then
    HAVE_WORKING_MBRTOC16=0
  else
    AC_CACHE_CHECK([whether mbrtoc16 works as well as mbrtowc],
      [gl_cv_func_mbrtoc16_sanitycheck],
      [
        dnl Initial guess, used when cross-compiling or when no suitable locale
        dnl is present.
changequote(,)dnl
        case "$host_os" in
          # Guess no on FreeBSD, Solaris, native Windows.
          freebsd* | midnightbsd* | solaris* | mingw* | windows*)
            gl_cv_func_mbrtoc16_sanitycheck="guessing no"
            ;;
          # Guess yes otherwise.
          *)
            gl_cv_func_mbrtoc16_sanitycheck="guessing yes"
            ;;
        esac
changequote([,])dnl
        if test $LOCALE_FR != none || test $LOCALE_ZH_CN != none; then
          AC_RUN_IFELSE(
            [AC_LANG_SOURCE([[
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef __HAIKU__
 #include <stdint.h>
#endif
#include <uchar.h>
int main ()
{
  int result = 0;
  /* This fails on MSVC:
     mbrtoc16 returns (size_t)-1.
     mbrtowc returns 1 (correct).  */
  if (strcmp ("$LOCALE_FR", "none") != 0
      && setlocale (LC_ALL, "$LOCALE_FR") != NULL)
    {
      mbstate_t state;
      wchar_t wc = (wchar_t) 0xBADF;
      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtowc (&wc, "\374", 1, &state) == 1)
        {
          char16_t c16 = (wchar_t) 0xBADF;
          memset (&state, '\0', sizeof (mbstate_t));
          if (mbrtoc16 (&c16, "\374", 1, &state) != 1)
            result |= 1;
        }
    }
  /* This fails on FreeBSD 13.2 and Solaris 11.4:
     mbrtoc16 returns (size_t)-2 or (size_t)-1.
     mbrtowc returns 4 (correct).  */
  if (strcmp ("$LOCALE_ZH_CN", "none") != 0
      && setlocale (LC_ALL, "$LOCALE_ZH_CN") != NULL)
    {
      mbstate_t state;
      wchar_t wc = (wchar_t) 0xBADF;
      memset (&state, '\0', sizeof (mbstate_t));
      if (mbrtowc (&wc, "\224\071\375\067", 4, &state) == 4)
        {
          char16_t c16 = (wchar_t) 0xBADF;
          memset (&state, '\0', sizeof (mbstate_t));
          if (mbrtoc16 (&c16, "\224\071\375\067", 4, &state) != 4)
            result |= 2;
        }
    }
  return result;
}]])],
            [gl_cv_func_mbrtoc16_sanitycheck=yes],
            [gl_cv_func_mbrtoc16_sanitycheck=no],
            [:])
        fi
      ])
    case "$gl_cv_func_mbrtoc16_sanitycheck" in
      *yes)
        HAVE_WORKING_MBRTOC16=1
        AC_DEFINE([HAVE_WORKING_MBRTOC16], [1],
          [Define if the mbrtoc16 function basically works.])
        ;;
      *) HAVE_WORKING_MBRTOC16=0 ;;
    esac
  fi
  AC_SUBST([HAVE_WORKING_MBRTOC16])
])

# Prerequisites of lib/mbrtoc16.c.
AC_DEFUN([gl_PREREQ_MBRTOC16], [
  :
])
