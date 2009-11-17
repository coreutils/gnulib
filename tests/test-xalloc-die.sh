#!/bin/sh
# Test suite for xalloc_die.
# Copyright (C) 2009 Free Software Foundation, Inc.
# This file is part of the GNUlib Library.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
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

tmpfiles=""
trap '__st=$?; rm -fr $tmpfiles; exit $__st' 0
trap '__st=$?; (exit $__st); exit $__st' 1 2 3 15

if ( diff --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { diff -u "$@"; }
elif ( cmp --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { cmp -s "$@"; }
else
  compare() { cmp "$@"; }
fi

tmpout=t-xalloc-die.tmp-stderr
tmperr=t-xalloc-die.tmp-stdout
tmpfiles="$tmpout $tmperr ${tmperr}2"

PATH=".:$PATH"
export PATH
test-xalloc-die${EXEEXT} 2> ${tmperr} > ${tmpout}
case $? in
  1) ;;
  *) (exit 1); exit 1 ;;
esac

tr -d '\015' < $tmperr > ${tmperr}2 || { (exit 1); exit 1; }

compare - ${tmperr}2 <<\EOF || { (exit 1); exit 1; }
test-xalloc-die: memory exhausted
EOF

test -s $tmpout && { (exit 1); exit 1; }

rm -fr $tmpfiles

exit 0
