#!/bin/sh

# Test that feraiseexcept() can trigger a trap.

final_rc=0

${CHECKER} ./test-fenv-except-tracking-3${EXEEXT}
rc=$?
if test $rc = 77; then
  final_rc=77
else
  if test $rc = 0; then
    echo "Failed: ./test-fenv-except-tracking-3" 1>&2
    exit 1
  fi
fi

exit $final_rc
