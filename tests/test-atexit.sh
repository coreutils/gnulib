#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-atexit.tmp"
# Check that an atexit handler is called when main() returns normally.
echo > t-atexit.tmp
./test-atexit${EXEEXT}
if test -f t-atexit.tmp; then
  exit 1
fi

# Check that an atexit handler is called when the program is left
# through exit(0).
echo > t-atexit.tmp
./test-atexit${EXEEXT} 0
if test -f t-atexit.tmp; then
  exit 1
fi

# Check that an atexit handler is called when the program is left
# through exit(1).
echo > t-atexit.tmp
./test-atexit${EXEEXT} 1
if test -f t-atexit.tmp; then
  exit 1
fi

rm -fr $tmpfiles

exit 0
