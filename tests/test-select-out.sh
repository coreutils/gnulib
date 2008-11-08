#!/bin/sh
# Test select() on file descriptors opened for writing.

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-select-out.out t-select-out.tmp"

# Regular files.

./test-select-fd${EXEEXT} w 1 > t-select-out.out 2> t-select-out.tmp
test `cat t-select-out.tmp` = "1" || exit 1

# Pipes.

( { echo abc; ./test-select-fd${EXEEXT} w 1; } | { sleep 1; cat; } ) > /dev/null 2> t-select-out.tmp
test `cat t-select-out.tmp` = "0" || exit 1

( { sleep 1; echo abc; ./test-select-fd${EXEEXT} w 1; } | cat) > /dev/null 2> t-select-out.tmp
test `cat t-select-out.tmp` = "1" || exit 1

# Special files.

./test-select-fd${EXEEXT} w 1 > /dev/null 2> t-select-out.tmp
test `cat t-select-out.tmp` = "1" || exit 1

rm -fr $tmpfiles

exit 0
