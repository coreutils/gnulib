#!/bin/sh
: ${srcdir=.}
. $srcdir/init.sh --set-path=.

fail=0;
echo abc | test-pread || fail=1

Exit $fail
