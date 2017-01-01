#!/bin/sh
# This would fail with grep-2.21's dfa.c.

# Copyright 2014-2017 Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

. "${srcdir=.}/init.sh"; path_prepend_ ../src

# Add "." to PATH for the use of dfa-match-aux.
path_prepend_ .

if (type timeout) >/dev/null 2>&1; then
  timeout_10='timeout 10'
else
  timeout_10=
fi

fail=0

dfa-match-aux a ba 0 > out || fail=1
compare /dev/null out || fail=1

in=$(printf "bb\nbb")
$timeout_10 dfa-match-aux a "$in" 1 > out || fail=1
compare /dev/null out || fail=1

Exit $fail
