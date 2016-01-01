# Variables for srclist-update and srclist.txt.
# Will likely change for each user.

# Copyright 2002-2016 Free Software Foundation, Inc.

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

# per-user overrides.
case $LOGNAME in
karl)
  : ${ACLOCAL=/usr/local/gnu/share/aclocal}
  : ${GETTEXT=/usr/local/gnu/src/dist-gettext}
  : ${GNUBIN=/usr/local/gnu/bin}
  : ${GNUORG=$HOME/gnu/gnuorg}
  : ${GNUWWWLICENSES=$HOME/gnu/www/www/licenses}
  : ${TEXMFROOT=/usr/local/texmf/texmf-dist}
  ;;
esac

# Default to sibling (of parent) directories.
: ${AUTOCONF=../autoconf}
: ${AUTOMAKE=../automake}
: ${GETTEXT=../gettext}
: ${GNUCONFIG=../config}
: ${GNULIBSRC=../gnulib}
: ${GNUORG=../gnuorg}
: ${GNUSTANDARDS=../gnustandards}
: ${GNUWWWLICENSES=$GNUORG}
: ${LIBCSRC=../libc}
: ${LIBTOOL=../libtool}
: ${TEXINFOSRC=../texinfo}
