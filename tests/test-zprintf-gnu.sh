#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-zprintf-gnu.tmp t-zprintf-gnu.out"
${CHECKER} ./test-zprintf-gnu${EXEEXT} > t-zprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-zprintf-gnu.tmp > t-zprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-zprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
