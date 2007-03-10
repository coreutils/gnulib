#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-printf-posix.tmp"
./test-printf-posix${EXEEXT} > t-printf-posix.tmp || exit 1

: ${DIFF=diff}
${DIFF} "${srcdir}/test-fprintf-posix.out" t-printf-posix.tmp
result=$?

rm -fr $tmpfiles

exit $result
