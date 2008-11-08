#!/bin/sh
# Test select() on file descriptors opened for reading.

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-select-in.tmp"

# Regular files.

./test-select-fd${EXEEXT} r 0 < ./test-select-fd${EXEEXT} 2> t-select-in.tmp
test `cat t-select-in.tmp` = "1" || exit 1

# Pipes.

{ sleep 1; echo abc; } | ./test-select-fd${EXEEXT} r 0 2> t-select-in.tmp
test `cat t-select-in.tmp` = "0" || exit 1

echo abc | { sleep 1; ./test-select-fd${EXEEXT} r 0; } 2> t-select-in.tmp
test `cat t-select-in.tmp` = "1" || exit 1

# Special files.

./test-select-fd${EXEEXT} r 0 < /dev/null 2> t-select-in.tmp
test `cat t-select-in.tmp` = "1" || exit 1

rm -fr $tmpfiles

exit 0
