#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vfprintf-gnu.tmp t-vfprintf-gnu.out"
${CHECKER} ./test-vfprintf-gnu${EXEEXT} > t-vfprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vfprintf-gnu.tmp > t-vfprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vfprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
