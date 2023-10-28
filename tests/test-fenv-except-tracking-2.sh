#!/bin/sh

# Test that a floating-point operation can trigger a trap.

final_rc=0

for arg in f d l; do
  ${CHECKER} ./test-fenv-except-tracking-2${EXEEXT} $arg
  rc=$?
  if test $rc = 77; then
    final_rc=77
  else
    if test $rc = 0; then
      echo "Failed: ./test-fenv-except-tracking-2 $arg" 1>&2
      exit 1
    fi
  fi
done

exit $final_rc
