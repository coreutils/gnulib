#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-printf-gnu.tmp t-printf-gnu.out"
${CHECKER} ./test-printf-gnu${EXEEXT} > t-printf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-printf-gnu.tmp > t-printf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-printf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
