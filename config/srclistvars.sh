# $Id: srclistvars.sh,v 1.27 2006-08-14 18:28:31 eggert Exp $
# Variables for srclist-update and srclist.txt.
# Will change for each user.

# Copyright (C) 2002, 2003, 2004 2005, 2006 Free Software Foundation,
# Inc.

# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
# 02110-1301, USA.

case $LOGNAME in
jas | eggert)
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${COREUTILS=../coreutils}
  : ${GETTEXT=../gettext}
  : ${GNUCONFIG=../config}
  : ${GNUORG=../gnuorg}
  : ${GNUWWWLICENSES=$GNUORG}
  : ${GNUSTANDARDS=../gnustandards}
  : ${LIBTOOL=../libtool}
  : ${LIBCSRC=../libc}
  : ${TEXINFOSRC=../texinfo}
  ;;

karl)
  : ${ACLOCAL=/usr/local/gnu/share/aclocal}
  : ${GNUBIN=/usr/local/gnu/bin}
  : ${GNUWWWLICENSES=$HOME/gnu/www/www/licenses}
  : ${TEXMFROOT=/usr/local/texmf/texmf-dist}
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
  : ${COREUTILS=../coreutils}
  : ${EMACSSRC=../emacs}
  : ${GETTEXT=../gettext}
  : ${GNUCONFIG=../config}
  : ${GNUORG=$HOME/gnu/gnuorg}
  : ${GNULIBSRC=../gnulib}
  : ${GNUSTANDARDS=../gnustandards}
  : ${LIBCSRC=../libc}
  : ${LIBTOOL=../libtool}
  : ${TEXINFOSRC=../texinfo}
  ;;

esac
