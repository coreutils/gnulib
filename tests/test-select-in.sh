#!/bin/sh
# Test select() on file descriptors opened for reading.

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-select-in.tmp"

# Regular files.

rm -f t-select-in.tmp
./test-select-fd${EXEEXT} r 0 t-select-in.tmp < ./test-select-fd${EXEEXT}
test `cat t-select-in.tmp` = "1" || exit 1

# Pipes.

rm -f t-select-in.tmp
{ sleep 1; echo abc; } | ./test-select-fd${EXEEXT} r 0 t-select-in.tmp
test `cat t-select-in.tmp` = "0" || exit 1

rm -f t-select-in.tmp
echo abc | { sleep 1; ./test-select-fd${EXEEXT} r 0 t-select-in.tmp; }
test `cat t-select-in.tmp` = "1" || exit 1

# Special files.

rm -f t-select-in.tmp
./test-select-fd${EXEEXT} r 0 t-select-in.tmp < /dev/null
test `cat t-select-in.tmp` = "1" || exit 1

rm -fr $tmpfiles

exit 0
