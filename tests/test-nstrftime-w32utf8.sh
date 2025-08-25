#!/bin/sh

# Test the UTF-8 environment on native Windows.
unset LC_ALL
unset LC_CTYPE
unset LC_MESSAGES
unset LC_NUMERIC
unset LC_COLLATE
unset LC_MONETARY
unset LC_TIME
unset LANG
LC_ALL=French_France.65001  ${CHECKER} ./test-nstrftime-w32utf8${EXEEXT} 1 || exit $?
LC_ALL=Japanese_Japan.65001 ${CHECKER} ./test-nstrftime-w32utf8${EXEEXT} 2 || exit $?
exit 0
