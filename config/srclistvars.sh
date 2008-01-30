# $Id: srclistvars.sh,v 1.28 2006-08-22 19:38:57 eggert Exp $
# Variables for srclist-update and srclist.txt.
# Will change for each user.

# Copyright (C) 2002, 2003, 2004 2005, 2006, 2008
# Free Software Foundation, Inc.

# This file is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

case $LOGNAME in
jas | eggert)
  : ${AUTOCONF=../autoconf}
  : ${AUTOMAKE=../automake}
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
  : ${EMACSSRC=../etrunk}
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
