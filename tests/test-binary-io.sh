#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles output.tmp output2.tmp"
./test-binary-io${EXEEXT} > output.tmp || exit 1

rm -fr $tmpfiles

exit 0
