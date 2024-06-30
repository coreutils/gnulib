#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-fzprintf-posix.tmp t-fzprintf-posix.out"
${CHECKER} ./test-fzprintf-posix${EXEEXT} > t-fzprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-fzprintf-posix.tmp > t-fzprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-fzprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
