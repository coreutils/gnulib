#!/bin/sh

# Test in the POSIX locale.
LC_ALL=C \
${CHECKER} ./test-strncasecmp${EXEEXT} 1 || exit 1
LC_ALL=POSIX \
${CHECKER} ./test-strncasecmp${EXEEXT} 1 || exit 1

exit 0
