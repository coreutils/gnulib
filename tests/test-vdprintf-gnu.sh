#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vdprintf-gnu.tmp t-vdprintf-gnu.out"
${CHECKER} ./test-vdprintf-gnu${EXEEXT} > t-vdprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vdprintf-gnu.tmp > t-vdprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vdprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
