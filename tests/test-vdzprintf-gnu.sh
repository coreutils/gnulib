#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vdzprintf-gnu.tmp t-vdzprintf-gnu.out"
${CHECKER} ./test-vdzprintf-gnu${EXEEXT} > t-vdzprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vdzprintf-gnu.tmp > t-vdzprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vdzprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
