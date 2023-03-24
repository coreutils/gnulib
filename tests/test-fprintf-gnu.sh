#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-fprintf-gnu.tmp t-fprintf-gnu.out"
${CHECKER} ./test-fprintf-gnu${EXEEXT} > t-fprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-fprintf-gnu.tmp > t-fprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-fprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
