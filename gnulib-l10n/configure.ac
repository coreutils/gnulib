dnl Configuration for the GNU gnulib localizations
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <https://www.gnu.org/licenses/>.

dnl Process this file with autoconf to produce a configure script.

AC_INIT([gnulib-l10n], m4_esyscmd([date -u +"%Y%m%d"]), [bug-gnulib@gnu.org])
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE([1.13 foreign tar-ustar])

dnl Override automake's tar command used for creating distributions.
am__tar='${AMTAR} chf - --format=ustar --owner=root --group=root "$$tardir"'

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.23])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([m4/Makefile])
AC_CONFIG_FILES([po/Makefile.in])

AC_OUTPUT
