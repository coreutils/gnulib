#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vzprintf-gnu.tmp t-vzprintf-gnu.out"
${CHECKER} ./test-vzprintf-gnu${EXEEXT} > t-vzprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vzprintf-gnu.tmp > t-vzprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vzprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
