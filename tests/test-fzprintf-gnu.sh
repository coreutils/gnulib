#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-fzprintf-gnu.tmp t-fzprintf-gnu.out"
${CHECKER} ./test-fzprintf-gnu${EXEEXT} > t-fzprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-fzprintf-gnu.tmp > t-fzprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-fzprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
