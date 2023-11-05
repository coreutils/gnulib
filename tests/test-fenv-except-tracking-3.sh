#!/bin/sh

# Test that feraiseexcept() can trigger a trap.

final_rc=0

for arg in FE_INVALID FE_DIVBYZERO FE_OVERFLOW FE_UNDERFLOW FE_INEXACT; do
  ${CHECKER} ./test-fenv-except-tracking-3${EXEEXT} $arg
  rc=$?
  if test $rc = 77; then
    final_rc=77
  else
    if test $rc = 0; then
      echo "Failed: ./test-fenv-except-tracking-3 $arg" 1>&2
      exit 1
    fi
  fi
done

exit $final_rc
