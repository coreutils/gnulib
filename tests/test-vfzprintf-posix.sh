#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vfzprintf-posix.tmp t-vfzprintf-posix.out"
${CHECKER} ./test-vfzprintf-posix${EXEEXT} > t-vfzprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vfzprintf-posix.tmp > t-vfzprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-vfzprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
