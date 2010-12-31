AC_INIT([GNU libposix],
    m4_esyscmd([./git-version-gen .tarball-version]),
    [bug-gnulib@gnu.org])

AS_BOX([Configuring AC_PACKAGE_TARNAME AC_PACKAGE_VERSION])

dnl this should be AC_REQUIRED by gnulib modules that need it,
dnl but either a couple of modules have forgotten it, or else
dnl AC_REQUIRE is emitting macro expansions out of order
dnl
AC_USE_SYSTEM_EXTENSIONS

dnl we can't use AC_CONFIG_AUX_DIR here, because the heuristics
dnl for finding install-sh in the generated configure script
dnl consider this directory to be a subproject of gnulib proper,
dnl and will only look for install-sh in . and .. :(
dnl AC_CONFIG_AUX_DIR([build-aux])
dnl
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile tests/Makefile])
AC_CONFIG_FILES([lib/version.h])

AM_INIT_AUTOMAKE([foreign])
LT_INIT

# libtool interface versioning for libposix.la
AC_SUBST([LTV_CURRENT], 0)
AC_SUBST([LTV_REVISION], 0)
AC_SUBST([LTV_AGE], 0)
AC_DEFINE_UNQUOTED(GNULIB_LIBPOSIX,1,[we are libposix])

AC_PROG_CC
LIBPOSIX_EARLY
AM_PROG_CC_C_O
LIBPOSIX_INIT

AC_OUTPUT
