#!/bin/sh

# Test of turning floating-point exceptions into traps (signals).

final_rc=0

for operation in 1 2 3 4 5 6 7 8 9; do
  for procedure in p q r s; do
    for type in f d l; do
      ${CHECKER} ./test-fenv-except-trapping-2${EXEEXT} $operation $procedure $type
      rc=$?
      if test $rc = 77; then
        if test $final_rc = 0; then
          final_rc=77
        fi
      else
        if test $procedure = s; then
          if test $rc != 0; then
            echo "Failed (got a trap): ./test-fenv-except-trapping-2 $operation $procedure $type" 1>&2
            final_rc=1
          fi
        else
          if test $rc = 0; then
            echo "Failed (got no trap): ./test-fenv-except-trapping-2 $operation $procedure $type" 1>&2
            final_rc=1
          fi
        fi
      fi
    done
  done
done

exit $final_rc
