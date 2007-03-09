#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-fprintf-posix.tmp"
./test-fprintf-posix${EXEEXT} > t-fprintf-posix.tmp || exit 1

: ${DIFF=diff}
${DIFF} "${srcdir}/test-fprintf-posix.out" t-fprintf-posix.tmp
result=$?

rm -fr $tmpfiles

exit $result
