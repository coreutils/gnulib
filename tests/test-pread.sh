#!/bin/sh
: ${srcdir=.}
. "$srcdir/init.sh" --set-path=.

fail=0
: | test-pread || fail=1

Exit $fail
