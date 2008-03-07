#!/bin/sh

# Execute the test only with seekable input stream.
# The behaviour of fflush() on a non-seekable input stream is undefined.
./test-fflush2${EXEEXT} < "$srcdir/test-fflush2.sh" || exit $?
#cat "$srcdir/test-fflush2.sh" | ./test-fflush2${EXEEXT} || exit $?

exit 0
