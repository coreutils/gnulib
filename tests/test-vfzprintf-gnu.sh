#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vfzprintf-gnu.tmp t-vfzprintf-gnu.out"
${CHECKER} ./test-vfzprintf-gnu${EXEEXT} > t-vfzprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vfzprintf-gnu.tmp > t-vfzprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vfzprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
