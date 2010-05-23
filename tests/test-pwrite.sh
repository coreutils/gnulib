#!/bin/sh
. "${srcdir=.}/init.sh"; path_prepend_ .

fail=0
test-pwrite || fail=1

Exit $fail
