#!/bin/sh
. "${srcdir=.}/init.sh"; path_prepend_ .

fail=0
: | test-pread || fail=1

Exit $fail
