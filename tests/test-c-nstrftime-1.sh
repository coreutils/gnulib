#!/bin/sh

LC_ALL=C ${CHECKER} ./test-c-nstrftime${EXEEXT} || exit 1

exit 0
