#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-dzprintf-gnu.tmp t-dzprintf-gnu.out"
${CHECKER} ./test-dzprintf-gnu${EXEEXT} > t-dzprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-dzprintf-gnu.tmp > t-dzprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-dzprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
