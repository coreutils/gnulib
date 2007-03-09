#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-vfprintf-posix.tmp"
./test-vfprintf-posix${EXEEXT} > t-vfprintf-posix.tmp || exit 1

: ${DIFF=diff}
${DIFF} "${srcdir}/test-fprintf-posix.out" t-vfprintf-posix.tmp
result=$?

rm -fr $tmpfiles

exit $result
