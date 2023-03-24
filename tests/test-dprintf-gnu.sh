#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-dprintf-gnu.tmp t-dprintf-gnu.out"
${CHECKER} ./test-dprintf-gnu${EXEEXT} > t-dprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-dprintf-gnu.tmp > t-dprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-dprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
